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
 *        
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

#include <hardware/rk_fh.h> // .Q : "rk_fh" ? 

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

#ifdef __cplusplus
}
#endif

#endif /* __RK_HWC_DEBUG_UTILS_H__ */

