/**
* File: IRayFpdSys.h
*
* Purpose: Main access interface of iRay flat panel SDK.
*
* @author Haitao.Ning
* @version 1.0 2015/02/02
*
* Copyright (C) 2009, 2015, iRay Technology (Shanghai) Ltd.
*
* @code: sample code for demostrating IRayFpdSys.DLL interface only, 
*        more error-handling shall be performed in user's program, detail see "Programming Guide" document.
* ---------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "IRayFpdSys.h"

#define TEST_WORKDIR_1 "D:\\MyDR\\MyDetector1"
#define TEST_WORKDIR_2 "D:\\MyDR\\MyDetector2"

void MyCallback(int nDetectorID, int nEventID, int nEventLevel,
	const char* pszMsg, int nParam1, int nParam2, int nPtrParamLen, void* pParam);

FnCreate  g_fpCreate;
FnDestroy g_fpDestroy;
FnGetAttr g_fpGetAttr;
FnSetAttr g_fpSetAttr;
FnInvoke  g_fpInvoke;

int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE hModule = LoadLibraryA("FpdSys.DLL");
	if (NULL == hModule)
	{
		printf("\r\n----Load SDK DLL failed!----");

		return 0;
	}

	g_fpCreate = (FnCreate)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_CREATE);
	g_fpDestroy = (FnDestroy)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_DESTROY);
	g_fpGetAttr = (FnGetAttr)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_GETATTR);
	g_fpSetAttr = (FnSetAttr)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_SETATTR);
	g_fpInvoke = (FnInvoke)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_INVOKE);
	if (NULL == g_fpCreate || NULL == g_fpDestroy || NULL == g_fpGetAttr ||
		NULL == g_fpSetAttr || NULL == g_fpInvoke)
	{
		printf("\r\n----Get exported function failed!----");
		FreeLibrary(hModule);

		return 0;
	}

	FPDRESULT result;

	int nDetectorID1;
	result = g_fpCreate(TEST_WORKDIR_1, &MyCallback, &nDetectorID1);
	if (Err_OK == result)
	{
		IRayVariant var;
		var.vt = IVT_INT;
		var.val.nVal = 1;
		result = g_fpSetAttr(nDetectorID1, Cfg_ComPort, &var);
		if (Err_OK != result)
		{
			printf("\r\n----Test set attribute data failed!----");
		}
		else
		{
			printf("\r\n----Test set attribute data succeed----");
		}

		result = g_fpGetAttr(nDetectorID1, Attr_State, &var);
		if (Err_OK != result)
		{
			printf("\r\n----Test get attribute data failed!----");
		}
		else
		{
			printf("\r\n----Test get attribute data succeed, value = %d----", var.val.nVal);
		}
	}
	else
	{
		printf("\r\n----Create detector object failed!----");
		FreeLibrary(hModule);

		return 0;
	}

	int nDetectorID2;
	result = g_fpCreate(TEST_WORKDIR_2, &MyCallback, &nDetectorID2);
	if (Err_OK == result)
	{
		result = g_fpInvoke(nDetectorID2, Cmd_Connect, NULL, 0);
		if (Err_TaskPending == result)
		{
			printf("\r\n----Test command invoke is pending, please wait task result event.----");
		}
		if (Err_OK == result)
		{
			printf("\r\n----Test command invoke succeed!----");
		}
		else
		{
			printf("\r\n----Test command invoke failed! ErrCode = %d----", result);
		}
	}
	else
	{
		printf("\r\n----Create detector object failed!----");
		g_fpDestroy(nDetectorID1);
		FreeLibrary(hModule);

		return 0;
	}

	while (getchar() != 'q')
	{
		Sleep(10);
	}

	g_fpDestroy(nDetectorID1);
	g_fpDestroy(nDetectorID2);

	FreeLibrary(hModule);

	return 0;
}

void MyCallback(int nDetectorID, int nEventID, int nEventLevel,
	const char* pszMsg, int nParam1, int nParam2, int nPtrParamLen, void* pParam)
{
	switch (nEventID)
	{
	case Evt_TaskResult_Succeed:
		if (Cmd_Connect == nParam1)
		{
			printf("\r\n----Test event callback succeed, Detector connected: ID = %d----", nDetectorID);
		}
		break;
	default:
		break;
	}
}

* ---------------------------------------------------------------------------------------------------------
* @endcode
*
*/
#ifndef _IRAY_FPD_SYSTEM_INTERFACE_H_
#define _IRAY_FPD_SYSTEM_INTERFACE_H_  

#include "IRayVariant.h"
#include "IRayImage.h"
#include "IRayErrDef.h"
#include "IRayCmdDef.h"
#include "IRayAttrDef.h"
#include "IRayEnumDef.h"
#include "IRayEventDef.h"

#define IRAY_FPD_PROC_NAME_VERSION   "GetSDKVersion"
#define IRAY_FPD_PROC_SET_USERCODE   "SetUserCode"

#define IRAY_FPD_PROC_NAME_REGISTER_SCANNotify  "RegisterScanNotify"
#define IRAY_FPD_PROC_NAME_SCAN                 "ScanOnce"

#define IRAY_FPD_PROC_NAME_CREATE   "Create"
#define IRAY_FPD_PROC_NAME_DESTROY  "Destroy"
#define IRAY_FPD_PROC_NAME_GETATTR  "GetAttr"
#define IRAY_FPD_PROC_NAME_SETATTR  "SetAttr"
#define IRAY_FPD_PROC_NAME_INVOKE   "Invoke"
#define IRAY_FPD_PROC_NAME_ABORT    "Abort"

#define IRAY_FPD_PROC_NAME_DEFECT_OPEN       "OpenDefectTemplateFile"
#define IRAY_FPD_PROC_NAME_DEFECT_SAVE       "SaveDefectTemplateFile"
#define IRAY_FPD_PROC_NAME_DEFECT_CLOSE      "CloseDefectTemplateFile"
#define IRAY_FPD_PROC_NAME_EXPLINE_CORRECT   "DoExpLineCorrect"

#define IRAY_FPD_PROC_NAME_SYNC_CREATE              "SyncBoxCreate"
#define IRAY_FPD_PROC_NAME_SYNC_DESTROY             "SyncBoxDestroy"
#define IRAY_FPD_PROC_NAME_SYNC_BIND                "SyncBoxBind"
#define IRAY_FPD_PROC_NAME_SYNC_GET_BIND            "SyncBoxGetBind"
#define IRAY_FPD_PROC_NAME_SYNC_GET_TUBEREADYTIME   "SyncBoxGetTubeReadyTime"
#define IRAY_FPD_PROC_NAME_SYNC_SET_TUBEREADYTIME   "SyncBoxSetTubeReadyTime"
#define IRAY_FPD_PROC_NAME_SYNC_GET_STATE           "SyncBoxGetState"


/**
* FnGetSDKVersion: Define a function type for DLL export function "GetSDKVersion"
*
* GetSDKVersion: Get IRay FPD SDK software version 
*
* @param pszVersion [out] buffer to receive the version infomation
*        for example "4.0.1.33" (MainVer.Function.Fixing.Build)         
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnGetSDKVersion)(char pszVersion[32]);

/**
* FnSetUserCode: Define a function type for DLL export function "SetUserCode"
*
* SetUserCode: Set user code to obtain the authority specified by the license
*
* @param pszUserCode [in] User code to match the license
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnSetUserCode)(char* pszUserCode);

/**
* DetectorProfile: Define a data struct for scan and profile functions
*/
#pragma pack(1)
typedef struct _tagDetectorProfile
{
	int nProdNo;
	int nSubProdNo;
	char szSN[64];
	char szIP[64];
	char szDetConfig[512];
	int  bDetState; // 0:not busy, 1:busy
}DetectorProfile;
#pragma pack()

/**
* FnNotifyScanResult: Define a function type for broadcast scanning callback, 
*         Note that: the data is only avaliable within the calling
*
* @param pDetectorProfile [in] scan result data struct
*
* @return void
*/
typedef void(*FnNotifyScanResult)(DetectorProfile* pDetectorProfile);

/**
* FnRegisterScanNotify: Define a function type for DLL export function "RegisterScanNotify"
*
* RegisterScanNotify: register the scanning result callback function
*
* @param pCallback [in] callback function pointer, return the scan result from operation "Scan once" or
*                       firmware pushed online notification.
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnRegisterScanNotify)(FnNotifyScanResult pCallback);

/**
* FnScanOnce: Define a function type for DLL export function "ScanOnce"
*
* ScanOnce:  Start a broadcast scan to find reachable detectors, the result notification will be 
*            returned with registered callback. 
*
* @param pszIP [in] IP address of selected local network card, NULL or "0.0.0.0" means broadcast all network cards
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnScanOnce)(char* pszSourceIP);

/**
* FnCallback: Define a function type for SDK callback,
*             this definition will be used as an function argument while creating a detector object.
*
* @param nDetectorID [in] the detector which fired the event
* @param nEventID [in] event ID
* @param nEventLevel [in] event level
* @param pszMsg [in] string description for the event
* @param nParam1 [in] parameter with the event, defined for a certain event
* @param nParam2 [in] parameter with the event, defined for a certain event
* @param nPtrParamLen [in] bytes count for pointer typed parameter, defined for a certain event
* @param pParam [in] pointer typed parameter, defined for a certain event
*
* @return void
*/
typedef void(*FnCallback)(int nDetectorID, int nEventID, int nEventLevel, 
	const char* pszMsg, int nParam1, int nParam2, int nPtrParamLen, void* pParam);

/**
* FnCreate: Define a function type for DLL export function "Create"
*
* Create: Create memory detector object, 
*         if work dir has been used, function call will return error, together with the occuppied detector ID.
*
* @param pszWorkDir [in] set the working directory for a certain flat panel device
* @param fpCallback [in] set the function pointer for SDK event callback
* @param pDetectorID [out] buffer to receive the created object ID
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnCreate)(const char* pszWorkDir, FnCallback fpCallback, int* pDetectorID);

/**
* FnDestroy: Define a function type for DLL export function "Destroy"
*
* Destroy: Close the connection of specified detector, release the corresponding memory object
*
* @param nDetectorID [in] the ID of memory detector object
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnDestroy)(int nDetectorID);

/**
* FnGetAttr: Define a function type for DLL export function "GetAttr"
*
* GetAttr: Get attribute value
*
* @param nDetectorID [in] specify a detector
* @param nAttrID [in] specify an attribute for getting
* @param pVar [out] buffer to receive the value
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnGetAttr)(int nDetectorID, int nAttrID, IRayVariant* pVar);

/**
* FnSetAttr: Define a function type for DLL export function "SetAttr"
*
* SetAttr: Set attribute value
*
* @param nDetectorID [in] specify a detector
* @param nAttrID [in] specify an attribute for setting
* @param pVar [in] variant to set
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnSetAttr)(int nDetectorID, int nAttrID, const IRayVariant* pVar);

/**
* FnInvoke: Define a function type for DLL export function "Invoke"
*
* Invoke: Call SDK method using a pre-defined command ID,
*         if "pending" returned, it means an asynchronous task is started and the result will be callback later.
*
* @param nDetectorID [in] specify a detector
* @param nCommandID [in] specify an command for calling
* @param pars [in] parameter list for a certain command
* @param nParCount [in] the count of parameters
*
* @return 0: succeed, Non-Zero: "pending state" or error code
*/
typedef int(*FnInvoke)(int nDetectorID, int nCommandID, IRayCmdParam pars[], int nParCount);

/**
* FnAbort: Define a function type for DLL export function "Abort"
*
* Abort: Abort the current transation processing flow
*
* @param nDetectorID [in] specify a detector
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnAbort)(int nDetectorID);

/**
* FnOpenDefectTemplateFile: Define a function type for DLL export function "OpenDefectTemplateFile"
*
* OpenDefectTemplateFile: Load the specified defect template file for editing
*
* @param pszFilePath [in] file path
* @param ppHandler [out] return the handler of the opened file
* @param pWidth [out] return width of the image
* @param pHeight [out] return height of the image
* @param ppPoints [out] return defect point map of the detector, for each element 0 means normal, 1 means defect
* @param ppRows [out] return defect row map of the detector, for each element 0 means normal, 1 means defect
* @param ppCols [out] return defect column map of the detector, for each element 0 means normal, 1 means defect
*                     , if ppDualReadCols2 not NULL, ppCols means  defect column map of the upper part
* @param ppDualReadCols2 [out] only for the 'Dual-Read' detector, return defect column map of the bottom part
*                     , NULL means not Dual-Read
*
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnOpenDefectTemplateFile)(
	const char* pszFilePath,
	void** ppHandler,
	unsigned short* pWidth,
	unsigned short* pHeight,
	char** ppPoints,       // arrayLen = width * height
	char** ppRows,         // arrayLen = height
	char** ppCols,         // arrayLen = width
	char** ppDualReadCols2 // arrayLen = width
	);

/**
* FnSaveDefectTemplateFile: Define a function type for DLL export function "SaveDefectTemplateFile"
*
* SaveDefectTemplateFile: Save the opened defect template file after editing
*
* @param pHandler [in] the handler of the opened file
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnSaveDefectTemplateFile)(void* pHandler);

/**
* FnCloseDefectTemplateFile: Define a function type for DLL export function "CloseDefectTemplateFile"
*
* CloseDefectTemplateFile: Close the opened defect template file
*
* @param pHandler [in] the handler of the opened file
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnCloseDefectTemplateFile)(void* pHandler);

/**
* FnDoExpLineCorrect: Define a function type for DLL export function "DoExpLineCorrect"
*
* DoExpLineCorrect: Do exposure line correction after grid removing process, for FreeSync mode only
*
* @param pData [in,out] image data pointer
* @param usWidth [in] image width
* @param usHeight [in] image height
* @param usExpLine [in] exposure line number
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnDoExpLineCorrect)(unsigned short* pData, unsigned short usWidth, unsigned short usHeight, unsigned short usExpLine);

/**
* FnSyncBoxCreate: Define a function type for DLL export function "SyncBoxCreate"
*
* SyncBoxCreate: Create a memory object of syncbox
*
* @param nComPort [in] specify the serial COM port which the device connected,
*                      is Zero is specified, the function should scan all the ports to find the device
* @param pSyncboxID [in,out] buffer to receive the value of created syncbox ID
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnSyncBoxCreate)(int nComPort, int *pSyncboxID);

/**
* FnSyncBoxDestroy: Define a function type for DLL export function "SyncBoxDestroy"
*
* SyncBoxDestroy: Close the syncbox device and release the corresponding memory object
*
* @param nSyncBoxID [in] the ID of memory syncbox object
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnSyncBoxDestroy)(int nSyncBoxID);

/**
* FnSyncBoxBind: Define a function type for DLL export function "SyncBoxBind"
*
* SyncBoxBind: Bind a detector object to the syncbox.
*              Note that if the detector object destroyed, binding may be changed.
*
* @param nSyncboxID [in] specify the syncbox
* @param nDetectorID [in] specify a detector which is going to bind to the syncbox
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnSyncBoxBind)(int nSyncboxID, int nDetectorID);

/**
* FnSyncBoxGetBind: Define a function type for DLL export function "SyncBoxGetBind"
*
* SyncBoxGetBind: Get the current bound detector of syncbox
*
* @param nSyncboxID [in] specify the syncbox
* @param pDetectorID [in, out] buffer to receive the value.
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnSyncBoxGetBind)(int nSyncboxID, int *pDetectorID);

/**
* FnSyncBoxGetTubeReadyTime: Define a function type for DLL export function "SyncBoxGetTubeReadyTime"
*
* SyncBoxGetTubeReadyTime: Get the parameter value of "TubeReadyTime", syncbox will wait the specified time
*                          after hand switch phase I is pressed and before the detector clear action start
*
* @param nSyncboxID [in] specify the syncbox
* @param pTimeInMS [in,out] buffer to receive the value
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnSyncBoxGetTubeReadyTime)(int nSyncboxID, int *pTimeInMS);

/**
* FnSyncBoxSetTubeReadyTime: Define a function type for DLL export function "SyncBoxSetTubeReadyTime"
*
* SyncBoxSetTubeReadyTime: Set the parameter value of "TubeReadyTime" to device
*
* @param nSyncboxID [in] specify the syncbox
* @param nTimeInMS [in] specify the time value, in MS
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnSyncBoxSetTubeReadyTime)(int nSyncboxID, int nTimeInMS);

/**
* FnSyncBoxGetState: Define a function type for DLL export function "SyncBoxGetState"
*
* SyncBoxGetState: Get the current state of syncbox
*
* @param nSyncboxID [in] specify the syncbox
* @param pState [in, out] buffer to receive the value. 0 - Unknown, 1 - Ready, 2 - Busy 
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnSyncBoxGetState)(int nSyncboxID, int *pState);

#endif