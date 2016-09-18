/*  --------------------------------------------------------------------------------------------------------
 *  File:   rk_hwc_debug_mark_time.cpp
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
#include "rk_hwc_debug_mark_time.h"

/* ---------------------------------------------------------------------------------------------------------
 * Local Macros
 * ---------------------------------------------------------------------------------------------------------
 */


/* ---------------------------------------------------------------------------------------------------------
 * Local Typedefs
 * ---------------------------------------------------------------------------------------------------------
 */


/* ---------------------------------------------------------------------------------------------------------
 * External Function Prototypes (referenced in this file)
 * ---------------------------------------------------------------------------------------------------------
 */

extern int hwc_get_int_property(const char* pcProperty,const char* default_value);
extern void dump_content_of_layers_to_file(hwc_display_contents_1_t* list);

/* ---------------------------------------------------------------------------------------------------------
 * Local Function Prototypes
 * ---------------------------------------------------------------------------------------------------------
 */

#ifdef ENABLE_MARK_TIME_FOR_DEBUGGING
static inline void collect_data_for_debugging(hwcContext* context, hwc_display_contents_1_t* contents)
{
    I("to dump display_contents.")
    dump_content_of_layers_to_file(contents);
}
#endif


/* ---------------------------------------------------------------------------------------------------------
 * Local Variables
 * ---------------------------------------------------------------------------------------------------------
 */


/* ---------------------------------------------------------------------------------------------------------
 * Global Variables
 * ---------------------------------------------------------------------------------------------------------
 */


/* ---------------------------------------------------------------------------------------------------------
 * Global Functions Implementation
 * ---------------------------------------------------------------------------------------------------------
 */

#ifdef ENABLE_MARK_TIME_FOR_DEBUGGING
/**
 * 根据当前 property "debug.sf.mark_time" 的 value, 决定是否 将当前线程要 "原地踏步".
 * 实际上, 在 mark_time 之前, 会执行特定的 debug 动作.
 */
void might_mark_time_for_debugging(hwcContext* context, hwc_display_contents_1_t* contents)
{
    const uint32_t MS_TO_WAIT_SETTING_DEBUG_PROP = 100;
    int should_mark_time = 0;

    /* 等待 若干 ms, 以便 可以 人工 设置对应的 debug_property. */
    // usleep(MS_TO_WAIT_SETTING_DEBUG_PROP * 1000);

    should_mark_time = hwc_get_int_property("debug.sf.mark_time", "0");
    if ( should_mark_time )
    {
        I("to mark time for debugging, to collect data first.")

        collect_data_for_debugging(context, contents);

        while ( 1 )
        {
            int should_move_forward = 0;

            usleep(MS_TO_WAIT_SETTING_DEBUG_PROP * 1000);

            should_move_forward = !(hwc_get_int_property("debug.sf.mark_time", "0") );
            if (should_move_forward)
            {
                break;
            }
        }
    }
}
#endif

/* ---------------------------------------------------------------------------------------------------------
 * Local Functions Implementation
 * ---------------------------------------------------------------------------------------------------------
 */

