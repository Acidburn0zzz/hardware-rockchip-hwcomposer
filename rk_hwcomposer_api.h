/*

* rockchip hwcomposer( 2D graphic acceleration unit) .

*

* Copyright (C) 2015 Rockchip Electronics Co., Ltd.

*/

#ifndef __rk_hwcomposer_api_h_
#define __rk_hwcomposer_api_h_

int vop_init_devices(void** vopCtx);
int vop_free_devices(void** vopctx);
void vop_dump(void* vopCtx);
#endif