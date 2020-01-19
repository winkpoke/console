/**
* File: IRayErrDef.h
*
* Purpose: IRay FPD error code definition
*
*
* @author Haitao.Ning
* @version 1.0 2015/4/23
*
* Copyright (C) 2009, 2015, iRay Technology (Shanghai) Ltd.
*
*/
#ifndef _IRAY_ERROR_DEF_H_
#define _IRAY_ERROR_DEF_H_

typedef int FPDRESULT;

const FPDRESULT Err_OK = 0;
const FPDRESULT Err_TaskPending = 1;
const FPDRESULT Err_Unknown = 2;
const FPDRESULT Err_DuplicatedCreation = 3;
const FPDRESULT Err_DetectorIdNotFound = 4;
const FPDRESULT Err_StateErr = 5;
const FPDRESULT Err_NotInitialized = 6;
const FPDRESULT Err_NotImplemented = 7;
const FPDRESULT Err_AccessDenied = 8;
const FPDRESULT Err_LoadDllFailed = 9;
const FPDRESULT Err_DllCreateObjFailed = 10;
const FPDRESULT Err_OpenFileFailed = 11;
const FPDRESULT Err_FileNotExist = 12;
const FPDRESULT Err_ConfigFileNotExist = 13;
const FPDRESULT Err_TemplateFileNotExist = 14;
const FPDRESULT Err_TemplateFileNotMatch = 15;
const FPDRESULT Err_InvalidFileFormat = 16;
const FPDRESULT Err_CreateLoggerFailed = 17;
const FPDRESULT Err_InvalidParamCount = 18;
const FPDRESULT Err_InvalidParamType = 19;
const FPDRESULT Err_InvalidParamValue = 20;
const FPDRESULT Err_PreCondition = 21;
const FPDRESULT Err_TaskTimeOut = 22;
const FPDRESULT Err_ProdInfoMismatch = 23;
const FPDRESULT Err_DetectorRespTimeout = 24;
const FPDRESULT Err_InvalidPacketNo = 25;
const FPDRESULT Err_InvalidPacketFormat = 26;
const FPDRESULT Err_PacketDataCheckFailed = 27;
const FPDRESULT Err_PacketLost_BufOverflow = 28;
const FPDRESULT Err_FrameLost_BufOverflow = 29;
const FPDRESULT Err_ImgChBreak = 30;
const FPDRESULT Err_BadImgQuality = 31;
const FPDRESULT Err_GeneralSocketErr = 32;
const FPDRESULT Err_DetectorSN_Mismatch = 33;
const FPDRESULT Err_CommDeviceNotFound = 34;
const FPDRESULT Err_CommDeviceOccupied = 35;
const FPDRESULT Err_CommParamNotMatch = 36;
const FPDRESULT Err_NotEnoughDiskSpace = 37;
const FPDRESULT Err_NotEnoughMemorySpace = 38;
const FPDRESULT Err_ApplyFirmwareFailed = 39;
const FPDRESULT Err_CallbackNotFinished = 40;
const FPDRESULT Err_FirmwareUpdated = 41;
const FPDRESULT Err_TooMuchDefectPoints = 42;
const FPDRESULT Err_TooLongFilePath = 43;
const FPDRESULT Err_FPD_General_Detector_Error = 1001;
const FPDRESULT Err_FPD_General_ControlBox_Error = 1002;
const FPDRESULT Err_FPD_General_FirmwareUpgrade_Error = 1003;
const FPDRESULT Err_FPD_General_GSensor_Error = 1004;
const FPDRESULT Err_FPD_NotImplemented = 1005;
const FPDRESULT Err_FPD_SeqNoOutOfSync = 1006;
const FPDRESULT Err_FPD_Busy = 1007;
const FPDRESULT Err_FPD_Busy_Initializing = 1018;
const FPDRESULT Err_FPD_Busy_Last_Command_Suspending = 1019;
const FPDRESULT Err_FPD_Busy_Mode_Not_Supported = 1020;
const FPDRESULT Err_FPD_Busy_MCU_Busy = 1021;
const FPDRESULT Err_FPD_Busy_FPGA_Busy = 1022;
const FPDRESULT Err_FPD_Busy_FPGA_Timeout = 1023;
const FPDRESULT Err_FPD_Busy_Doing_Dynamic_Ghost = 1024;
const FPDRESULT Err_FPD_Busy_Doing_Dynamic_Preoffset = 1025;
const FPDRESULT Err_FPD_Busy_FTP_Image_Uploading = 1026;
const FPDRESULT Err_FPD_Busy_Capture_State_Recover = 1027;
const FPDRESULT Err_FPD_Busy_System_Error = 1028;
const FPDRESULT Err_FPD_Occupied = 1008;
const FPDRESULT Err_FPD_SleepWakeupFailed = 1009;
const FPDRESULT Err_FPD_SleepCaptureError = 1010;
const FPDRESULT Err_FPD_CmdExecuteTimeout = 1011;
const FPDRESULT Err_FPD_FirmwareFallback = 1012;
const FPDRESULT Err_FPD_NotSupportInCurrMode = 1013;
const FPDRESULT Err_FPD_NoEnoughStorageSpace = 1014;
const FPDRESULT Err_FPD_FileNotExist = 1015;
const FPDRESULT Err_FPD_FtpServerAccessError = 1016;
const FPDRESULT Err_FPD_HWCaliFileError = 1017;
const FPDRESULT Err_FTP_OpenLoginFailed = 2001;
const FPDRESULT Err_FTP_MkdirCdFailed = 2002;
const FPDRESULT Err_FTP_LocalFileOpenFailed = 2003;
const FPDRESULT Err_FTP_UploadFailed = 2004;
const FPDRESULT Err_FTP_DownloadFailed = 2005;
const FPDRESULT Err_FTP_FileVerifyFailed = 2006;
const FPDRESULT Err_FTP_TypeError = 2007;
const FPDRESULT Err_Cali_GeneralError = 3001;
const FPDRESULT Err_Cali_UnexpectImage_DoseHighHigh = 3002;
const FPDRESULT Err_Cali_UnexpectImage_ExpLineNotSatisfy = 3003;
const FPDRESULT Err_Cali_UnexpectImage_MistakeTrigger = 3004;
const FPDRESULT Err_Cali_DataNotReadyForGen = 3005;
const FPDRESULT Err_Cali_NotEnoughIntervalTime_OffsetTmpl = 3006;

#endif
