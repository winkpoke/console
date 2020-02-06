#include "Detector.h"


const int CDetector::OFFSETMASK = Enm_CorrectOp_SW_PreOffset | Enm_CorrectOp_SW_PostOffset | Enm_CorrectOp_HW_PreOffset | Enm_CorrectOp_HW_PostOffset;
const int CDetector::GAINMASK = Enm_CorrectOp_SW_Gain | Enm_CorrectOp_HW_Gain;
const int CDetector::DEFECTMASK = Enm_CorrectOp_SW_Defect | Enm_CorrectOp_HW_Defect;

CDetector::CDetector()
: m_nDetectorID(0)
, m_bInitilized(false)
{
	m_WaitAckEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CDetector::~CDetector()
{
	Destroy();
	FreeIRayLibrary();
	if (m_WaitAckEvent)
	{
		CloseHandle(m_WaitAckEvent);
		m_WaitAckEvent = NULL;
	}
}

int CDetector::LoadIRayLibrary()
{
	m_hModule = LoadLibrary(TEXT("FpdSys.DLL"));
	if (NULL == m_hModule)
	{
		return Err_LoadDllFailed;
	}
	m_fpCreate = (FnCreate)GetProcAddress(m_hModule, IRAY_FPD_PROC_NAME_CREATE);
	if (NULL == m_fpCreate)
	{
		return Err_NotImplemented;
	}
	m_fpDestroy = (FnDestroy)GetProcAddress(m_hModule, IRAY_FPD_PROC_NAME_DESTROY);
	if (NULL == m_fpDestroy)
	{
		return Err_NotImplemented;
	}
	m_fpGetAttr = (FnGetAttr)GetProcAddress(m_hModule, IRAY_FPD_PROC_NAME_GETATTR);
	if (NULL == m_fpGetAttr)
	{
		return Err_NotImplemented;
	}
	m_fpSetAttr = (FnSetAttr)GetProcAddress(m_hModule, IRAY_FPD_PROC_NAME_SETATTR);
	if (NULL == m_fpSetAttr)
	{
		return Err_NotImplemented;
	}
	m_fpInvoke = (FnInvoke)GetProcAddress(m_hModule, IRAY_FPD_PROC_NAME_INVOKE);
	if (NULL == m_fpInvoke)
	{
		return Err_NotImplemented;
	}
	m_fpAbort = (FnAbort)GetProcAddress(m_hModule, IRAY_FPD_PROC_NAME_ABORT);
	if (NULL == m_fpAbort)
	{
		return Err_NotImplemented;
	}

	m_pFnGetErrorInfo = (FnGetErrInfo)GetProcAddress(m_hModule, IRAY_FPD_PROC_NAME_GET_ERROR_INFO);
	if (NULL == m_pFnGetErrorInfo)
	{
		return Err_NotImplemented;
	}
	m_bInitilized = true;
	return Err_OK;
}

void CDetector::FreeIRayLibrary()
{
	if (m_hModule)
	{
		FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
}

FPDRESULT CDetector::Create(const char* pszWorkDir, FnCallback fpCallback)
{
	if (!m_bInitilized)
	{
		if (!LoadIRayLibrary())
		{
			return Err_NotInitialized;
		}
	}

	FPDRESULT nRet = m_fpCreate(pszWorkDir, fpCallback, &m_nDetectorID);
	if (Err_OK != nRet)
	{
		return nRet;
	}

	return nRet;
}


FPDRESULT CDetector::Destroy()
{
	if (m_nDetectorID > 0)
	{
		m_fpDestroy(m_nDetectorID);
		m_nDetectorID = 0;
	}
	return Err_OK;
}

FPDRESULT CDetector::Abort()
{
	return m_fpAbort(m_nDetectorID);
}


FPDRESULT CDetector::SetAttr(int nAttrID, int nValue)
{
	if (!m_bInitilized)
		return Err_NotInitialized;

	IRayVariant var;
	var.vt = IVT_INT;
	var.val.nVal = nValue;
	FPDRESULT result = m_fpSetAttr(m_nDetectorID, nAttrID, &var);
	if (Err_OK != result)
	{
	}
	return result;
}

FPDRESULT CDetector::SetAttr(int nAttrID, float fValue)
{
	if (!m_bInitilized)
		return Err_NotInitialized;

	IRayVariant var;
	var.vt = IVT_FLT;
	var.val.fVal = fValue;
	FPDRESULT result = m_fpSetAttr(m_nDetectorID, nAttrID, &var);
	if (Err_OK != result)
	{
	}
	return result;
}
FPDRESULT CDetector::SetAttr(int nAttrID, const char* strValue)
{
	if (!m_bInitilized)
		return Err_NotInitialized;

	if (!strValue)
		return Err_InvalidParamValue;

	IRayVariant var;
	var.vt = IVT_STR;
	strcpy_s(var.val.strVal, strValue);
	FPDRESULT result = m_fpSetAttr(m_nDetectorID, nAttrID, &var);
	if (Err_OK != result)
	{
	}
	return result;
}

void CDetector::GetAttr(int nAttrID, AttrResult& result)
{
	IRayVariant var;
	FPDRESULT ret = m_fpGetAttr(m_nDetectorID, nAttrID, &var);
	if (Err_OK != ret)
	{
		memset(&result, 0, sizeof(result));
		return;
	}

	if (IVT_INT == var.vt)
	{
		result.nVal = var.val.nVal;
	}
	else if (IVT_FLT == var.vt)
	{
		result.fVal = var.val.fVal;
	}
	else if (IVT_STR == var.vt)
	{
		memcpy_s(result.strVal, IRAY_MAX_STR_LEN, var.val.strVal, IRAY_MAX_STR_LEN);
	}
}


string CDetector::GetErrorInfo(int nErrorCode)
{
	ErrorInfo info;
	m_pFnGetErrorInfo(nErrorCode, &info);
	return info.szDescription;
}


FPDRESULT CDetector::Invoke(int cmdId)
{
	if (!m_bInitilized)
		return Err_NotInitialized;

	ResetEvent(m_WaitAckEvent);
	if (Cmd_WriteUserROM == cmdId || Cmd_WriteUserRAM == cmdId)
		m_CurCmdId = Cmd_ReadUserRAM;
	else
		m_CurCmdId = cmdId;

	FPDRESULT result = m_fpInvoke(m_nDetectorID, cmdId, NULL, 0);
	if (Err_OK != result && Err_TaskPending != result)
	{
	}
	return result;
}

FPDRESULT CDetector::Invoke(int cmdId, int nValue)
{
	if (!m_bInitilized)
		return Err_NotInitialized;

	ResetEvent(m_WaitAckEvent);
	m_CurCmdId = cmdId;
	IRayCmdParam param;
	param.pt = IPT_VARIANT;
	param.var.vt = IVT_INT;
	param.var.val.nVal = nValue;
	FPDRESULT result = m_fpInvoke(m_nDetectorID, cmdId, &param, 1);
	if (Err_OK != result && Err_TaskPending != result)
	{
	}
	return result;
}

FPDRESULT CDetector::Invoke(int cmdId, int nPar1, int nPar2)
{
	if (!m_bInitilized)
		return Err_NotInitialized;

	ResetEvent(m_WaitAckEvent);
	m_CurCmdId = cmdId;
	IRayCmdParam param[2];
	param[0].pt = IPT_VARIANT;
	param[0].var.vt = IVT_INT;
	param[0].var.val.nVal = nPar1;
	param[1].pt = IPT_VARIANT;
	param[1].var.vt = IVT_INT;
	param[1].var.val.nVal = nPar2;
	FPDRESULT result = m_fpInvoke(m_nDetectorID, cmdId, param, 2);
	if (Err_OK != result && Err_TaskPending != result)
	{
	}
	return result;
}

FPDRESULT CDetector::Invoke(int cmdId, float fValue)
{
	if (!m_bInitilized)
		return Err_NotInitialized;

	ResetEvent(m_WaitAckEvent);
	m_CurCmdId = cmdId;
	IRayCmdParam param;
	param.pt = IPT_VARIANT;
	param.var.vt = IVT_FLT;
	param.var.val.fVal = fValue;
	FPDRESULT result = m_fpInvoke(m_nDetectorID, cmdId, &param, 1);
	if (Err_OK != result && Err_TaskPending != result)
	{
	}
	return result;
}

FPDRESULT CDetector::Invoke(int cmdId, const char* strValue)
{
	if (!m_bInitilized)
		return Err_NotInitialized;

	if (!strValue)
		return Err_InvalidParamValue;

	ResetEvent(m_WaitAckEvent);
	m_CurCmdId = cmdId;
	IRayCmdParam param;
	param.var.vt = IVT_STR;
	strcpy_s(param.var.val.strVal, strValue);
	FPDRESULT result = m_fpInvoke(m_nDetectorID, cmdId, &param, 1);
	if (Err_OK != result && Err_TaskPending != result)
	{
		//print("Invoke  failed! Err = %s", GetErrorInfo(result).c_str());
	}
	return result;
}

FPDRESULT CDetector::Invoke(int nCmdId, int nPara1, int nPara2, const char* strPara1, const char* strPara2)
{
	if (!m_bInitilized)
		return Err_NotInitialized;

	ResetEvent(m_WaitAckEvent);
	m_CurCmdId = nCmdId;
	IRayCmdParam param[4];
	param[0].pt = IPT_VARIANT;
	param[0].var.vt = IVT_INT;
	param[0].var.val.nVal = nPara1;

	param[1].pt = IPT_VARIANT;
	param[1].var.vt = IVT_INT;
	param[1].var.val.nVal = nPara2;

	param[2].pt = IPT_VARIANT;
	param[2].var.vt = IVT_STR;
	strcpy_s(param[2].var.val.strVal, strPara1);

	param[3].pt = IPT_VARIANT;
	param[3].var.vt = IVT_STR;
	strcpy_s(param[3].var.val.strVal, strPara2);

	FPDRESULT result = m_fpInvoke(m_nDetectorID, nCmdId, param, 4);
	if (Err_OK != result && Err_TaskPending != result)
	{
		//print("Invoke  failed! Err = %s", GetErrorInfo(result).c_str());
	}
	return result;
}

int CDetector::WaitEvent(int timeout)
{
	DWORD wait = WaitForSingleObject(m_WaitAckEvent, timeout);
	if (WAIT_TIMEOUT == wait)
		return Err_TaskTimeOut;
	else
		return m_nLastError;
}

FPDRESULT CDetector::SyncInvoke(int cmdId, int timeout)
{
	int result = Invoke(cmdId);
	if (Err_TaskPending == result)
	{
		result = WaitEvent(timeout);
	}
	return result;
}

FPDRESULT CDetector::SyncInvoke(int cmdId, int nValue, int timeout)
{
	int result = Invoke(cmdId, nValue);
	if (Err_TaskPending == result)
	{
		result = WaitEvent(timeout);
	}
	return result;
}
FPDRESULT CDetector::SyncInvoke(int cmdId, int nPar1, int nPar2, int timeout)
{
	int result = Invoke(cmdId, nPar1, nPar2);
	if (Err_TaskPending == result)
	{
		result = WaitEvent(timeout);
	}
	return result;
}
FPDRESULT CDetector::SyncInvoke(int cmdId, float fValue, int timeout)
{
	int result = Invoke(cmdId, fValue);
	if (Err_TaskPending == result)
	{
		result = WaitEvent(timeout);
	}
	return result;
}
FPDRESULT CDetector::SyncInvoke(int cmdId, const char* strValue, int timeout)
{
	int result = Invoke(cmdId, strValue);
	if (Err_TaskPending == result)
	{
		result = WaitEvent(timeout);
	}
	return result;
}

void CDetector::SDKCallback(int nDetectorID, int nEventID, int nEventLevel,
	const char* pszMsg, int nParam1, int nParam2, int nPtrParamLen, void* pParam)
{
	if ((Evt_TaskResult_Succeed == nEventID) || (Evt_TaskResult_Failed == nEventID))
	{
		if (Evt_TaskResult_Succeed == nEventID)
			m_nLastError = Err_OK;
		else
			m_nLastError = nParam2;
		if (m_CurCmdId == nParam1)
		{
			SetEvent(m_WaitAckEvent);
		}
	}
}


std::string GetWorkDirPath()
{
	char buff[128] = {0};
	FILE* pFile = NULL;
	std::string workdir("");
	fopen_s(&pFile, "resources\\iRay\\work_dir path.txt", "r");
	if (pFile)
	{
		fgets(buff, 128, pFile);
		fclose(pFile);
		workdir = std::string(buff);
	}
	else
	{
		fopen_s(&pFile, "work_dir path.txt", "r");
		if (pFile)
		{
			fgets(buff, 128, pFile);
			fclose(pFile);
			workdir = std::string(buff);
		}
	}
	return workdir.erase(workdir.find_last_not_of("\n") + 1);
}