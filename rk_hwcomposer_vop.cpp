#include "rk_hwcomposer_vop.h"

template <typename T>
T* vop_alloc_bytype(T* t)
{
    T* tmp;
    tmp = (T*)malloc(sizeof(T));
    if(!tmp)
        ALOGE("malloc vopDev fail for %s",strerror(errno));
    else
        memset((void*)tmp,0,sizeof(T));
    return tmp;
}

static void vop_win_add_area()
{
    return;
}

static int vop_vop_add_win(vopDevice* vopDev,u8 winIndex,int winFd,char* name)
{
    //ALOGD("wzqtest %s,%d",__func__,__LINE__);
    vopWin * win = NULL;
    vopWin* iter = vop_alloc_bytype(win);
    if(iter) {
        iter->winFd = winFd;
        iter->winIndex = winIndex;
        iter->nodeName.assign(name);
        vopDev->mVopWinS.push_back(iter);
    } else {
        ALOGE("Get alloc vopWin fail for:%s",strerror(errno));
        return -errno;
    }
    return 0;
}

static int vop_dev_add_vop(vopContext* vopCtx,u8 vopId,u8 winIndex,int winFd,char* name)
{
    //ALOGD("wzqtest %s,%d",__func__,__LINE__);

    u8 i = 0;
    vopDevice* vopDev = NULL;
    for(;(u8)vopCtx->mVopDevs.size() < vopId + 1;) {
        vopDevice* iter = vop_alloc_bytype(vopDev);
        if(iter) vopCtx->mVopDevs.push_back(iter);
    }
    if(vopCtx->mVopDevs.at(vopId) == NULL) {
        vopDevice* iter = vop_alloc_bytype(vopDev);
        if(iter) {
            //ALOGD("wzqtest %s,%d",__func__,__LINE__);
            iter->vopid = vopId;
            vop_vop_add_win(iter,winIndex,winFd,name);
            auto it = vopCtx->mVopDevs.begin();
            vopCtx->mVopDevs.insert(it+vopId,iter);
        } else {
            ALOGE("Get alloc vopDev fail for:%s",strerror(errno));
            return -errno;
        }
    } else {
        //ALOGD("wzqtest %s,%d",__func__,__LINE__);
        vopDevice* iter = vopCtx->mVopDevs.at(vopId);
        if(vopId != iter->vopid) iter->vopid = vopId;
        vop_vop_add_win(iter,winIndex,winFd,name);
    }
    return 0;
}

static int vop_collect_nodes(vopContext* vopCtx)
{
    int lcdFd = -1;
    int fbFd = -1;
    int ret = 0;

    u8 vopId = 0;

    char name[64];
    char value[10];
    const char fbd[] = "/dev/graphics/fb%u";
    const char node[] = "/sys/class/graphics/fb%u";
    for(unsigned int i = 0; i < 10 && ret >= 0; i++) {
        snprintf(name, 64, node, i);
        strcat(name,"/lcdcid");
        lcdFd = open(name,O_RDONLY,0);
        if(lcdFd > 0) {
            snprintf(name, 64, fbd, i);
            fbFd = open(name,O_RDWR,0);
            ret = read(lcdFd,value,sizeof(value));
            if(ret < 0) {
                if(fbFd > -1) close(fbFd);
                ALOGW("Get fb%d lcdcid fail:%s",i,strerror(errno));
            } else {
                vopId = atoi(value);
                snprintf(name, 64, node, i);
                vop_dev_add_vop(vopCtx,vopId,i,fbFd,name);
            }
            close(lcdFd);
        } else {
            ALOGW("Open fb%d lcdcid fail:%s",i,strerror(errno));
        }
    }
    return 0;
}

static int vop_init_wins(vopWin* vopWin)
{
    int ret = 0;
    int fbFd = -1;
    char nodeName[100] = {0};
    char value[100] = {0};
    if(!vopWin)  VOPONERROR(ret = -1);

    std::strcpy(nodeName,vopWin->nodeName.c_str());
    strcat(nodeName,"/win_property");

    //ALOGD("nodeName=%s",nodeName);
    fbFd = open(nodeName,O_RDONLY);
    if(fbFd > 0) {
        ret = read(fbFd,value,80);
        if(ret <= 0) {
            VOPONERROR(errno);
        } else {
            sscanf(value,"feature: %d, max_input_x: %d, max_input_y: %d",
            &(vopWin->winFeature),&(vopWin->max_input_x),&(vopWin->max_input_y));
        }
    } else {
        VOPONERROR(errno);
    }
    
    ret = 0;
    vopWin->areaNums = (vopWin->winFeature & SUPPORT_MULTI_AREA) ? 4 : 1;

OnError:
    if(fbFd > -1) close(fbFd);
    return ret;
}

static int vop_init_device(vopDevice* vopDev)
{
    int ret = 0;
    if((vopDev->mVopWinS.at(0) != NULL) && !vopDev->vopFd) {
        vopWin * iter = vopDev->mVopWinS.at(0);
        vopDev->vopFd = iter->winFd;
    } else
        VOPONERROR(ret = -1);

    //ALOGD("vopDev->vopFd=%d",vopDev->vopFd);
    
    ret = ioctl(vopDev->vopFd, RK_FBIOGET_IOMMU_STA, &vopDev->iommuEn);
    if (0 != ret) VOPONERROR(errno);

    ret = ioctl(vopDev->vopFd, FBIOGET_FSCREENINFO, &vopDev->fixInfo);
    if (0 != ret) VOPONERROR(errno);

    ret = ioctl(vopDev->vopFd, FBIOGET_VSCREENINFO, &vopDev->info);
    if (0 != ret) VOPONERROR(errno);

    for(size_t i = 0;i < vopDev->mVopWinS.size();i++) {
        vopWin* iter = vopDev->mVopWinS.at(i);
        ret = vop_init_wins(iter);
        if (0 != ret) {
            goto OnError;
        }
    }

OnError:
    return ret;
}

int vop_fix_device(void* ctx, int vopId)
{
    if(!ctx) return -1;
    
    vopContext* vopCtx = (vopContext*)(ctx);
    vopDevice* vopDev = vopCtx->mVopDevs.at(vopId);

    if(!vopDev) return -1;
    return vop_init_device(vopDev);
}

static int vop_exit_devices(void* vopctx)
{
    unsigned int isize = 0;
    unsigned int jsize = 0;
    vopContext* vopCtxFree = (vopContext*)(vopCtx);
    if(vopCtxFree != vopctx) {
        ALOGE("VOP context is not this owner");
        return -EINVAL;
    }

    if(!vopCtxFree) {
        ALOGE("VOP context is null,can not free");
        return -EINVAL;
    }

    pthread_mutex_lock(&vopCtxFree->vopMutex.mlk);
    vopCtxFree->vopMutex.count --;
    if(vopCtxFree->vopMutex.count != 0) {
        pthread_mutex_unlock(&vopCtxFree->vopMutex.mlk);
        pthread_mutex_unlock(&vopCtxFree->vopMutex.mtx);
        return 0;
    }

    vopCtx = NULL;
    while(!vopCtxFree->mVopDevs.empty()) {
        isize = vopCtxFree->mVopDevs.size();
        vopDevice *vopDev = vopCtxFree->mVopDevs.at(isize-1);
        if(!vopDev) continue;
        while(!vopDev->mVopWinS.empty()) {
            jsize = vopDev->mVopWinS.size();
            vopWin* iter = vopDev->mVopWinS.at(jsize-1);
            if(!iter) continue;
            if(iter->winFd > -1 && iter->winFd != 0)
                close(iter->winFd);
            free(iter);
            vopDev->mVopWinS.pop_back();
        }
        if(vopDev->vopFd > -1 && vopDev->vopFd != 0)
            close(vopDev->vopFd);
        if(vopDev->screenFd > -1 && vopDev->screenFd != 0)
            close(vopDev->screenFd);
        if(vopDev->hwVsyncFd > -1 && vopDev->hwVsyncFd != 0)
            close(vopDev->hwVsyncFd);
        free(vopDev);
        vopCtxFree->mVopDevs.pop_back();
    }

    pthread_mutex_unlock(&vopCtxFree->vopMutex.mtx);
    free_thread_pamaters(&vopCtxFree->vopMutex);
    free(vopCtxFree);
    return 0;
}

int vop_init_devices(void** vopctx)
{
    int ret = 0;
    //ALOGD("wzqtest %s,%d",__func__,__LINE__);
    if(!vopCtx) {
        vopCtx = (vopContext*)malloc(sizeof(vopContext));
        if(!vopCtx) {
            ALOGE("Malloc vop context fail for %s",strerror(errno));
            ret = -errno;
            goto OnError;
        } else {
            memset(vopCtx,0,sizeof(vopContext));
            init_thread_pamaters(&vopCtx->vopMutex);
        }
    }

    //ALOGD("wzqtest %s,%d",__func__,__LINE__);
    pthread_mutex_lock(&vopCtx->vopMutex.mtx);
    //ALOGD("wzqtest %s,%d",__func__,__LINE__);
    if(!vopCtx) {
        ALOGE("vopCtx is null");
        ret = -errno;
        goto OnError;
    }

    vop_collect_nodes(vopCtx);
    //ALOGD("wzqtest %s,%d",__func__,__LINE__);
    for(size_t i = 0;i < vopCtx->mVopDevs.size();i++) {
        vopDevice* vopDev = vopCtx->mVopDevs.at(i);
        ret = vop_init_device(vopDev);
        if (0 != ret) VOPONERROR(errno);
    }
    //ALOGD("wzqtest %s,%d",__func__,__LINE__);
    *vopctx = vopCtx;
    return ret;
OnError:
    if(vopCtx) {
        if(vopCtx->vopMutex.count == 0) {
            vop_exit_devices((void*)vopCtx);
        } else
            pthread_mutex_unlock(&vopCtx->vopMutex.mtx);
    }
    return ret;
}

int vop_free_devices(void** vopctx)
{
    int ret = 0;
    if(vopCtx != *vopctx) {
        ALOGE("VOP context is not this owner");
        return -EINVAL;
    }
    pthread_mutex_lock(&vopCtx->vopMutex.mtx);
    if(!vopCtx) {
        ALOGE("vopCtx is null");
        ret = -errno;
        goto OnError;
    }
    ret = vop_exit_devices(vopCtx);

OnError:
    return ret;
}

int vop_dump_win(vopWin* vopWin)
{
    int ret = 0;
    if(!vopWin) {
        ALOGD("vop win not exit");
        return -ENODEV;
    }
    ALOGI("winIndex=%d | winFd=%4d | winFeature=0x%x | max_input_x=0x%6x | max_intput_y=0x%6x | nodeName=%s\n",
        vopWin->winIndex,vopWin->winFd,vopWin->winFeature,vopWin->max_input_x,
        vopWin->max_input_y,vopWin->nodeName.c_str());
    return ret;
}

int vop_dump_device(vopDevice* vopDev)
{
    int ret = 0;
    if(!vopDev) {
        ALOGD("vop dev not exit");
        return -ENODEV;
    }
    ALOGI("vopid=%d,fbSize=0x%x,lcdSize=0x%x,vopFeature=0x%x,vsync_period=0x%x,vopFd=%d,iommuEn=%d,screenFd=%d\n"
        "hwVsyncFd=%d,allWinNums=%d,allAreaNums=%d,multAreaWinNums=%d,isPause=%d,isActive=%d,connected=%d,timeStamp=0x%x",
        vopDev->vopid,vopDev->fbSize,vopDev->lcdSize,vopDev->vopFeature,vopDev->vsync_period,
        vopDev->vopFd,vopDev->iommuEn,vopDev->screenFd,vopDev->hwVsyncFd,vopDev->allWinNums,vopDev->allAreaNums, 
        vopDev->multAreaWinNums,vopDev->isPause,vopDev->isActive,vopDev->connected,vopDev->timeStamp);
    ALOGI("-----------|------------|-----------------|----------------------|-----------------------|------");
    for(size_t i = 0;i < vopDev->mVopWinS.size();i++) {
        vopWin* vopWin = vopDev->mVopWinS.at(i);
        ret = vop_dump_win(vopWin);
    }
    return ret;
}

void vop_dump(void* vopctx)
{
    int ret = 0;
    vopContext* vopCtx = (vopContext*)vopctx;
    if(!vopCtx) return;
    ALOGI("\nsize of vop devices:%d\n",vopCtx->mVopDevs.size());
    for(size_t i = 0;i < vopCtx->mVopDevs.size();i++) {
        ALOGI("vop device %4d:",i);
        vopDevice* vopDev = vopCtx->mVopDevs.at(i);
        ret = vop_dump_device(vopDev);
    }
    return;
}
