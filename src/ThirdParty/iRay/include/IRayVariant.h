/**
* File: IRayVariant.h
*
* Purpose: Definit data structs for common data type wrapper
*
*
* @author Haitao.Ning
* @version 1.0 2015/02/02
*
* Copyright (C) 2009, 2015, iRay Technology (Shanghai) Ltd.
*
*/
#ifndef _IRAY_VARIANT_H_
#define _IRAY_VARIANT_H_  

#define IRAY_MAX_STR_LEN 512 

#pragma pack(1)

enum IRAY_VAR_TYPE
{
	IVT_INT = 0,
	IVT_FLT = 1,
	IVT_STR = 2,
};

typedef struct _tagIRayVariant
{
	IRAY_VAR_TYPE vt;
	struct
	{
		int   nVal;
		float fVal;
		char  strVal[IRAY_MAX_STR_LEN];
	}val;
}IRayVariant;

enum IRAY_PARAM_TYPE
{
	IPT_VARIANT = 0,
	IPT_BLOCK = 100
};

typedef struct _tagIRayDataBlock
{
	unsigned uBytes;
	void* pData;
}IRayDataBlock;

typedef struct _tagIRayCmdParam
{
	IRAY_PARAM_TYPE pt;
	IRayVariant var;
	IRayDataBlock blc;
}IRayCmdParam;

typedef struct _tagIRayVariantMapItem
{
	int nMapKey;
	IRayVariant varMapVal;
}IRayVariantMapItem;

typedef struct _tagIRayVariantMap
{
	int nItemCount;
	IRayVariantMapItem* pItems;
}IRayVariantMap;

#pragma pack()

#endif