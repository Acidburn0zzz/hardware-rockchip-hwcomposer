#ifndef __rk_hwcomposer_vop_h_
#define __rk_hwcomposer_vop_h_
#include <linux/fb.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "rk_hwcomposer.h"
#include <hardware/hardware.h>
#include <cutils/log.h>
#include <sys/prctl.h>
#include <stdlib.h>
#include <errno.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <sync/sync.h>
#include <time.h>
#include <poll.h>
#include "rk_hwcomposer_hdmi.h"
#include <ui/PixelFormat.h>
#include <sys/stat.h>
#include <linux/ion.h>
#include <ion/ion.h>
#include <linux/rockchip_ion.h>
#include <string.h>
#include <string>
#include <vector>

#define _VOPONERROR(prefix, errno) \
    { \
        LOGD( "ONERROR: status=%d @ %s(%d) in ", \
            errno, __FUNCTION__, __LINE__); \
        goto OnError; \
    }

#define VOPONERROR(errno) _VOPONERROR(vop, errno)

enum rk_vop_feature {
	SUPPORT_VOP_IDENTIFY	= 1 << 0,
	SUPPORT_IFBDC		    = 1 << 1,
	SUPPORT_AFBDC		    = 1 << 2,
	SUPPORT_WRITE_BACK	    = 1 << 3,
	SUPPORT_YUV420_OUTPUT	= 1 << 4
};

enum rk_win_feature {
	SUPPORT_WIN_IDENTIFY	= 1 << 0,
	SUPPORT_SCALE		    = 1 << 1,
	SUPPORT_YUV		        = 1 << 2,
	SUPPORT_YUV10BIT	    = 1 << 3,
	SUPPORT_MULTI_AREA	    = 1 << 4,
	SUPPORT_HWC_LAYER	    = 1 << 5
};

/*
@xres:
@yres:
@stride:
@vsync_period:

@xdpi:
@ydpi:
*/
typedef struct _vopDisplay {
    u32 xres;
    u32 yres;
    u32 stride;
    u32 vsync_period;

    float xdpi;
    float ydpi;
}vopDisplay;
/*
@node :The area node was belong to eg,0 is equal to "sys/class/graphics/fb0" 
@fbFd :fbFd is equal to open(sys/class/graphics/fb0,mode)
@width,height,xstride,ystride:Follow the win and belong to lcd which is connect to vop
@scale:The property of the area eg,scale down
@areaFeature: now is equal to winFeature
@max_output_x:
@max_output_y:
*/
typedef struct _vopWinArea {
    int node;
    int fbFd;

    u32 areaFeature;
	u32 max_output_x;
	u32 max_output_y;
}vopWinArea;

/*
@winFd:
@areaNums:

@winFeature :The property of win.The property can be
    SUPPORT_WIN_IDENTIFY	= BIT(0),
	SUPPORT_SCALE		    = BIT(1),
	SUPPORT_YUV		        = BIT(2),
	SUPPORT_YUV10BIT	    = BIT(3),
	SUPPORT_MULTI_AREA	    = BIT(4),
	SUPPORT_HWC_LAYER	    = BIT(5)
@areaNums :The numbers of win at one vop

@mVopWinAreas:The vector of the area so it is easy to expand

@fixInfo:
@info:
*/
typedef struct _vopWin {
    u8 winIndex;
    
    int winFd;
    int areaNums;

    u32 winFeature;
	u32 max_input_x;
	u32 max_input_y;
    std::string nodeName;
    //std::vector<vopWinArea *> mVopWinAreas;
}vopWin;

/*
@vopFeature :The property of vop.The property can be 
	SUPPORT_VOP_IDENTIFY	= BIT(0),
	SUPPORT_IFBDC		    = BIT(1),
	SUPPORT_AFBDC		    = BIT(2),
	SUPPORT_WRITE_BACK	    = BIT(3),
	SUPPORT_YUV420_OUTPUT	= BIT(4)
@vsync_period:

@hwVsyncFd :
@allWinNums :The numbers of win at one vop
@allAreaNums :The numbers of area at one vop
@multAreaWinNums :The numbers of the win which has more areas
@mVopWinAreas:The vector of the win so it is easy to expand

@connected :Applies only to pluggable disp.Connected does not mean it ready to use.
    It should be active also. (UNBLANKED)
@isActive :
@isPause:In pause state, composition is bypassedused for WFD displays only

@mVopWinS:
*/
typedef struct _vopDevice {
    u32 vopid;
    u32 fbSize;
    u32 lcdSize;
    u32 vopFeature;
    u32 vsync_period;

    int vopFd;
    int iommuEn;
    int screenFd;
    int hwVsyncFd;
    int allWinNums;
    int allAreaNums;
    int multAreaWinNums;

    bool isPause;
    bool isActive;
    bool connected;

    uint64_t timeStamp;

    std::vector<vopWin *> mVopWinS;

    struct fb_var_screeninfo info;
    struct fb_fix_screeninfo fixInfo;
}vopDevice;

typedef struct _vopContext{
    threadPamaters vopMutex;
    std::vector<vopDevice *> mVopDevs;
}vopContext;

vopContext* vopCtx = NULL;
#endif
