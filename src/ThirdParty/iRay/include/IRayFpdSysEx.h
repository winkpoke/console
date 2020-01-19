/**
* File: FpdSysEx.h
*
* Purpose: Main access interface of iRay flat panel not public.
*
* @author Haitao.Ning
* @version 1.0 2015/02/02
*
* Copyright (C) 2009, 2015, iRay Technology (Shanghai) Ltd.
*
*/
#ifndef _IRAY_FPD_SYSTEM_INTERFACE_EX_H_
#define _IRAY_FPD_SYSTEM_INTERFACE_EX_H_  

#include "IRayFpdSys.h"

#define IRAY_FPD_PROC_NAME_GET_AUTHORITY         "GetAuthority"
#define IRAY_FPD_PROC_NAME_GET_PRODUCT_COUNT     "GetProdCount"
#define IRAY_FPD_PROC_NAME_GET_PRODUCT_LIST      "GetProdList"
#define IRAY_FPD_PROC_NAME_GET_ATTRS_COUNT       "GetAttrsCount"
#define IRAY_FPD_PROC_NAME_GET_ATTR_IDLIST       "GetAttrIDList"
#define IRAY_FPD_PROC_NAME_GET_ATTR_INFO         "GetAttrInfo"
#define IRAY_FPD_PROC_NAME_GET_ENUM_ITEMS_COUNT  "GetEnumItemsCount"
#define IRAY_FPD_PROC_NAME_GET_ENUM_ITEM_LIST    "GetEnumItemList"
#define IRAY_FPD_PROC_NAME_GET_CMD_COUNT         "GetCommandCount"
#define IRAY_FPD_PROC_NAME_GET_CMD_LIST          "GetCommandInfoList"
#define IRAY_FPD_PROC_NAME_GET_CMD_PARAM_COUNT   "GetCmdParamCount"
#define IRAY_FPD_PROC_NAME_GET_CMD_PARAM_INFO    "GetCmdParamInfo"
#define IRAY_FPD_PROC_NAME_GET_ERROR_INFO        "GetErrInfo"

#define IRAY_MAX_NAME_LEN 128 
#define IRAY_MAX_DESC_LEN 256 

#pragma pack(1)

typedef struct _tagProdInfo
{
	int nProdNo;
	char szName[IRAY_MAX_NAME_LEN];
	char szDescription[IRAY_MAX_DESC_LEN];
}ProdInfo;

enum PARAM_VALIDATOR 
{
	Enm_ParamValidator_Null = 0,
	Enm_ParamValidator_MinMax = 1,
	Enm_ParamValidator_Enum = 2,
	Enm_ParamValidator_FilePath = 3,
	Enm_ParamValidator_IP = 4,
	Enm_ParamValidator_MAC = 5,
	Enm_ParamValidator_FpdSN = 6,
};

typedef struct _tagAttrInfo
{
	int nAttrID;
	IRAY_VAR_TYPE nDataType; // DataType
	int bIsConfigItem;       // 0: not a config item, 1: is a config item
	int bIsWritable;         // 0: not writable, 1: is writable
	int bIsEnum;             // 0: not a enum, 1: is a enum
	int	nPrecision;          // 0: integer, 1: 0.1, 2: 0.01, 3: 0.001...
	float fMinValue;
	float fMaxValue;
	char szPath[IRAY_MAX_NAME_LEN];        // Group information
	char szName[IRAY_MAX_NAME_LEN];
	char szDisplayName[IRAY_MAX_NAME_LEN];
	char szUnit[IRAY_MAX_NAME_LEN];
	char szDescription[IRAY_MAX_DESC_LEN];
	char szEnumTypeName[IRAY_MAX_NAME_LEN];
	PARAM_VALIDATOR eValidator;
}AttrInfo;

typedef struct _tagEnumItem
{
	int nVal;
	char szName[IRAY_MAX_NAME_LEN];
	char szDescription[IRAY_MAX_DESC_LEN];
}EnumItem;

typedef struct _tagCmdItem
{
	int nCmdID;
	char szName[IRAY_MAX_NAME_LEN];
}CmdInfo;

typedef struct _tagCmdParamInfo
{
	IRAY_VAR_TYPE nDataType; // DataType
	int bIsDataBlock;        // 0: not a data block, 1: is a data block
	int bIsEnum;             // 0: not a enum, 1: is a enum
	float fMinValue;
	float fMaxValue;
	char szName[IRAY_MAX_NAME_LEN];
	char szEnumTypeName[IRAY_MAX_DESC_LEN];
	PARAM_VALIDATOR eValidator;
}CmdParamInfo;

typedef struct _tagErrorInfo
{
	int nErrorCode;
	char szDescription[IRAY_MAX_DESC_LEN];
	char szSolution[IRAY_MAX_DESC_LEN];
}ErrorInfo;

#pragma pack()


/**
* FnGetAuthority: Define a function type for DLL export function "GetAuthority"
*
* GetAuthority: Get access level 
*
* @param pAuthority [out]  0x00000000: basic access, 0x11111111: full factory access
*
* @return 0: succeed, Non-Zero: error code
*
* @see Enm_Authority
*/
typedef int(*FnGetAuthority)(int *pAuthority);

/**
* FnGetProdCount: Define a function type for DLL export function "GetProdCount"
*
* GetProdCount: Query the count of iRay Detector Products
*
* @param pCount [out] buffer to receive the result
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnGetProdCount)(int* pCount);

/**
* FnGetProdList: Define a function type for DLL export function "GetProdList"
*
* GetProdList: Query the product list,
*              please call GetProdCount previously to get the size for result receiving buffer.
*
* @param pProdList [out] buffer to receive the result
* @param nCount [in] size of the buffer, must equals the result of "GetProdCount"
*
* @return 0: succeed, Non-Zero: error code
*
* @see GetProdCount
*/
typedef int(*FnGetProdList)(ProdInfo pProdList[], int nCount);

/**
* FnGetErrInfo: Define a function type for DLL export function "GetErrInfo"
*
* GetErrInfo: Query information of a certain error by error code
*
* @param nErrorCode [in] specify the error code
* @param pInfo [out] buffer to receive the result
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnGetErrInfo)(int nErrorCode, ErrorInfo* pInfo);

/**
* FnGetEnumItemsCount: Define a function type for DLL export function "GetEnumItemsCount"
*
* GetEnumItemsCount: Query the element count of a certain Enum type,
*                    usually this function is used while attribute value displaying if the value is an Enum.
*
* @param pszEnumTypeName [in] specify the type name of the Enum
* @param pCount [out] buffer to receive the result
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnGetEnumItemsCount)(char* pszEnumTypeName, int* pCount);

/**
* FnGetEnumItemList: Define a function type for DLL export function "GetEnumItemList"
*
* GetEnumItemList: Query the element list of a certain Enum type,
*                  please call GetEnumItemsCount previously to get the size for result receiving buffer.
*
* @param pszEnumTypeName [in] specify the type name of the Enum
* @param pItemList [out] buffer to receive the result, each element of the array is an EnumItem struct
* @param nCount [in] size of the buffer, must equals the result of "GetEnumItemsCount"
*
* @return 0: succeed, Non-Zero: error code
*
* @see GetEnumItemsCount
*/
typedef int(*FnGetEnumItemList)(char* pszEnumTypeName, EnumItem pItemList[], int nCount);

/**
* FnGetAttrsCount: Define a function type for DLL export function "GetAttrsCount"
*
* GetAttrsCount: Query the count of all public attributes
*
* @param nDetectorID [in] specify a detector
* @param pCount [out] buffer to receive the result
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnGetAttrsCount)(int nDetectorID, int* pCount);

/**
* FnGetAttrIDList: Define a function type for DLL export function "GetAttrIDList"
*
* GetAttrIDList: Query public attribute list,
*                please call GetAttrsCount previously to get the size for result receiving buffer.
*
* @param nDetectorID [in] specify a detector
* @param pnIDList [out] buffer to receive the result, each element of the array is an attribute ID
* @param nCount [in] size of the buffer, must equals the result of "GetAttrsCount"
*
* @return 0: succeed, Non-Zero: error code
*
* @see GetAttrsCount
*/
typedef int(*FnGetAttrIDList)(int nDetectorID, int pnIDList[], int nCount);

/**
* FnGetAttrInfo: Define a function type for DLL export function "GetAttrInfo"
*
* GetAttrInfo: Query the information of a certain attribute,
*              the information include data type, min/max value, display name..., even enum name for UI displaying.
*
* @param nDetectorID [in] specify a detector
* @param nAttrID [in] specify an attribute
* @param pInfo [out] buffer to receive the result
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnGetAttrInfo)(int nDetectorID, int nAttrID, AttrInfo* pInfo);

/**
* FnGetCommandCount: Define a function type for DLL export function "GetCommandCount"
*
* GetCommandCount: Query the count of SDK supported Commands
*
* @param nDetectorID [in] specify a detector
* @param pCount [out] buffer to receive the result
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnGetCommandCount)(int nDetectorID, int* pCount);

/**
* FnGetCommandInfoList: Define a function type for DLL export function "GetCommandInfoList"
*
* GetCommandInfoList: Query the Command list,
*                 please call GetCommandCount perviously to get the size for result receiving buffer.
*
* @param nDetectorID [in] specify a detector
* @param pCmdList [out] buffer to receive the result
* @param nCount [in] size of the buffer, must equals the result of "GetCommandCount"
*
* @return 0: succeed, Non-Zero: error code
*
* @see GetCommandCount
*/
typedef int(*FnGetCommandInfoList)(int nDetectorID, CmdInfo pCmdList[], int nCount);

/**
* FnGetCmdParamCount: Define a function type for DLL export function "GetCmdParamCount"
*
* GetCmdParamCount: Query the parameter count of a certain Command
*
* @param nDetectorID [in] specify a detector
* @param nCmdID [in] specify the Command
* @param pCount [out] buffer to receive the result
*
* @return 0: succeed, Non-Zero: error code
*/
typedef int(*FnGetCmdParamCount)(int nDetectorID, int nCmdID, int* pCount);

/**
* FnGetEnumItemList: Define a function type for DLL export function "GetCmdParamInfo"
*
* GetCmdParamInfo: Query the parameter infomation list for a certain Command,
*                  please call GetCmdParamCount previously to get the size for result receiving buffer.
*
* @param nDetectorID [in] specify a detector
* @param nCmdID [in] specify the Command
* @param pInfoList [out] buffer to receive the result, each element of the array is an CmdParamInfo struct
* @param nCount [in] size of the buffer, must equals the result of "GetCmdParamCount"
*
* @return 0: succeed, Non-Zero: error code
*
* @see GetCmdParamCount
*/
typedef int(*FnGetCmdParamInfo)(int nDetectorID, int nCmdID, CmdParamInfo pInfoList[], int nCount);

#endif