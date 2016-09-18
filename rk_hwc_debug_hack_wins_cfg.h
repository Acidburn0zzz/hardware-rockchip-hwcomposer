/*  --------------------------------------------------------------------------------------------------------
 *  File:   rk_hwc_debug_hack_wins_cfg.h
 *
 *  Desc:   声明用于对 ioctl_config_done 的 wins_cfg 参数 hack 的测试用例的入口.
 *
 *  Usage:	在下面定义 ENABLE_HACK_WINS_CFG_FOR_CONFIG_DONE, 使能该功能.
 *          在 rk_hwc 合适位置调用 hack_wins_cfg_for_config_done,
 *              通常是 gles_composition 之后, ioctl_config_done for post 之前.
 *
 *  Note:
 *
 *  Author: ChenZhen
 *
 *  Log:
	----Thu Oct 20 19:48:48 2016            init_ver
 *  --------------------------------------------------------------------------------------------------------
 */


#ifndef __RK_HWC_DEBUG_HACK_WINS_CFG_H__
#define __RK_HWC_DEBUG_HACK_WINS_CFG_H__


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

// define it to enable
// #define ENABLE_HACK_WINS_CFG_FOR_CONFIG_DONE

#ifdef ENABLE_HACK_WINS_CFG_FOR_CONFIG_DONE
#ifndef USE_AFBC_LAYER
#error "hack_wins_cfg is only for afbc_layer now!"
#endif
#endif

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


#ifdef ENABLE_HACK_WINS_CFG_FOR_CONFIG_DONE
void hack_wins_cfg_for_config_done(hwc_context_t* context,
                                   hwc_display_contents_1_t* contents,
                                   struct rk_fb_win_cfg_data* wins_cfg);

/* ---------------------------------------------------------------------------------------------------------
 *  Inline Functions Implementation
 * ---------------------------------------------------------------------------------------------------------
 */

static inline void disable_hacking_wins_cfg()
{
    property_set("debug.hwc.hack_wins_cfg", "0");
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RK_HWC_DEBUG_HACK_WINS_CFG_H__ */

