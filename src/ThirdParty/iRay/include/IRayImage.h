/**
* File: IRayImage.h
*
* Purpose: Definit a struct for image data and information
*
*
* @author Haitao.Ning
* @version 1.0 2015/02/02
*
* Copyright (C) 2009, 2015, iRay Technology (Shanghai) Ltd.
*
*/
#ifndef _IRAY_IMAGE_H_
#define _IRAY_IMAGE_H_  

#include "IRayVariant.h"
#include "IRayEnumDef.h"

#pragma pack(1)

typedef struct _tagIRayImage
{
	int nWidth;
	int nHeight;
	int nBytesPerPixel;
	unsigned short* pData;
	IRayVariantMap propList;
}IRayImage;

#pragma pack()

#endif