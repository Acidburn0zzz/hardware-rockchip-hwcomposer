/*  --------------------------------------------------------------------------------------------------------
 *  File:   rk_hwc_debug_utils.h
 *
 *  Desc:
 *
 *          -----------------------------------------------------------------------------------
 *          < 习语 和 缩略语 > :
 *
 *          -----------------------------------------------------------------------------------
 *  Usage:		
 *
 *  Note:
 *
 *  Author: ChenZhen
 *
 *  Log:
	----Thu Apr 14 20:55:44 2016            init_ver.
 *  --------------------------------------------------------------------------------------------------------
 */


#ifndef __RK_HWC_DEBUG_UTILS_H__
#define __RK_HWC_DEBUG_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------------------------------------
 *  Include Files
 * ---------------------------------------------------------------------------------------------------------
 */

#include <log/custom_log.h>

#include <hardware/rk_fh.h>

/* ---------------------------------------------------------------------------------------------------------
 *  Macros Definition
 * ---------------------------------------------------------------------------------------------------------
 */

/**
 * dump 数据实例到 log 时, 缩进的最大个数.
 */
#define MAX_NUM_OF_INDENTS          (15)

/*---------------------------------------------------------------------------*/

#ifdef ENABLE_VERBOSE_LOG
#define DUMP_FB_CONFIG_FOR_NEXT_FRAME(pCfg, indentNum) \
    { \
        D("dump fb_config_for_next_frame_t instance at '" #pCfg"' : "); \
        dumpFbConfigForNextFrame( (pCfg), (indentNum) ); \
    }
#else
#define  DUMP_FB_CONFIG_FOR_NEXT_FRAME(...)  ((void)0)
#endif

/* ---------------------------------------------------------------------------------------------------------
 *  Types and Structures Definition
 * ---------------------------------------------------------------------------------------------------------
 */

typedef struct rk_fb_area_par area_config_for_next_frame_t;

typedef struct rk_fb_win_par win_config_for_next_frame_t;

typedef struct rk_fb_win_cfg_data fb_config_for_next_frame_t;


/* ---------------------------------------------------------------------------------------------------------
 *  Global Functions' Prototype
 * ---------------------------------------------------------------------------------------------------------
 */


/* ---------------------------------------------------------------------------------------------------------
 *  Inline Functions Implementation
 * ---------------------------------------------------------------------------------------------------------
 */

// x_offset
inline u16 get_x_offset(struct rk_fb_area_par* pThis)
{
    return pThis->x_offset;
}
inline void set_x_offset(struct rk_fb_area_par* pThis, u16 x_offset)
{
    pThis->x_offset = x_offset;
}

// y_offset
inline u16 get_y_offset(struct rk_fb_area_par* pThis)
{
    return pThis->y_offset;
}
inline void set_y_offset(struct rk_fb_area_par* pThis, u16 y_offset)
{
    pThis->y_offset = y_offset;
}

// xpos
inline u16 get_x_pos(struct rk_fb_area_par* pThis)
{
    return pThis->xpos;
}
inline void set_x_pos(struct rk_fb_area_par* pThis, u16 x_pos)
{
    pThis->xpos = x_pos;
}

// ypos
inline u16 get_y_pos(struct rk_fb_area_par* pThis)
{
    return pThis->ypos;
}
inline void set_y_pos(struct rk_fb_area_par* pThis, u16 y_pos)
{
    pThis->ypos = y_pos;
}

// x_size
inline u16 get_x_size(struct rk_fb_area_par* pThis)
{
    return pThis->xsize;
}
inline void set_x_size(struct rk_fb_area_par* pThis, u16 x_size)
{
    pThis->xsize = x_size;
}

// y_size
inline u16 get_y_size(struct rk_fb_area_par* pThis)
{
    return pThis->ysize;
}
inline void set_y_size(struct rk_fb_area_par* pThis, u16 y_size)
{
    pThis->ysize = y_size;
}

// x_act
inline u16 get_x_act(struct rk_fb_area_par* pThis)
{
    return pThis->xact;
}
inline void set_x_act(struct rk_fb_area_par* pThis, u16 x_act)
{
    pThis->xact = x_act;
}

// y_act
inline u16 get_y_act(struct rk_fb_area_par* pThis)
{
    return pThis->yact;
}
inline void set_y_act(struct rk_fb_area_par* pThis, u16 y_act)
{
    pThis->yact = y_act;
}

// x_vir
inline u16 get_x_vir(struct rk_fb_area_par* pThis)
{
    return pThis->xvir;
}
inline void set_x_vir(struct rk_fb_area_par* pThis, u16 x_vir)
{
    pThis->xvir = x_vir;
}

// y_vir
inline u16 get_y_vir(struct rk_fb_area_par* pThis)
{
    return pThis->yvir;
}
inline void set_y_vir(struct rk_fb_area_par* pThis, u16 y_vir)
{
    pThis->yvir = y_vir;
}

/*-------------------------------------------------------*/

inline bool is_valid_area_par(const struct rk_fb_area_par* area_par)
{
    return (area_par->ion_fd > 0 && area_par->data_format > 0);
}

inline bool is_afbc_area_par(const struct rk_fb_area_par* area_par)
{
    return  is_valid_area_par(area_par) && (0x27 == area_par->data_format); // .T : 不止 0x27.
}

/*-------------------------------------------------------*/

inline void set_win_id(struct rk_fb_win_par* win_par, u8 win_id)
{
    win_par->win_id = win_id;
}

/*---------------------------------------------------------------------------*/

/**
 * 获取指定 wins_cfg 中 valid_area_par 实例的个数.
 */
inline uint32_t get_num_of_valid_area_par(const struct rk_fb_win_cfg_data* wins_cfg)
{
    uint32_t num = 0; // num_of_valid_area_par

    for ( uint32_t i = 0; i < RK30_MAX_LAYER_SUPPORT; i++ )
    {
        const struct rk_fb_win_par* win_par = &( (wins_cfg->win_par)[i] );

        for ( uint32_t j = 0; j < RK_WIN_MAX_AREA; j++ )
        {
            const struct rk_fb_area_par* area_par = &( (win_par->area_par)[j] );

            if ( is_valid_area_par(area_par) )
            {
                num++;
            }
        }
    }

    return num;
}

/**
 * 获取指定 wins_cfg 中 afbc_area_par 实例的个数.
 */
inline uint32_t get_num_of_afbc_area_par(const struct rk_fb_win_cfg_data* wins_cfg)
{
    uint32_t num = 0; // num_of_valid_area_par

    for ( uint32_t i = 0; i < RK30_MAX_LAYER_SUPPORT; i++ )
    {
        const struct rk_fb_win_par* win_par = &( (wins_cfg->win_par)[i] );

        for ( uint32_t j = 0; j < RK_WIN_MAX_AREA; j++ )
        {
            const struct rk_fb_area_par* area_par = &( (win_par->area_par)[j] );

            if ( is_afbc_area_par(area_par) )
            {
                num++;
            }
        }
    }

    return num;
}

/**
 * 获取指定 wins_cfg 中最先出现的 valid_area_par 的指针.
 */
inline struct rk_fb_area_par* get_first_valid_area_par(struct rk_fb_win_cfg_data* wins_cfg)
{
    for ( uint32_t i = 0; i < RK30_MAX_LAYER_SUPPORT; i++ )
    {
        struct rk_fb_win_par* win_par = &( (wins_cfg->win_par)[i] );

        for ( uint32_t j = 0; j < RK_WIN_MAX_AREA; j++ )
        {
            struct rk_fb_area_par* area_par = &( (win_par->area_par)[j] );

            if ( is_valid_area_par(area_par) )
            {
                return area_par;
            }
        }
    }

EXIT:
    return NULL;
}

/**
 * 返回 指定 wins_cfg 中, 第一个 valid_area_par 所在 的 win_par 的指针.
 */
inline struct rk_fb_win_par * get_win_par_of_first_valid_area_par(struct rk_fb_win_cfg_data* wins_cfg)
{
    for ( uint32_t i = 0; i < RK30_MAX_LAYER_SUPPORT; i++ )
    {
        struct rk_fb_win_par* win_par = &( (wins_cfg->win_par)[i] );

        for ( uint32_t j = 0; j < RK_WIN_MAX_AREA; j++ )
        {
            struct rk_fb_area_par* area_par = &( (win_par->area_par)[j] );

            if ( is_valid_area_par(area_par) )
            {
                return win_par;
            }
        }
    }

    return NULL;
}

/**
 * 返回 第一个 包含 afbc_area 的 wins_cfg 的指针.
 */
inline const struct rk_fb_win_par* get_first_win_par_with_afbc_area(const struct rk_fb_win_cfg_data* wins_cfg)
{
    for ( uint32_t i = 0; i < RK30_MAX_LAYER_SUPPORT; i++ )
    {
        const struct rk_fb_win_par* win_par = &( (wins_cfg->win_par)[i] );

        for ( uint32_t j = 0; j < RK_WIN_MAX_AREA; j++ )
        {
            const struct rk_fb_area_par* area_par = &( (win_par->area_par)[j] );

            if ( is_afbc_area_par(area_par) )
            {
                return win_par;
            }
        }
    }

    return NULL;
}

inline struct rk_fb_area_par* get_first_afbc_area(struct rk_fb_win_cfg_data* wins_cfg)
{
    for ( uint32_t i = 0; i < RK30_MAX_LAYER_SUPPORT; i++ )
    {
        struct rk_fb_win_par* win_par = &( (wins_cfg->win_par)[i] );

        for ( uint32_t j = 0; j < RK_WIN_MAX_AREA; j++ )
        {
            struct rk_fb_area_par* area_par = &( (win_par->area_par)[j] );

            if ( is_afbc_area_par(area_par) )
            {
                return area_par;
            }
        }
    }

    return NULL;
}

typedef void (*pfn_modify_area_par_t)(struct rk_fb_area_par* area_par, void* modifier_data);
typedef struct area_par_modifier_t
{
    pfn_modify_area_par_t m_pfn_modify_area_par;
    /**
     * 对 'm_pfn_modify_area_par' 策略回调的时候, 将作为其参数 'modifier_data' 的实参.
     */
    void* m_modifier_data;
}   area_par_modifier_t;
inline void init_area_par_modifier(struct area_par_modifier_t* modifier,
                            pfn_modify_area_par_t pfn_modify_area_par,
                            void* modifier_data)
{
    modifier->m_pfn_modify_area_par =  pfn_modify_area_par;
    modifier->m_modifier_data = modifier_data;
}

/**
 * 使用传入的 'area_par_modifier' 对 'wins_cfg' 中的所有 "非" afbc_area_par 进行修改.
 */
inline void modify_each_non_afbc_area_par(struct rk_fb_win_cfg_data* wins_cfg, area_par_modifier_t* area_par_modifier)
{
    for ( uint32_t i = 0; i < RK30_MAX_LAYER_SUPPORT; i++ )
    {
        struct rk_fb_win_par* win_par = &( (wins_cfg->win_par)[i] );

        for ( uint32_t j = 0; j < RK_WIN_MAX_AREA; j++ )
        {
            struct rk_fb_area_par* area_par = &( (win_par->area_par)[j] );

            if ( is_valid_area_par(area_par) && !is_afbc_area_par(area_par) )
            {
                area_par_modifier->m_pfn_modify_area_par(area_par, area_par_modifier->m_modifier_data);
            }
        }
    }
}

/**
 * 使用传入的 'area_par_modifier' 对 'wins_cfg' 中的所有 afbc_area_par 进行修改.
 */
inline void modify_each_afbc_area_par(struct rk_fb_win_cfg_data* wins_cfg, area_par_modifier_t* area_par_modifier)
{
    for ( uint32_t i = 0; i < RK30_MAX_LAYER_SUPPORT; i++ )
    {
        struct rk_fb_win_par* win_par = &( (wins_cfg->win_par)[i] );

        for ( uint32_t j = 0; j < RK_WIN_MAX_AREA; j++ )
        {
            struct rk_fb_area_par* area_par = &( (win_par->area_par)[j] );

            if ( is_afbc_area_par(area_par) )
            {
                area_par_modifier->m_pfn_modify_area_par(area_par, area_par_modifier->m_modifier_data);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

inline void dumpAreaConfigForNextFrame(const area_config_for_next_frame_t* pThis, uint8_t indentNum)
{
    char indents[MAX_NUM_OF_INDENTS + 1];

    setIndents(indents, indentNum);

    LOGD("%s data_format : 0x%x", indents, pThis->data_format);

    LOGD("%s ion_fd : %d", indents, pThis->ion_fd);
    LOGD("%s phy_addr : 0x%x", indents, pThis->phy_addr);

    LOGD("%s acq_fence_fd : %d", indents, pThis->acq_fence_fd);

    LOGD("%s x_offset : %u", indents, pThis->x_offset);
    LOGD("%s y_offset : %u", indents, pThis->y_offset);
    LOGD("%s xpos : %u", indents, pThis->xpos);
    LOGD("%s ypos : %u", indents, pThis->ypos);
    LOGD("%s xsize : %u", indents, pThis->xsize);
    LOGD("%s ysize : %u", indents, pThis->ysize);

    LOGD("%s xact : %u", indents, pThis->xact);
    LOGD("%s yact : %u", indents, pThis->yact);

    LOGD("%s xvir : %u", indents, pThis->xvir);
    LOGD("%s yvir : %u", indents, pThis->yvir);

    LOGD("%s fbdc_en : %u", indents, pThis->fbdc_en);
    LOGD("%s fbdc_cor_en : %u", indents, pThis->fbdc_cor_en);
    LOGD("%s fbdc_data_format : %u", indents, pThis->fbdc_data_format);
}

inline void dumpWinConfigForNextFrame(const win_config_for_next_frame_t* pThis, uint8_t indentNum)
{
    char indents[MAX_NUM_OF_INDENTS + 1];
    uint8_t i;

    setIndents(indents, indentNum);

    LOGD("%s win_id : %u", indents, pThis->win_id);

    LOGD("%s z_order : %u", indents, pThis->z_order);

    LOGD("%s alpha_mode : %u", indents, pThis->alpha_mode);
    LOGD("%s g_alpha_val : %u", indents, pThis->g_alpha_val);

    LOGD("%s mirror_en : %u", indents, pThis->mirror_en);

    LOGD("%s area_par : ", indents);
    for ( i = 0; i < RK_WIN_MAX_AREA; i++ )
    {
        const area_config_for_next_frame_t* pAreaCfg = &( (pThis->area_par)[i] );

        if ( pAreaCfg->data_format > 0 )
        {
            LOGD("%s\t ---------------", indents);
            dumpAreaConfigForNextFrame( pAreaCfg, indentNum + 1);
        }
    }
}

inline void dumpFbConfigForNextFrame(const fb_config_for_next_frame_t* pThis, uint8_t indentNum)
{
    char indents[MAX_NUM_OF_INDENTS + 1];
    uint8_t i;

    setIndents(indents, indentNum);

    LOGD("%s num_of_area_par : %u", indents, get_num_of_valid_area_par(pThis) );
    LOGD("%s num_of_afbc_area_par : %u", indents, get_num_of_afbc_area_par(pThis) );
    LOGD("%s ---------------", indents);
    LOGD("%s wait_fs : %u", indents, pThis->wait_fs);

    LOGD("%s ret_fence_fd : %d", indents, pThis->ret_fence_fd);

    LOGD("%s rel_fence_fd : ", indents);
    for ( i = 0; i < RK_MAX_BUF_NUM; i++ )
    {
        LOGD("%s \t [%d] : %d", indents, i, (pThis->rel_fence_fd)[i] );
    }

    LOGD("%s win_par : ", indents);
    for ( i = 0; i < RK_WIN_MAX_AREA; i++ )
    {
        const win_config_for_next_frame_t* pWinCfg = &( (pThis->win_par)[i] );

        // if ( 0 != pWinCfg->win_id )
        {
            LOGD("%s\t ---------------", indents);
            dumpWinConfigForNextFrame( &( (pThis->win_par)[i] ), indentNum + 1);
        }
    }

    // post_cfg
}

/*---------------------------------------------------------------------------*/

inline void close_all_release_fence_fds(struct rk_fb_win_cfg_data* wins_cfg)
{
    uint32_t i = 0;

    for ( i = 0; i < RK_MAX_BUF_NUM; i++ )
    {
        if ( (wins_cfg->rel_fence_fd)[i] > 0 )
        {
            close( (wins_cfg->rel_fence_fd)[i] );
            (wins_cfg->rel_fence_fd)[i] = -1;
        }
    }
}

inline void close_retire_fence_fd(struct rk_fb_win_cfg_data* wins_cfg)
{
    if ( wins_cfg->ret_fence_fd > 0 )
    {
        close(wins_cfg->ret_fence_fd);
        wins_cfg->ret_fence_fd = -1;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* __RK_HWC_DEBUG_UTILS_H__ */

