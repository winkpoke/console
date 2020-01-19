/**
* File: IRayEventDef.h
*
* Purpose: IRay FPD callback event definition
*
*
* @author Haitao.Ning
* @version 1.0 2015/4/23
*
* Copyright (C) 2009, 2015, iRay Technology (Shanghai) Ltd.
*
*/
#ifndef _IRAY_EVENT_DEF_H_
#define _IRAY_EVENT_DEF_H_

const int Evt_GeneralInfo = 1; // General Info
const int Evt_GeneralWarn = 2; // General Warn
const int Evt_GeneralError = 3; // General Error
const int Evt_TaskResult_Succeed = 4; // Task succeed
const int Evt_TaskResult_Failed = 5; // Task failed
const int Evt_TaskResult_Canceled = 6; // Task been canceled
const int Evt_AutoTask_Started = 7; // Task is started which is not emitted by user command, outer or AED image acquiring will trigger this event.
const int Evt_ScanOnce_Request = 8; // Request scanner to start a broadcast scanning
const int Evt_ImageFileUpload_Result = 9; // Image file upload result notify
const int Evt_TemplateFileUpload_Result = 10; // Template file upload result notify
const int Evt_TemplateFileDownload_Result = 11; // Template file download result notify
const int Evt_HwCaliTemplateList = 12; // Return hardware calibration template list
const int Evt_HwWiFiScanList = 13; // Return hardware WiFi scan list
const int Evt_HistoricalImageList = 14; // Return hardware historical image list
const int Evt_TimeDiff = 15; // Return time difference between detector and PC in seconds
const int Evt_LivingTime = 16; // Return living time from detector in seconds
const int Evt_TransactionAborted = 17; // Notify that current transaction was aborted
const int Evt_Image = 1001; // Image received
const int Evt_Prev_Image = 1002; // Preview image received
const int Evt_Retransfer_Image = 1012; // Retransfered image received after break and re-connecting
const int Evt_WaitImage_Timeout = 1003; // Acqusition operation time out(equal: Possiable Image Loss)
const int Evt_Exp_Prohibit = 1004; // Exposure prohibit
const int Evt_Exp_Enable = 1005; // Exposure enable
const int Evt_Exp_Timeout = 1006; // Exposure timeout
const int Evt_OffsetAmassingTime = 1008; // Pixel base amassing time,typically from previous clear operation to current acquisition
const int Evt_MistakenTrigger = 1009; // Mistaken trigger in FreeSync mode
const int Evt_Image_Trans_Slow = 1010; // Warn that image transfer speed low, usually at wireless connection.
const int Evt_Image_Abandon = 1011; // Warn that currently acquiring image abandoned because bad network situation
const int Evt_ConnectProcess = 2001; // Report Process info while connecting
const int Evt_CommFailure = 2002; // Communication failed
const int Evt_TemperatureHigh = 2003; // Temperature too high
const int Evt_FpsOutOfRange = 2004; // Fps too high or too low
const int Evt_LowBattery = 2005; // Low battery warn
const int Evt_TemplateOverDue = 2006; // Calibration template file over due

#endif
