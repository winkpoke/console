#ifndef _FLAT_PANEL_IRAY_INCLUDE_H_
#define _FLAT_PANEL_IRAY_INCLUDE_H_

#include "DisplayProgressbar.h"
#include "Detector.h"
#include "IRayImage.h"

namespace control::fpd {
    struct iray_t {
        using on_image_f = std::function<void(int width, int height, int byte_per_pixel, void* data)>;
        struct mode_t {
            int index;
            int PGA;
            int binning;
            int zoom;
            float freq;
            char subset[32];
        } mode;
        CDetector* detector;
        int acquire_time = 5000;
    };

    bool init(iray_t* iray);
    void drop(iray_t* iray);


    struct ApplicatioMode
    {
        int Index;
        int PGA;
        int Binning;
        int Zoom;
        float Freq;
        char subset[32];
    };

    static CDetector* gs_pDetInstance = NULL;
    static int s_nAcquireTime = 5000;

    int Initializte();
    void Deinit();

    //typedef void (*FP_CALLBACK_IMAGE_RECIEVED)(int width, int height, int byte_per_pixel, void* data);
    using FP_CALLBACK_IMAGE_RECIEVED = std::function<void(int width, int height, int byte_per_pixel, void* data)>;

    void SDKCallbackHandler(int nDetectorID, int nEventID, int nEventLevel,
        const char* pszMsg, int nParam1, int nParam2, int nPtrParamLen, void* pParam);

    bool fp_init();

    FPDRESULT fp_start_acquire();

    FPDRESULT fp_stop_acquire();

    void fp_set_callback_image_recieved(FP_CALLBACK_IMAGE_RECIEVED f);

    ApplicatioMode GetAppModeAttr();

    int Initializte();

    void Deinit();
}
#endif // !_FLAT_PANEL_INCLUDE_H_

#ifdef FLAT_PANEL_IRAY_IMPLEMENTATION
#ifndef FLAT_PANEL_IRAY_IMPLEMENTED
#define FLAT_PANEL_IRAY_IMPLEMENTED
//#pragma once

// logging
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "DisplayProgressbar.h"
#include "Detector.h"
#include "IRayImage.h"

namespace control::fpd {
    using namespace spdlog;

    int Initializte();
    void Deinit();

    FP_CALLBACK_IMAGE_RECIEVED fp_callback_image_recieved;

    void SDKCallbackHandler(int nDetectorID, int nEventID, int nEventLevel,
        const char* pszMsg, int nParam1, int nParam2, int nPtrParamLen, void* pParam)
    {
        gs_pDetInstance->SDKCallback(nDetectorID, nEventID, nEventLevel, pszMsg, nParam1, nParam2, nPtrParamLen, pParam);
        IRayImage* img;
        switch (nEventID)
        {
        case Evt_Image:
            img = (IRayImage*)pParam;
            if (fp_callback_image_recieved != NULL) {
                fp_callback_image_recieved(img->nWidth, img->nHeight, img->nBytesPerPixel, img->pData);
            }

            break;
        default:
            break;
        }
    }

    bool fp_init()
    {
        fp_callback_image_recieved = NULL;
        if (Err_OK != Initializte()) {
            Deinit();
            return false;
        }
        return true;
    }

    FPDRESULT fp_start_acquire()
    {
        if (gs_pDetInstance != NULL) {
            return gs_pDetInstance->Invoke(Cmd_StartAcq);
        }
        else {
            return NULL;
        }
    }

    FPDRESULT fp_stop_acquire()
    {
        if (gs_pDetInstance != NULL) {
            return gs_pDetInstance->SyncInvoke(Cmd_StopAcq, 2000);
        }
        else {
            return NULL;
        }
    }

    void fp_set_callback_image_recieved(FP_CALLBACK_IMAGE_RECIEVED f) {
        fp_callback_image_recieved = f;
    }

    ApplicatioMode GetAppModeAttr()
    {
        //Attr_CurrentCorrectOption, Attr_UROM_PGA, Attr_UROM_BinningMode, Attr_UROM_ZoomMode, Attr_UROM_SequenceIntervalTime
        ApplicatioMode mode = { 0 };
        AttrResult attr;
        gs_pDetInstance->GetAttr(Attr_UROM_PGA, attr);
        mode.PGA = attr.nVal;
        gs_pDetInstance->GetAttr(Attr_UROM_BinningMode, attr);
        mode.Binning = attr.nVal;
        gs_pDetInstance->GetAttr(Attr_UROM_ZoomMode, attr);
        mode.Zoom = attr.nVal;
        gs_pDetInstance->GetAttr(Attr_UROM_SequenceIntervalTime, attr);
        mode.Freq = (attr.nVal == 0) ? 0 : 1000.0 / attr.nVal;
        return mode;
    }

    int Initializte()
    {
        gs_pDetInstance = new CDetector();

        int ret = gs_pDetInstance->LoadIRayLibrary();
        if (Err_OK != ret)
        {
            SPDLOG_ERROR("Load libray.. failed");
            return ret;
        }
        else {
            SPDLOG_INFO("Load libray.. succeeded");
        }

        ret = gs_pDetInstance->Create(GetWorkDirPath().c_str(), SDKCallbackHandler);
        if (Err_OK != ret)
        {
            SPDLOG_ERROR("Create instance.. failed::{}", gs_pDetInstance->GetErrorInfo(ret).c_str());
            return ret;
        }
        else {
            SPDLOG_INFO("Create instance.. succeeded");
        }

        ret = gs_pDetInstance->SyncInvoke(Cmd_Connect, 30000);
        if (Err_OK != ret)
        {
            SPDLOG_ERROR("Connect device.. failed::{}", gs_pDetInstance->GetErrorInfo(ret).c_str());
            return ret;
        }
        else
            SPDLOG_INFO("Connect device.. succeeded");

        ret = gs_pDetInstance->SyncInvoke(Cmd_SetCaliSubset, "Mode1", 5000);
        if (Err_OK == ret)
        {
            SPDLOG_INFO("Set calibration subset.. succeeded");
            ApplicatioMode mode = GetAppModeAttr();
            SPDLOG_INFO("    [ PGA:{:d}, binning:{:d}, zoom:{:d}, Freq:{:.1f} ]", mode.PGA, mode.Binning, mode.Zoom, mode.Freq);
        }
        else
            SPDLOG_ERROR("Set calibration subset.. failed");

        //TRACE("Set correction option");
        //ret = gs_pDetInstance->SyncInvoke(Cmd_SetCorrectOption, Enm_CorrectOp_HW_PreOffset | Enm_CorrectOp_HW_Gain | Enm_CorrectOp_HW_Defect, 5000);
        //if (Err_OK != ret)
        //{
        //    TRACE("\t\t[No ]\n");
        //    return ret;
        //}
        //else
        //    TRACE("\t\t[Yes]\n");

        return ret;
    }


    void Deinit()
    {
        if (gs_pDetInstance)
        {
            gs_pDetInstance->Destroy();
            gs_pDetInstance->FreeIRayLibrary();
            delete gs_pDetInstance;
            gs_pDetInstance = NULL;
        }
    }
}
#endif // !FLAT_PANEL_IRAY_IMPLEMENTED
#endif // FLAT_PANEL_IRAY_IMPLEMENTATION
