/**
* File: IRayCmdDef.h
*
* Purpose: IRay command definition
*
*
* @author Haitao.Ning
* @version 1.0 2015/4/23
*
* Copyright (C) 2009, 2015, iRay Technology (Shanghai) Ltd.
*
*/
#ifndef _IRAY_COMMAND_DEF_H_
#define _IRAY_COMMAND_DEF_H_

const int Cmd_SetLogLevel = 1;
const int Cmd_Connect = 2;
const int Cmd_Disconnect = 3;
const int Cmd_Sleep = 4;
const int Cmd_Wakeup = 5;
const int Cmd_SetCorrectOption = 6;
const int Cmd_SetCaliSubset = 7;
const int Cmd_Reset = 8;
const int Cmd_Clear = 1001;
const int Cmd_ClearAcq = 1002;
const int Cmd_Acq2 = 1003;
const int Cmd_AecPreExp = 1016;
const int Cmd_StartAcq = 1004;
const int Cmd_StopAcq = 1005;
const int Cmd_ForceSingleAcq = 1006;
const int Cmd_ForceContinuousAcq = 1007;
const int Cmd_ForceDarkSingleAcq = 1008;
const int Cmd_ForceDarkClearAcq = 1009;
const int Cmd_ForceDarkContinuousAcq = 1010;
const int Cmd_ProhibOutExp = 1011;
const int Cmd_EnableOutExp = 1012;
const int Cmd_SyncStart = 1013;
const int Cmd_SyncCancel = 1014;
const int Cmd_SyncAcq = 1015;
const int Cmd_ReadUserROM = 2001;
const int Cmd_WriteUserROM = 2002;
const int Cmd_ReadUserRAM = 2030;
const int Cmd_WriteUserRAM = 2031;
const int Cmd_ReadFactoryROM = 2003;
const int Cmd_WriteFactoryROM = 2004;
const int Cmd_ReadVtMap = 2005;
const int Cmd_WriteVtMap = 2006;
const int Cmd_Recover = 2007;
const int Cmd_UpdateFirmware = 2008;
const int Cmd_SetImgChannel = 2009;
const int Cmd_ReadTemperature = 2010;
const int Cmd_ReadHumidity = 2011;
const int Cmd_UploadDetectorLog = 2012;
const int Cmd_UploadShockLog = 2013;
const int Cmd_ClearShockLog = 2014;
const int Cmd_WriteShockThreshold = 2015;
const int Cmd_ReadShockThreshold = 2016;
const int Cmd_ReadBatteryStatus = 2017;
const int Cmd_SetTimeByDiff = 2018;
const int Cmd_QueryTimeDiff = 2019;
const int Cmd_QueryLivingTime = 2020;
const int Cmd_ReadWifiStatus = 2021;
const int Cmd_QueryWifiScanList = 2022;
const int Cmd_WriteWifiSettings = 2023;
const int Cmd_ReadWifiSettings = 2024;
const int Cmd_DownloadWorkList = 2025;
const int Cmd_QueryHistoricalImageList = 2026;
const int Cmd_SelectWorkItem = 2027;
const int Cmd_UploadHistoricalImages = 2028;
const int Cmd_StopUploadingHistoricalImages = 2029;
const int Cmd_OffsetGeneration = 3001;
const int Cmd_GainInit = 3002;
const int Cmd_GainSelectCurrent = 3004;
const int Cmd_GainSelectAll = 3005;
const int Cmd_GainGeneration = 3006;
const int Cmd_DefectInit = 3007;
const int Cmd_LoadTemporaryDefectFile = 3008;
const int Cmd_DefectSelectCurrent = 3009;
const int Cmd_DefectSelectAll = 3033;
const int Cmd_DefectGeneration = 3010;
const int Cmd_LagInit = 3012;
const int Cmd_LagSelectCurrent = 3013;
const int Cmd_LagGeneration = 3014;
const int Cmd_UpdateFreqCompositeCoeff = 3032;
const int Cmd_FinishGenerationProcess = 3015;
const int Cmd_DeleteGenerationTempFile = 3016;
const int Cmd_DownloadCaliFile = 3017;
const int Cmd_UploadCaliFile = 3018;
const int Cmd_SelectCaliFile = 3019;
const int Cmd_HwGeneratePreOffsetTemplate = 3020;
const int Cmd_QueryHwCaliTemplateList = 3021;
const int Cmd_ApplyDefectCorrection = 3022;
const int Cmd_RequestSyncboxControl = 3023;
const int Cmd_ReleaseSyncboxControl = 3024;
const int Cmd_ReadOutExpEnableState = 3025;
const int Cmd_EnableAutoSleep = 3026;
const int Cmd_DisableAutoSleep = 3027;
const int Cmd_ReadAutoSleepEnableState = 3028;
const int Cmd_PowerOff = 3029;
const int Cmd_StartAutoCleanup = 3030;
const int Cmd_StopAutoCleanup = 3031;
const int Cmd_PanelRecover = 3034;
const int Cmd_Debug_ReadCRC = 10001;

#endif
