/*  --------------------------------------------------------------------------------------------------------
 *  File:   rk_hwc_debug_mark_time.h
 *
 *  Desc:   声明能在当前线程中 mark time(原地踏步) 的调试接口.
 *
 *  Usage:  若需要使能, 则要在下面定义 ENABLE_MARK_TIME_FOR_DEBUGGING,
 *          然后在 rk_hwc 合适位置调用 might_mark_time_for_debugging,
 *          进入和退出 mark time, 还受到属性 "debug.sf.mark_time" 的控制.
 *
 *  Note:
 *
 *  Author: ChenZhen
 *
 *  Log:
	----Thu Oct 20 19:51:01 2016            init_ver
 *  --------------------------------------------------------------------------------------------------------
 */


#ifndef __RK_HWC_DEBUG_MARK_TIME_H__
#define __RK_HWC_DEBUG_MARK_TIME_H__


/* ---------------------------------------------------------------------------------------------------------
 *  Include Files
 * ---------------------------------------------------------------------------------------------------------
 */

#include <cutils/properties.h>

#include "rk_hwcomposer.h"

/* ---------------------------------------------------------------------------------------------------------
 *  Macros Definition
 * ---------------------------------------------------------------------------------------------------------
 */

// config
// #define ENABLE_MARK_TIME_FOR_DEBUGGING

/* ############################################################################################# */

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------------------------------------
 *  Types and Structures Definition
 * ---------------------------------------------------------------------------------------------------------
 */


/* ---------------------------------------------------------------------------------------------------------
 *  Global Functions' Prototype
 * ---------------------------------------------------------------------------------------------------------
 */


#ifdef ENABLE_MARK_TIME_FOR_DEBUGGING
void might_mark_time_for_debugging(hwcContext* context, hwc_display_contents_1_t* contents);
#endif


/* ---------------------------------------------------------------------------------------------------------
 *  Inline Functions Implementation
 * ---------------------------------------------------------------------------------------------------------
 */

#ifdef __cplusplus
}
#endif

#endif /* __RK_HWC_DEBUG_MARK_TIME_H__ */

