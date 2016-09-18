#

# rockchip hwcomposer( 2D graphic acceleration unit) .

#

# Copyright (C) 2015 Rockchip Electronics Co., Ltd.
#


LOCAL_PATH := $(call my-dir)
#include $(LOCAL_PATH)/../../Android.mk.def

#
# hwcomposer.default.so
#
include $(CLEAR_VARS)

$(info $(shell $(LOCAL_PATH)/version.sh))

LOCAL_SRC_FILES := \
	rk_hwcomposer.cpp \
	rk_hwc_com.cpp \
	rga_api.cpp \
	hwc_rga.cpp \
	rk_hwc_debug_mark_time.cpp \
	rk_hwc_debug_hack_wins_cfg.cpp

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),rk3399)
LOCAL_SRC_FILES += \
	rk_hwcomposer_htg.cpp \
	rk_hwcomposer_vop.cpp
else
LOCAL_SRC_FILES += \
	rk_hwcomposer_hdmi.cpp
endif

LOCAL_SRC_FILES += \
	rk_hwcomposer_blit.cpp

LOCAL_CFLAGS := \
	$(CFLAGS) \
	-Wall \
	-Wextra \
	-DLOG_TAG=\"hwcomposer\"

LOCAL_C_INCLUDES := \
	$(AQROOT)/sdk/inc \
	$(AQROOT)/hal/inc

LOCAL_C_INCLUDES += hardware/rockchip/libgralloc/ump/include

LOCAL_C_INCLUDES += \
	system/core/libion/include \
	system/core/libion/kernel-headers

ifneq (1,$(strip $(shell expr $(PLATFORM_VERSION) \>= 5.0)))
       LOCAL_C_INCLUDES += hardware/rk29/libgralloc_ump \
       hardware/rk29/libon2
else
       LOCAL_C_INCLUDES += hardware/rockchip/libgralloc \
       hardware/rockchip/librkvpu
       LOCAL_CFLAGS += -DSUPPORT_STEREO
endif

LOCAL_LDFLAGS := \
	-Wl,-z,defs	

LOCAL_SHARED_LIBRARIES := \
	libhardware \
	liblog \
	libui \
	libEGL \
	libcutils \
	libion \
	libhardware_legacy \
	libsync \
	libui \
	libutils



#LOCAL_C_INCLUDES := \
#	$(LOCAL_PATH)/inc

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),rk30xxb)	
LOCAL_CFLAGS += -DTARGET_BOARD_PLATFORM_RK30XXB
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),rk3399)
LOCAL_CFLAGS += -DTARGET_BOARD_PLATFORM_RK3399
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),tablet)
LOCAL_CFLAGS += -DRK3399_MID
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),box)
LOCAL_CFLAGS += -DRK3399_BOX
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),phone)
LOCAL_CFLAGS += -DRK3399_PHONE
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),vr)
LOCAL_CFLAGS += -DRK3399_VR
endif
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),rk3366)
LOCAL_CFLAGS += -DTARGET_BOARD_PLATFORM_RK3366
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),tablet)
LOCAL_CFLAGS += -DRK3366_MID
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),box)
LOCAL_CFLAGS += -DRK3366_BOX
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),phone)
LOCAL_CFLAGS += -DRK3366_PHONE
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),vr)
LOCAL_CFLAGS += -DRK3366_VR
endif
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),rk3368)
LOCAL_CFLAGS += -DTARGET_BOARD_PLATFORM_RK3368
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),tablet)
LOCAL_CFLAGS += -DRK3368_MID
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),box)
LOCAL_CFLAGS += -DRK3368_BOX
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),phone)
LOCAL_CFLAGS += -DRK3368_PHONE
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),vr)
LOCAL_CFLAGS += -DRK3368_VR
endif
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),rk3288)
LOCAL_CFLAGS += -DTARGET_BOARD_PLATFORM_RK3288
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),tablet)
LOCAL_CFLAGS += -DRK3288_MID
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),box)
LOCAL_CFLAGS += -DRK3288_BOX
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),phone)
LOCAL_CFLAGS += -DRK3288_PHONE
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),vr)
LOCAL_CFLAGS += -DRK3288_VR
endif
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),tablet)
LOCAL_CFLAGS += -DRK_MID
else
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),box)
LOCAL_CFLAGS += -DRK_BOX
else
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),phone)
LOCAL_CFLAGS += -DRK_PHONE
else
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),vr)
LOCAL_CFLAGS += -DRK_VR
endif #vr
endif #phone
endif #box
endif #tablet

ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)),G6110)
        LOCAL_CFLAGS += -DGPU_G6110
endif

ifeq ($(strip $(GRAPHIC_MEMORY_PROVIDER)),dma_buf)
LOCAL_CFLAGS += -DUSE_DMA_BUF
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)), mali-t720)
LOCAL_CFLAGS += -DMALI_PRODUCT_ID_T72X=1
LOCAL_CFLAGS += -DMALI_AFBC_GRALLOC=0
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)), mali-t760)
LOCAL_CFLAGS += -DMALI_PRODUCT_ID_T76X=1
# we use mali_afbc_gralloc, only if macro MALI_AFBC_GRALLOC is 1
LOCAL_CFLAGS += -DMALI_AFBC_GRALLOC=1
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)), mali-t860)
LOCAL_CFLAGS += -DMALI_PRODUCT_ID_T86X=1
LOCAL_CFLAGS += -DMALI_AFBC_GRALLOC=1
endif

#LOCAL_CFLAGS += -DUSE_LCDC_COMPOSER

ifeq ($(strip $(BOARD_USE_LCDC_COMPOSER)),true)	
LOCAL_CFLAGS += -DUSE_LCDC_COMPOSER
ifeq ($(strip $(BOARD_LCDC_COMPOSER_LANDSCAPE_ONLY)),false)
LOCAL_CFLAGS += -DLCDC_COMPOSER_FULL_ANGLE
endif
endif

LOCAL_CFLAGS += -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)

ifeq ($(strip $(BOARD_USE_AFBC_LAYER)),true)
# 宏 USE_AFBC_LAYER 和 在源码中定义的 G6110_SUPPORT_FBDC 相互独立并互斥.
LOCAL_CFLAGS += -DUSE_AFBC_LAYER
endif

LOCAL_MODULE := hwcomposer.$(TARGET_BOARD_HARDWARE)
LOCAL_MODULE_TAGS    := optional
#LOCAL_MODULE_PATH    := $(TARGET_OUT_SHARED_LIBRARIES)/hw
ifneq (1,$(strip $(shell expr $(PLATFORM_VERSION) \>= 5.0)))
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
else
ifneq ($(strip $(TARGET_2ND_ARCH)), )
LOCAL_MULTILIB := both
endif
LOCAL_MODULE_RELATIVE_PATH := hw
endif

LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)

