#ifndef _DETECTOR_H
#define _DETECTOR_H

#include "IRayFpdSys.h"
#include "IRayFpdSysEx.h"
#include <Windows.h>
#include <vector>
#include <string>
using namespace::std;

#define TRACE printf

union AttrResult
{
	int   nVal;
	float fVal;
	char  strVal[IRAY_MAX_STR_LEN];
};

class CDetectorManager;
class iEventReceiver;
class CDetector
{
public:
	CDetector();
	~CDetector();
	int  LoadIRayLibrary();
	void FreeIRayLibrary();

	FPDRESULT Create(const char* pszWorkDir, FnCallback fpCallback);
	FPDRESULT Destroy();
	FPDRESULT SetAttr(int nAttrID, int nValue);
	FPDRESULT SetAttr(int nAttrID, float fValue);
	FPDRESULT SetAttr(int nAttrID, const char* strValue);
	void      GetAttr(int nAttrID, AttrResult& result);
	FPDRESULT Invoke(int cmdId);
	FPDRESULT Invoke(int cmdId, int nValue);
	FPDRESULT Invoke(int cmdId, int nPar1, int nPar2);
	FPDRESULT Invoke(int cmdId, float fValue);
	FPDRESULT Invoke(int cmdId, const char* strValue);
	FPDRESULT Invoke(int nCmdId, int nPara1, int nPara2, const char* strPara1, const char* strPara2);
	FPDRESULT SyncInvoke(int cmdId, int timeout);
	FPDRESULT SyncInvoke(int cmdId, int nValue, int timeout);
	FPDRESULT SyncInvoke(int cmdId, int nPar1, int nPar2, int timeout);
	FPDRESULT SyncInvoke(int cmdId, float fValue, int timeout);
	FPDRESULT SyncInvoke(int cmdId, const char* strValue, int timeout);
	FPDRESULT Abort();
	string    GetErrorInfo(int nErrorCode);
	int       DetectorID(){ return m_nDetectorID; }
	void      SDKCallback(int nDetectorID, int nEventID, int nEventLevel,
						  const char* pszMsg, int nParam1, int nParam2, int nPtrParamLen, void* pParam);
	int WaitEvent(int timeout);

	static const int OFFSETMASK;
	static const int GAINMASK;
	static const int DEFECTMASK;
private:
	bool m_bInitilized;
	HMODULE m_hModule;
	FnCreate  m_fpCreate;
	FnDestroy m_fpDestroy;
	FnGetAttr m_fpGetAttr;
	FnSetAttr m_fpSetAttr;
	FnInvoke  m_fpInvoke;
	FnAbort   m_fpAbort;
	FnGetErrInfo m_pFnGetErrorInfo;
	int m_nDetectorID;
	HANDLE m_WaitAckEvent;
	int m_CurCmdId;
	int m_nLastError;
};

std::string GetWorkDirPath();

#endif