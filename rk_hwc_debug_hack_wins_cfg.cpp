/*  --------------------------------------------------------------------------------------------------------
 *  File:   rk_hwc_debug_hack_wins_cfg.cpp
 *
 *  Desc:
 *
 *  Note:
 *
 *  Author: ChenZhen
 *
 *  Log:
	----Thu Oct 20 17:17:26 2016            init_ver.
 */


/* ---------------------------------------------------------------------------------------------------------
 * Include Files
 * ---------------------------------------------------------------------------------------------------------
 */

/*
#define ENABLE_DEBUG_LOG
#define ENABLE_VERBOSE_LOG
*/
#include <log/custom_log.h>

#include <sys/socket.h>
#include <linux/netlink.h>

#include "rk_hwc_debug_utils.h"
#include "rk_hwc_debug_hack_wins_cfg.h"


#ifdef ENABLE_HACK_WINS_CFG_FOR_CONFIG_DONE

/* ---------------------------------------------------------------------------------------------------------
 * Local Macros
 * ---------------------------------------------------------------------------------------------------------
 */


/* ---------------------------------------------------------------------------------------------------------
 * Local Typedefs
 * ---------------------------------------------------------------------------------------------------------
 */

/**
 * .DP : vop_exception, ve : vop 硬件报告的异常, 通常以中断的形式, 比如 intr_post_buf_empty.
 */
typedef int vop_exception_t;
#define VE_NONE                 ( (vop_exception_t)0)   // "VE" : Vop Exception.
#define VE_POST_EMPTY_BUFFER    ( (vop_exception_t)1)

typedef struct rk_hwc_debug_facility_t
{
    bool m_inited;

    /*-------------------------------------------------------*/
    /**
     * vop_exception_monitor_thread.
     */
    pthread_t m_ve_monitor_thread;
    /**
     * current_vop_exception.
     */
    vop_exception_t m_ve;
    /**
     * 用于实现 ve_monitor_thread 和 main_thread 对 m_ve 的互斥访问.
     */
    pthread_mutex_t m_mutex_for_ve;
    /**
     * 用于实现从 ve_monitor_thread 向 main_thread 发送 ve_event 的条件变量.
     */
    pthread_cond_t m_cond_for_ve;
    /*-------------------------------------------------------*/

}   rk_hwc_debug_facility_t;

/* ---------------------------------------------------------------------------------------------------------
 * External Function Prototypes (referenced in this file)
 * ---------------------------------------------------------------------------------------------------------
 */

extern void dump_content_of_layers_to_file(hwc_display_contents_1_t* list);

/* ---------------------------------------------------------------------------------------------------------
 * Local Function Prototypes
 * ---------------------------------------------------------------------------------------------------------
 */

static void leave_afbc_area_only(struct rk_fb_win_cfg_data* wins_cfg);

static void set_safe_values_to_pos_and_size_fields(struct rk_fb_area_par* area_par);

static void set_suspect_invalid_values_to_pos_and_size_fieles(struct rk_fb_area_par* area_par);

static void hack_wins_cfg_with_only_one_afbc_area(struct rk_fb_win_cfg_data* wins_cfg);

static void invalidate_area_par(struct rk_fb_area_par* area_par, void* modifier_data);

static void invalidate_all_afbc_area_pars(struct rk_fb_win_cfg_data* wins_cfg);

/*---------------------------------------------------------------------------*/

static int rk_hwc_debug_facility_init(rk_hwc_debug_facility_t* facility);

static void rk_hwc_debug_facility_term(rk_hwc_debug_facility_t* facility);

/*---------------------------------------------------------------------------*/

static int getUeventSocket(int* pFd);

static int64_t getSystemTime();

static void notify_ve_event(rk_hwc_debug_facility_t* facility, vop_exception_t ve);

static vop_exception_t get_ve_from_uevent(rk_hwc_debug_facility_t* debug_facility, char* uevent_msg);

static void get_timeval_from_ms(int64_t in_ms, struct timeval* time_val);

static void* wait_uevent_of_ve_with_timeout(void* data);

static int start_ve_monitor_thread(rk_hwc_debug_facility_t* debug_facility);

static vop_exception_t wait_ve_event(rk_hwc_debug_facility_t* facility);

static bool is_y_act_unaccepted(u16 y_act);

static bool is_y_pos_unaccepted(u16 y_pos);

static void test_y_size_and_the_same_y_act_of_afbc_area(hwc_context_t* context,
                                                        hwc_display_contents_1_t* contents,
                                                        struct rk_fb_win_cfg_data* wins_cfg);

static void test_x_act_and_the_same_x_size_of_afbc_area(hwc_context_t* context,
                                                        hwc_display_contents_1_t* contents,
                                                        struct rk_fb_win_cfg_data* wins_cfg);

static void test_x_pos_of_afbc_area(hwc_context_t* context,
                                    hwc_display_contents_1_t* contents,
                                    struct rk_fb_win_cfg_data* wins_cfg);

static void test_y_pos_of_afbc_area(hwc_context_t* context,
                                    hwc_display_contents_1_t* contents,
                                    struct rk_fb_win_cfg_data* wins_cfg);

static void test_pos_and_size_par_of_afbc_area(hwc_context_t* context,
                                               hwc_display_contents_1_t* contents,
                                               struct rk_fb_win_cfg_data* wins_cfg);
/* ---------------------------------------------------------------------------------------------------------
 * Local Variables
 * ---------------------------------------------------------------------------------------------------------
 */

static rk_hwc_debug_facility_t s_debug_facility;

/* ---------------------------------------------------------------------------------------------------------
 * Global Variables
 * ---------------------------------------------------------------------------------------------------------
 */


/* ---------------------------------------------------------------------------------------------------------
 * Global Functions Implementation
 * ---------------------------------------------------------------------------------------------------------
 */

/**
 * 定制的, 篡改 wins_cfg 内容的 debug 用函数.
 *
 * @param context
 *      当前的 hwc_context_of_current_display_device.
 *      实际上, 是本函数的 功能扩展了, 将甚至执行对 rk_fb_dev 的 ioctl RK_FBIOSET_CONFIG_DONE, 需要 rk_fb_dev 的 fd.
 *      函数和 property 名称也已经不合适了.
 */
void hack_wins_cfg_for_config_done(hwc_context_t* context,
                                   hwc_display_contents_1_t* contents,
                                   struct rk_fb_win_cfg_data* wins_cfg)
{
    int ret = 0;
    rk_hwc_debug_facility_t* facility = &s_debug_facility;

    CHECK_FUNC_CALL( rk_hwc_debug_facility_init(facility) , ret, EXIT);

    /*-------------------------------------------------------*/

    // leave_afbc_area_only(wins_cfg);
    // hack_wins_cfg_with_only_one_afbc_area(wins_cfg);

    // invalidate_all_afbc_area_pars(wins_cfg);

    test_pos_and_size_par_of_afbc_area(context, contents, wins_cfg);

    /*-------------------------------------------------------*/

    rk_hwc_debug_facility_term(facility);

EXIT:
    return;
}


/* ---------------------------------------------------------------------------------------------------------
 * Local Functions Implementation
 * ---------------------------------------------------------------------------------------------------------
 */

static void leave_afbc_area_only(struct rk_fb_win_cfg_data* wins_cfg)
{
    struct rk_fb_win_cfg_data buffer;
    struct rk_fb_win_cfg_data* artificial = &buffer;

    memset(artificial, 0, sizeof(*artificial) );

    const struct rk_fb_win_par* win_par_with_afbc_area = get_first_win_par_with_afbc_area(wins_cfg);
    if ( NULL != win_par_with_afbc_area )
    {
        artificial->wait_fs = wins_cfg->wait_fs;
        artificial->ret_fence_fd = wins_cfg->ret_fence_fd;
        memcpy(artificial->rel_fence_fd, wins_cfg->rel_fence_fd, sizeof(wins_cfg->rel_fence_fd) );

        memcpy(&(artificial->win_par[0] ), win_par_with_afbc_area, sizeof(*win_par_with_afbc_area) );

        memcpy(wins_cfg, artificial, sizeof(*artificial) );
    }
}

/**
 * 将确认是 safe 的 values 设置到指定 area_par 的 pos_and_size_fields 中.
 */
static void set_safe_values_to_pos_and_size_fields(struct rk_fb_area_par* area_par)
{
    set_x_offset(area_par, 0);
    set_y_offset(area_par, 0);

    set_x_pos(area_par, 0);
    set_y_pos(area_par, 0);

    set_x_size(area_par, 1200);
    set_y_size(area_par, 1920);

    set_x_act(area_par, 1200);
    set_y_act(area_par, 1920);

    set_x_vir(area_par, 1200);
    set_y_vir(area_par, 1920);
}

/**
 * 将有 可能的 invalid 的 values, 设置到 area_par 的 pos_and_size_fields 中.
 */
static void set_suspect_invalid_values_to_pos_and_size_fieles(struct rk_fb_area_par* area_par)
{
    set_x_size(area_par, 1200);
    set_y_size(area_par, 1284);

    set_x_act(area_par, 1200);
    set_y_act(area_par, 1284);
}

/**
 * 对仅仅只有一个 area 且是 afbc_area 的 wins_cfg 数据, 进行 hack.
 */
static void hack_wins_cfg_with_only_one_afbc_area(struct rk_fb_win_cfg_data* wins_cfg)
{
    struct rk_fb_area_par* area_par = get_first_afbc_area(wins_cfg);

    if ( NULL == area_par )
    {
        W("no afbc_area.");
        return;
    }

    /*-------------------------------------------------------*/
    // set_safe_values_to_pos_and_size_fields(area_par);

    set_suspect_invalid_values_to_pos_and_size_fieles(area_par);

}

static void invalidate_area_par(struct rk_fb_area_par* area_par, void* modifier_data)
{
    area_par->ion_fd = 0;
    area_par->data_format = 0;

    /*
    if ( area_par->acq_fence_fd > 0 )
    {
        close(area_par->acq_fence_fd);
        area_par->acq_fence_fd = -1;
    }
    */
}

/**
 * 丢弃(不送显示) wins_cfg 中的所有 afbc_area_pars.
 */
static void invalidate_all_afbc_area_pars(struct rk_fb_win_cfg_data* wins_cfg)
{
    struct area_par_modifier_t area_par_invalidater;

    init_area_par_modifier(&area_par_invalidater, invalidate_area_par, NULL);

    modify_each_afbc_area_par(wins_cfg, &area_par_invalidater);
}

/*---------------------------------------------------------------------------*/

static int rk_hwc_debug_facility_init(rk_hwc_debug_facility_t* facility)
{
    int ret = 0;

    if ( facility->m_inited )
    {
        goto EXIT;
    }

    /*-------------------------------------------------------*/

    facility->m_ve = VE_NONE;
    D("to init mutex and cond.");
    pthread_mutex_init(&facility->m_mutex_for_ve, NULL);
    pthread_cond_init(&facility->m_cond_for_ve, NULL);

    /*-------------------------------------------------------*/

    facility->m_inited = true;

EXIT:
    return ret;
}

static void rk_hwc_debug_facility_term(rk_hwc_debug_facility_t* facility)
{
    if ( !(facility->m_inited ) )
    {
        return;
    }

    /*-------------------------------------------------------*/

    pthread_mutex_destroy(&facility->m_mutex_for_ve);
    pthread_cond_destroy(&facility->m_cond_for_ve);
    facility->m_ve = VE_NONE;

    /*-------------------------------------------------------*/

    facility->m_inited = false;
}

/*---------------------------------------------------------------------------*/

/**
 * 获取 uevent socket 的 fd, 从 '*pFd' 返回.
 */
static int getUeventSocket(int* pFd)
{
    int ret = 0;

    struct sockaddr_nl addr;
    int sock = -1;      // '*pFd'.

    memset(&addr, 0, sizeof(addr) );
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = 1;

    sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if ( sock < 0 )
    {
        SET_ERROR_AND_JUMP("fail to open socket. err : %s.", ret, -errno , EXIT, strerror(errno) );
    }
    fcntl(sock, F_SETFD, FD_CLOEXEC);

    if ( bind(sock, (struct sockaddr *)(&addr), sizeof(addr) ) < 0 )
    {
        SET_ERROR_AND_JUMP("fail to bind socket. err : %s.", ret, -errno , EXIT, strerror(errno) );
    }

    *pFd = sock;

EXIT:
    return ret;
}

/**
 * 返回 CLOCK_MONOTONIC 类型的系统时间, ns 为单位.
 * 本函数可以用来 profile 特定程序的运行时间.
 */
static int64_t getSystemTime()
{
    struct timespec t;
    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &t); // Android 平台下, clock_gettime() 实现在 libc 中.

    return (int64_t)(t.tv_sec)*1000000000LL + t.tv_nsec;
}

static void notify_ve_event(rk_hwc_debug_facility_t* facility, vop_exception_t ve)
{
    pthread_mutex_lock(&(facility->m_mutex_for_ve) );
    facility->m_ve = ve;
    pthread_mutex_unlock(&(facility->m_mutex_for_ve) );
    pthread_cond_signal(&(facility->m_cond_for_ve) );
}

/**
 * 判断传入的 'uevent_msg' 是否是 uevent_of_ve,
 * 若是, 返回具体的 ve 类型,
 * 否则, 返回 VE_NONE.
 */
static vop_exception_t get_ve_from_uevent(rk_hwc_debug_facility_t* debug_facility, char* uevent_msg)
{
    int ret = 0;

    D_STR(uevent_msg);

    if ( NULL != strstr(uevent_msg, "vop0") ) // .T : 细化.
    {
        return VE_POST_EMPTY_BUFFER;
    }
    else
    {
        return VE_NONE;
    }
}

/**
 * 从 以 ms 为单位的 time interval 计算对应的 timeval 实例的内容.
 */
static void get_timeval_from_ms(int64_t in_ms, struct timeval* time_val)
{
    if (in_ms >= 1000)
    {
        time_val->tv_sec = in_ms / 1000;
    }
    else
    {
        time_val->tv_sec = 0;
    }

    time_val->tv_usec = (in_ms - 1000 * time_val->tv_sec) * 1000;
}

/**
 * 运行在 vop_exception_monitor_thread 中,
 * 有超时地, 等待可能出现的, 来自 kernel 的 ueventA_of_vop_exception,
 * 并将 相关信息通知到 debug_main_thread 中.
 * 若超时, 则说明 没有 exception 发生.
 */
static void* wait_uevent_of_ve_with_timeout(void* data)
{
    rk_hwc_debug_facility_t* debug_facility = (rk_hwc_debug_facility_t*)data;
    int ret = 0;
    int ueventSocket = -1;      /* uevent socket. */
    fd_set rfds;                /* 待监听的 fd 的集合. */
    struct timeval timeOut;     /* timeout for select() on uevent_socket. */
#define UEVENT_MSG_LEN	1000
    char msg[UEVENT_MSG_LEN + 2];   // raw uevent buf.
    ssize_t msglen = 0;
    int64_t start = 0;  // system time of starting to wait, in ns.
    int64_t current = 0;// current systemtime, in ns.
    const int64_t TIMEOUT_OF_WAITING_IN_MS = 50; // timeout for waiting vop_exception in ms.
    // const int64_t TIMEOUT_OF_WAITING_IN_MS = 1 * 1000;
    vop_exception_t ve = VE_NONE; // ve_from_uevent.

    /*-------------------------------------------------------*/
    get_timeval_from_ms(TIMEOUT_OF_WAITING_IN_MS, &timeOut);

    CHECK_FUNC_CALL( getUeventSocket(&ueventSocket) , ret, EXIT);

    start = getSystemTime();
    /* 若 未等到 uevent_of_ve, 且 "未" 超时, ... */
    while ( current - start < TIMEOUT_OF_WAITING_IN_MS * 1000 * 1000 )
    {
        FD_ZERO(&rfds);
        FD_SET(ueventSocket, &rfds);
        /* 有超时地监听 uevent socket 是否有数据可以读取. */
        ret = select(ueventSocket + 1, &rfds, NULL, NULL, &timeOut);
        /* 若出错. 如 指定的 描述符 ready 之前, 捕捉到一个信号. 则... */
        if ( -1 == ret )
        {
            SET_ERROR_AND_JUMP("error when selecting. err : %s.", ret, -errno, EXIT, strerror(errno) );
        }
        /* 若超时, 则... */
        if ( 0 == ret )
        {
            D("select() timeout.");
        }
        /* 否则, 即 'ueventSocket' 中确实有数据可读, 则...*/
        else
        {
            /* 读取 uevent. */
            msglen = recv(ueventSocket, msg, UEVENT_MSG_LEN, 0);
            /* 若出错, 则... */
            if ( msglen < 0 )
            {
                SET_ERROR_AND_JUMP("fail to recv uevent, err : %s.", ret, -errno, EXIT, strerror(errno) );
            }
            if ( UEVENT_MSG_LEN == msglen )
            {
                W("uevent received is too long.");
                continue;
            }

            msg[msglen] = '\0';
            msg[msglen + 1] = '\0';

            ve = get_ve_from_uevent(debug_facility, msg);
            if ( VE_NONE == ve )
            {
                D("not uevent_of_ve, to continue.");
                continue;
            }
            else if ( VE_POST_EMPTY_BUFFER == ve )
            {
                D("got a ve_post_empty_buffer, to notify it to main_thread, then exit");
                notify_ve_event(debug_facility, VE_POST_EMPTY_BUFFER);
                goto EXIT;
            }
            else
            {
                W("unexpected ve = 0x%x.", ve);
                continue;
            }
        }

        current = getSystemTime();
    }
    D("timeout without uevent_of_ve");

    D("to notify event_of_no_ve to main_thread.");
    /* 将 no_ve 事件 通知回到 main_thread. */
    notify_ve_event(debug_facility, VE_NONE);

EXIT:
    if ( ueventSocket != -1 )
    {
        close(ueventSocket);
    }

    return NULL;
}

/**
 * 启动 vop_exception_monitor_thread.
 */
static int start_ve_monitor_thread(rk_hwc_debug_facility_t* debug_facility)
{
    int ret = 0;

    CHECK_FUNC_CALL( pthread_create(&(debug_facility->m_ve_monitor_thread),
                                    NULL,
                                    wait_uevent_of_ve_with_timeout,
                                    debug_facility)
                    , ret, EXIT);
EXIT:
    return ret;
}

/**
 * 等待来自 ve_monitor_thread 的 ve_event, 并返回当前 ve.
 */
static vop_exception_t wait_ve_event(rk_hwc_debug_facility_t* facility)
{
    vop_exception_t ve = VE_NONE;

    pthread_mutex_lock(&(facility->m_mutex_for_ve) );
    pthread_cond_wait(&(facility->m_cond_for_ve),
                      &(facility->m_mutex_for_ve) );
    ve = facility->m_ve;
    pthread_mutex_unlock(&(facility->m_mutex_for_ve) );

    return ve;
}

static u16 s_list_of_unaccepted_y_act[] =
{
    4,  20, 36, 52, 68,
};
static bool is_y_act_unaccepted(u16 y_act)
{
#if 0
    uint32_t i = 0;

    for ( i = 0; i < ELEMENT_NUM(s_list_of_unaccepted_y_act); i++ )
    {
        if ( s_list_of_unaccepted_y_act[i] == y_act )
        {
            return true;
        }
    }

    return false;
#else
    if ( y_act % 16 == 4        // 导致 intr_post_buf_empty.
        || y_act % 16 == 8      // 显示抖动.
        || y_act % 16 == 12     // 抖动
        )
    {
        return true;
    }

    return false;
#endif
}

static u16 s_list_of_unaccepted_y_pos[] =
{
    9, 10, 11,
};
static bool is_y_pos_unaccepted(u16 y_pos)
{
    uint32_t i = 0;

    for ( i = 0; i < ELEMENT_NUM(s_list_of_unaccepted_y_pos); i++ )
    {
        if ( s_list_of_unaccepted_y_pos[i] == y_pos )
        {
            return true;
        }
    }

    return false;
}

/**
 * 对 wins_cfg 中的 afbc_area_par(通常只有一个), 设定特定一系列的 y_size 和相同的 y_act,
 * 执行 ioctl_config_done,
 * 测试是否会触发 intr_post_buf_empty.
 */
static void test_y_size_and_the_same_y_act_of_afbc_area(hwc_context_t* context,
                                                        hwc_display_contents_1_t* contents,
                                                        struct rk_fb_win_cfg_data* wins_cfg)
{
    struct rk_fb_area_par* area_par = get_first_afbc_area(wins_cfg);
    struct rk_fb_win_par* win_par = get_win_par_of_first_valid_area_par(wins_cfg);
    u16 y_act = 8;  // 必须是 8 的整数倍, rk_vop_hw 要求.
    u16 y_size = y_act;
    rk_hwc_debug_facility_t* debug_facility = &s_debug_facility;
    vop_exception_t ve = VE_NONE;
    u8 win_id = 0;  // id for target rk_vop_win

    D("to set win_id to %u.", win_id);
    set_win_id(win_par, win_id);

    for ( ;
            y_act < get_y_vir(area_par) && y_size < get_y_vir(area_par);
            y_act += 8 )
    {
#if 1
        /* 若 'y_act' 已经被确认是是不被接受的, 则.... */
        if ( is_y_act_unaccepted(y_act) )
        {
            continue;
        }
#endif

        /* 启动 vop_exception_monitor_thread. */
        start_ve_monitor_thread(debug_facility);

        /*-----------------------------------*/

        D("to set %u to y_act and y_size.", y_act);
        set_y_act(area_par, y_act);
#if 1
        /* 设置 y_size 和 y_act 相同. */
        set_y_size(area_par, y_act);
#else
        y_size = y_act * 2;
        set_y_size(area_par, y_size);
#endif

        DUMP_FB_CONFIG_FOR_NEXT_FRAME(wins_cfg, 1);
        /* ioctl_config_done 到 rk_fb_dev. */
        if ( ioctl(context->fbFd, RK_FBIOSET_CONFIG_DONE, wins_cfg) )
        {
            E("ioctl_config_done fail, err : %s.",  strerror(errno) );
            goto EXIT;
        }

        /* 关闭对应的 release_fence_fds 和 retire_fence_fd. */
        close_all_release_fence_fds(wins_cfg);
        close_retire_fence_fd(wins_cfg);

        /*-----------------------------------*/

        D("to wait ve_event.");
        /* 等待来自 vop_exception_monitor_thread 的 vop_exception, 正常应该是 VE_NONE. */
        ve = wait_ve_event(debug_facility);
        /* 若有 ve 发生. */
        if ( VE_NONE != ve )
        {
            D("got a ve : 0x%x, to dump display_contents, then exit. 'y_act' : %d", ve, y_act);
            dump_content_of_layers_to_file(contents);
            goto EXIT;
        }
    }
    // 至此, 所有的 values of y_act 都测试通过
    D("all the values of y_act PASS.");

EXIT:
    return;
}

/**
 * 对 wins_cfg 中的 afbc_area_par(通常只有一个), 设定特定一系列的 x_act 和相同的 y_act,
 * 执行 ioctl_config_done,
 * 测试是否会触发 intr_post_buf_empty.
 */
static void test_x_act_and_the_same_x_size_of_afbc_area(hwc_context_t* context,
                                                        hwc_display_contents_1_t* contents,
                                                        struct rk_fb_win_cfg_data* wins_cfg)
{
    struct rk_fb_area_par* area_par = get_first_afbc_area(wins_cfg);
    struct rk_fb_win_par* win_par = get_win_par_of_first_valid_area_par(wins_cfg);
    u16 x_act = 16;  // 必须是 16 的整数倍, rk_vop_hw 要求.
    u16 x_size = x_act;
    rk_hwc_debug_facility_t* debug_facility = &s_debug_facility;
    vop_exception_t ve = VE_NONE;
    u8 win_id = 0;  // id for target rk_vop_win

    D("to set win_id to %u.", win_id);
    set_win_id(win_par, win_id);

    for ( ;
            x_act < get_x_vir(area_par) && x_size < get_x_vir(area_par);
            x_act += 16 )
    {
        /* 启动 vop_exception_monitor_thread. */
        start_ve_monitor_thread(debug_facility);

        /*-----------------------------------*/

        D("to set %u to x_act and x_size.", x_act);
        set_x_act(area_par, x_act);
        /* 设置 x_size 和 x_act 相同. */
        set_x_size(area_par, x_act);

        DUMP_FB_CONFIG_FOR_NEXT_FRAME(wins_cfg, 1);
        /* ioctl_config_done 到 rk_fb_dev. */
        if ( ioctl(context->fbFd, RK_FBIOSET_CONFIG_DONE, wins_cfg) )
        {
            E("ioctl_config_done fail, err : %s.",  strerror(errno) );
            goto EXIT;
        }

        /* 关闭对应的 release_fence_fds 和 retire_fence_fd. */
        close_all_release_fence_fds(wins_cfg);
        close_retire_fence_fd(wins_cfg);

        /*-----------------------------------*/

        D("to wait ve_event.");
        /* 等待来自 vop_exception_monitor_thread 的 vop_exception, 正常应该是 VE_NONE. */
        ve = wait_ve_event(debug_facility);
        /* 若有 ve 发生. */
        if ( VE_NONE != ve )
        {
            D("got a ve : 0x%x, to dump display_contents, then exit. 'x_act' : %d", ve, x_act);
            dump_content_of_layers_to_file(contents);
            goto EXIT;
        }
    }
    // 至此, 所有的 values of x_act 都测试通过
    D("all the values of x_act PASS.");

EXIT:
    return;
}

/**
 * 对 wins_cfg 中的 afbc_area_par(通常只有一个), 设定特定一系列的 x_pos 和对应的 x_size,
 * 执行 ioctl_config_done,
 * 测试是否会触发 intr_post_buf_empty.
 */
static void test_x_pos_of_afbc_area(hwc_context_t* context,
                                    hwc_display_contents_1_t* contents,
                                    struct rk_fb_win_cfg_data* wins_cfg)
{
    struct rk_fb_area_par* area_par = get_first_afbc_area(wins_cfg);
    struct rk_fb_win_par* win_par = get_win_par_of_first_valid_area_par(wins_cfg);
    u16 x_pos = 0;
    u16 phy_width = get_x_vir(area_par);
    u16 x_size = phy_width - x_pos;
    rk_hwc_debug_facility_t* debug_facility = &s_debug_facility;
    vop_exception_t ve = VE_NONE;
    u8 win_id = 0;  // id for target rk_vop_win

    D("to set win_id to %u.", win_id);
    set_win_id(win_par, win_id);

    for ( ;
            x_pos < phy_width;
            x_pos += 1 )
    {
        /* 启动 vop_exception_monitor_thread. */
        start_ve_monitor_thread(debug_facility);

        /*-----------------------------------*/

        x_size = phy_width - x_pos;
        D("to set %u to x_pos, %u to x_size and x_act", x_pos, x_size);
        set_x_pos(area_par, x_pos);
        set_x_size(area_par, x_size);
        set_x_act(area_par, x_size);

        DUMP_FB_CONFIG_FOR_NEXT_FRAME(wins_cfg, 1);
        /* ioctl_config_done 到 rk_fb_dev. */
        if ( ioctl(context->fbFd, RK_FBIOSET_CONFIG_DONE, wins_cfg) )
        {
            E("ioctl_config_done fail, err : %s.",  strerror(errno) );
            goto EXIT;
        }

        /* 关闭对应的 release_fence_fds 和 retire_fence_fd. */
        close_all_release_fence_fds(wins_cfg);
        close_retire_fence_fd(wins_cfg);

        /*-----------------------------------*/

        D("to wait ve_event.");
        /* 等待来自 vop_exception_monitor_thread 的 vop_exception, 正常应该是 VE_NONE. */
        ve = wait_ve_event(debug_facility);
        /* 若有 ve 发生. */
        if ( VE_NONE != ve )
        {
            D("got a ve : 0x%x, to dump display_contents, then exit. 'x_pos' : %d", ve, x_pos);
            dump_content_of_layers_to_file(contents);
            goto EXIT;
        }
    }
    // 至此, 所有的 values of x_pos 都测试通过
    D("all the values of x_pos PASS.");

EXIT:
    return;
}

/**
 * 对 wins_cfg 中的 afbc_area_par(通常只有一个), 设定特定一系列的 y_pos 和对应的 y_size,
 * 执行 ioctl_config_done,
 * 测试是否会触发 intr_post_buf_empty.
 */
static void test_y_pos_of_afbc_area(hwc_context_t* context,
                                    hwc_display_contents_1_t* contents,
                                    struct rk_fb_win_cfg_data* wins_cfg)
{
    struct rk_fb_area_par* area_par = get_first_afbc_area(wins_cfg);
    struct rk_fb_win_par* win_par = get_win_par_of_first_valid_area_par(wins_cfg);
    u16 y_pos = 0;
    u16 phy_height = get_y_vir(area_par);
    u16 y_size = phy_height - y_pos;
    rk_hwc_debug_facility_t* debug_facility = &s_debug_facility;
    vop_exception_t ve = VE_NONE;
    u8 win_id = 1;  // id for target rk_vop_win

    D("to set win_id to %u.", win_id);
    set_win_id(win_par, win_id);

    for ( ;
            y_pos < phy_height;
            y_pos += 1 )
    {
#if 1
        /* 若 'y_pos' 已经被确认是是不被 rk_fb_dev 接受的, 则.... */
        if ( is_y_pos_unaccepted(y_pos) )
        {
            continue;
        }
#endif
        /* 启动 vop_exception_monitor_thread. */
        start_ve_monitor_thread(debug_facility);

        /*-----------------------------------*/

        y_size = phy_height - y_pos;
        D("to set %u to y_pos, %u to y_size and y_act", y_pos, y_size);
        set_y_pos(area_par, y_pos);
        set_y_size(area_par, y_size);
        set_y_act(area_par, y_size);

        DUMP_FB_CONFIG_FOR_NEXT_FRAME(wins_cfg, 1);
        /* ioctl_config_done 到 rk_fb_dev. */
        if ( ioctl(context->fbFd, RK_FBIOSET_CONFIG_DONE, wins_cfg) )
        {
            E("ioctl_config_done fail, err : %s.",  strerror(errno) );
            goto EXIT;
        }

        /* 关闭对应的 release_fence_fds 和 retire_fence_fd. */
        close_all_release_fence_fds(wins_cfg);
        close_retire_fence_fd(wins_cfg);

        /*-----------------------------------*/

        D("to wait ve_event.");
        /* 等待来自 vop_exception_monitor_thread 的 vop_exception, 正常应该是 VE_NONE. */
        ve = wait_ve_event(debug_facility);
        /* 若有 ve 发生. */
        if ( VE_NONE != ve )
        {
            D("got a ve : 0x%x, to dump display_contents, then exit. 'y_pos' : %d", ve, y_pos);
            dump_content_of_layers_to_file(contents);
            goto EXIT;
        }
    }
    // 至此, 所有的 values of y_pos 都测试通过
    D("all the values of y_pos PASS.");

EXIT:
    return;
}

/**
 * 在 'context' 下, hack *wins_cfg(通常只包含一个 afbc_area) 中的 afbc_area 的 pos_and_size_fields,
 * 再将修改之后的 wins_cfg 送 rk_fb_dev.
 *
 * 实际上是用于测试 hack 设置的 pos_and_size_fields 的 value 是否会导致异常中断 intr_post_buf_empty.
 *
 * caller 要保证传入的 afbc_area 的 x_vir 和 y_vir 对应等于 物理屏幕的宽度和高度.
 */
static void test_pos_and_size_par_of_afbc_area(hwc_context_t* context,
                                               hwc_display_contents_1_t* contents,
                                               struct rk_fb_win_cfg_data* wins_cfg)
{
    // test_y_size_and_the_same_y_act_of_afbc_area(context, contents, wins_cfg);

    // test_x_act_and_the_same_x_size_of_afbc_area(context, contents, wins_cfg);

    // test_x_pos_of_afbc_area(context, contents, wins_cfg);

    test_y_pos_of_afbc_area(context, contents, wins_cfg);
}
#endif      // #ifdef ENABLE_HACK_WINS_CFG_FOR_CONFIG_DONE

