/**
* File: IRayEnumDef.h
*
* Purpose: IRay enum definition
*
*
* @author Haitao.Ning
* @version 1.0 2015/4/23
*
* Copyright (C) 2009, 2015, iRay Technology (Shanghai) Ltd.
*
*/
#ifndef _IRAY_ENUM_DEF_H_
#define _IRAY_ENUM_DEF_H_

//*
//** Parsed content begin
//*

enum Enm_EventLevel
{
	Enm_EventLevel_Info = 0,
	Enm_EventLevel_Warn = 1,
	Enm_EventLevel_Error = 2,
	Enm_EventLevel_Notify = 3,
};

enum Enm_LogLevel
{
	Enm_LogLevel_Debug = 0,
	Enm_LogLevel_Info = 1,
	Enm_LogLevel_Warn = 2,
	Enm_LogLevel_Error = 3,
	Enm_LogLevel_Always = 4,
};

enum Enm_Authority
{
	Enm_Authority_Basic = 0x00000000,
	Enm_Authority_RawImage = 0x00000001, 	 // Bit_1
	Enm_Authority_UserDetConfig = 0x00000002, 	 // Bit_2
	Enm_Authority_Test = 0x00002000, 	 // Bit_14
	Enm_Authority_FactoryConfig = 0x00004000, 	 // Bit_15
	Enm_Authority_WriteSN = 0x00008000, 	 // Bit_16
	Enm_Authority_EMS = 0x00010000, 	 // Bit_17
	Enm_Authority_RemoveGrid = 0x00020000, 	 // Bit_18
	Enm_Authority_VirtualGrid = 0x00040000, 	 // Bit_19
	Enm_Authority_DynaNoiseReduct = 0x00080000, 	 // Bit_20
	Enm_Authority_OEM = 0x00007FFF, 	 // Bit_1..15
	Enm_Authority_Full = 0x7FFFFFFF, 	 // Bit_1..31
};

enum Enm_AuthMode
{
	Enm_AuthMode_Null = 0,
	Enm_AuthMode_PCID = 1,
	Enm_AuthMode_UserCode = 2,
	Enm_AuthMode_DetSN = 3,
};

enum Enm_Switch
{
	Enm_Off = 0,
	Enm_On = 1,
};

enum Enm_DetectorState
{
	Enm_State_Unknown = 0,
	Enm_State_Ready = 1,
	Enm_State_Busy = 2,
	Enm_State_Sleeping = 3,
};

enum Enm_Transaction
{
	Enm_Transaction_Null = 0,
	Enm_Transaction_GainGen = 1, 	 // Generating Gain calibration templage
	Enm_Transaction_DefectGen = 2, 	 // Generating Defect calibration templage
};

enum Enm_ScannedState
{
	Enm_ScannedState_NotFound = 0,
	Enm_ScannedState_Occupy = 1,
	Enm_ScannedState_Standby = 2,
};

enum Enm_ConnectionState
{
	Enm_ConnState_Unknown = 0, 	 // not initialized
	Enm_ConnState_HardwareBreak = 1, 	 // specified communication hardware can not find, or been plugged out
	Enm_ConnState_NotConnected = 2, 	 // hardware exist but not ready for communication
	Enm_ConnState_LowRate = 3, 	 // connected but in bad situation
	Enm_ConnState_OK = 4, 	 // normal connected
};

enum Enm_ImageTag
{
	Enm_ImageTag_Width = 0x0100, 	 // Tiff tag: image width in pixels
	Enm_ImageTag_Height = 0x0101, 	 // Tiff tag: image height in pixels
	Enm_ImageTag_BitsPerSample = 0x0102, 	 // Tiff tag: bits per channel (sample)
	Enm_ImageTag_Compression = 0x0103, 	 // Tiff tag: data compression technique
	Enm_ImageTag_PhotoMetric = 0x0106, 	 // Tiff tag: photometric interpretation
	Enm_ImageTag_Description = 0x010E, 	 // Tiff tag: info about image
	Enm_ImageTag_Maker = 0x010F, 	 // Tiff tag: scanner manufacturer name
	Enm_ImageTag_Model = 0x0110, 	 // Tiff tag: scanner model name/number
	Enm_ImageTag_StripOffsets = 0x0111, 	 // Tiff tag: offsets to data strips
	Enm_ImageTag_Orientation = 0x0112, 	 // Tiff tag: image orientation
	Enm_ImageTag_SamplePerPixel = 0x0115, 	 // Tiff tag: samples per pixel
	Enm_ImageTag_RowsPerStrip = 0x0116, 	 // Tiff tag: rows per strip of data
	Enm_ImageTag_StripByteCounts = 0x0117, 	 // Tiff tag: bytes counts for strips
	Enm_ImageTag_XResolution = 0x011A, 	 // Tiff tag: pixels/resolution in x
	Enm_ImageTag_YResolution = 0x011B, 	 // Tiff tag: pixels/resolution in y
	Enm_ImageTag_ResolutionUnit = 0x0128, 	 // Tiff tag: units of resolutions
	Enm_ImageTag_Software = 0x0131, 	 // Tiff tag: name & release
	Enm_ImageTag_DateTime = 0x0132, 	 // Tiff tag: creation date and time
	Enm_ImageTag_FrameNo = 0x8001, 	 // Extern tag:
	Enm_ImageTag_Temperature = 0x8002, 	 // Extern tag:
	Enm_ImageTag_RealFrequency = 0x8003, 	 // Extern tag:
	Enm_ImageTag_ExposureDelay = 0x8004, 	 // Extern tag:
	Enm_ImageTag_AcquireTime = 0x8005, 	 // Extern tag:
	Enm_ImageTag_KV = 0x8006, 	 // Extern tag:
	Enm_ImageTag_MA = 0x8007, 	 // Extern tag:
	Enm_ImageTag_MS = 0x8008, 	 // Extern tag:
	Enm_ImageTag_SID = 0x8009, 	 // Extern tag:
	Enm_ImageTag_Dose = 0x800A, 	 // Extern tag:
	Enm_ImageTag_ExposureLine = 0x800B, 	 // Extern tag:
	Enm_ImageTag_CorrectFlag = 0x800C, 	 // Extern tag:
	Enm_ImageTag_BinningMode = 0x800D, 	 // Extern tag:
	Enm_ImageTag_XRayWindow = 0x800E, 	 // Extern tag:
	Enm_ImageTag_FrameRate = 0x800F, 	 // Extern tag:
	Enm_ImageTag_BodyPartSection = 0x8010, 	 // Extern tag:
	Enm_ImageTag_BodyPart = 0x8011, 	 // Extern tag:
	Enm_ImageTag_DoseRate = 0x8012, 	 // Extern tag:
	Enm_ImageTag_AvgValue = 0x8013, 	 // Extern tag:
	Enm_ImageTag_CenterValue = 0x8014, 	 // Extern tag:
	Enm_ImageTag_ImageQualityNG = 0x8015, 	 // Extern tag: Image quality not good, caused by Long-delay-time, vibration, or invalid correction, etc.
	Enm_ImageTag_LagPositionNG = 0x8016, 	 // Extern tag: Exposure time not good for lag calibration template image
	Enm_ImageTag_iRayProductNO = 0x8017, 	 // Extern tag: iRay detector product number
	Enm_ImageTag_ExposureStatus = 0x8018, 	 // Extern tag: Exposure status, -1:default freesync mode, 0: obsolete, 2: new freesync mode, other: reserved
	Enm_ImageTag_FirmwareVer = 0xA002, 	 // Extern tag:
	Enm_ImageTag_SoftwareVer = 0xA003, 	 // Extern tag:
	Enm_ImageTag_FpdSerialNo = 0xA004, 	 // Extern tag:
	Enm_ImageTag_Filter = 0xA005, 	 // Extern tag:
	Enm_ImageTag_CheckID = 0xA006, 	 // Extern tag:
	Enm_ImageTag_PatientID = 0xA007, 	 // Extern tag:
	Enm_ImageTag_PatientName = 0xA008, 	 // Extern tag:
	Enm_ImageTag_PatientGender = 0xA009, 	 // Extern tag:
	Enm_ImageTag_PatientAge = 0xA00A, 	 // Extern tag:
	Enm_ImageTag_PatientWeight = 0xA00B, 	 // Extern tag:
	Enm_ImageTag_PatientHeight = 0xA00C, 	 // Extern tag:
	Enm_ImageTag_PatientSize = 0xA00D, 	 // Extern tag:
};

enum Enm_TriggerMode
{
	Enm_TriggerMode_Outer = 0,
	Enm_TriggerMode_Inner = 1,
	Enm_TriggerMode_Soft = 2,
	Enm_TriggerMode_Prep = 3,
	Enm_TriggerMode_Service = 4,
	Enm_TriggerMode_FreeSync = 5,
};

enum Enm_Binning
{
	Enm_Binning_Null = 0, 	 // No binning
	Enm_Binning_2x2 = 1, 	 // 2x2 binning
	Enm_Binning_3x3 = 2, 	 // 3x3 binning
	Enm_Binning_4x4 = 3, 	 // 4x4 binning
};

enum Enm_Zoom
{
	Enm_Zoom_Null = 0, 	 // No zoom
	Enm_Zoom_1024x640 = 1,
	Enm_Zoom_1024x320 = 2,
	Enm_Zoom_1024x4 = 3,
	Enm_Zoom_1024x2 = 4,
	Enm_Zoom_1024x1 = 5,
	Enm_Zoom_512x512 = 6,
	Enm_Zoom_256x256 = 7,
	Enm_Zoom_3072x3072 = 8,
	Enm_Zoom_2048x2048 = 9,
	Enm_Zoom_1536x1536 = 10,
	Enm_Zoom_1024x1024 = 11,
};

enum Enm_DynaFlag
{
	Enm_DynaFlag_Static = 0,
	Enm_DynaFlag_Dynamic = 1,
};

enum Enm_ReXferMode
{
	Enm_ReXferMode_Null = 0,
	Enm_ReXferMode_Packet = 1,
	Enm_ReXferMode_Frame = 2,
};

enum Enm_CorrectOption
{
	Enm_CorrectOp_HW_PreOffset = 0x0001,
	Enm_CorrectOp_HW_PostOffset = 0x0002,
	Enm_CorrectOp_HW_Gain = 0x0004,
	Enm_CorrectOp_HW_Defect = 0x0010,
	Enm_CorrectOp_HW_Ghost = 0x0020,
	Enm_CorrectOp_HW_Lag = 0x0040,
	Enm_CorrectOp_HW_MicroPhony = 0x0080,
	Enm_CorrectOp_SW_PreOffset = 0x00010000,
	Enm_CorrectOp_SW_PostOffset = 0x00020000,
	Enm_CorrectOp_SW_Gain = 0x00040000,
	Enm_CorrectOp_SW_Defect = 0x00100000,
	Enm_CorrectOp_SW_Ghost = 0x00200000,
	Enm_CorrectOp_SW_Lag = 0x00400000,
	Enm_CorrectOp_SW_MicroPhony = 0x00800000,
};

enum Enm_SubProductNo
{
	Enm_SubProductNo_GoS = 0,
	Enm_SubProductNo_CsI = 1,
	Enm_SubProductNo_CsI400 = 2,
	Enm_SubProductNo_CsI550 = 3,
};

enum Enm_SignalLevel
{
	Enm_SignalLevel_Low = 0,
	Enm_SignalLevel_High = 1,
};

enum Enm_FluroSync
{
	Enm_FluroSync_FreeRun = 0,
	Enm_FluroSync_SyncIn = 1,
	Enm_FluroSync_SyncOut = 2,
};

enum Enm_PGA
{
	Enm_PGA_0 = 0,
	Enm_PGA_1 = 1,
	Enm_PGA_2 = 2,
	Enm_PGA_3 = 3,
	Enm_PGA_4 = 4,
	Enm_PGA_5 = 5,
	Enm_PGA_6 = 6,
	Enm_PGA_7 = 7,
	Enm_PGA_8 = 8,
};

enum Enm_HWOffsetType
{
	Enm_OffsetType_Null = 0,
	Enm_OffsetType_Post = 1,
	Enm_OffsetType_Pre = 2,
};

enum Enm_ExpMode
{
	Enm_ExpMode_Null = 0x00,
	Enm_ExpMode_Manual = 0x01,
	Enm_ExpMode_AEC = 0x02,
	Enm_ExpMode_Pulse = 0x80,
	Enm_ExpMode_Continous = 0x81,
	Enm_ExpMode_Linewise = 0x82,
};

enum Enm_PrepCapMode
{
	Enm_PrepCapMode_ClearAcq = 0,
	Enm_PrepCapMode_Acq2 = 1,
};

enum Enm_OutModeCapTrig
{
	Enm_OutModeCapTrig_X_ON = 0,
	Enm_OutModeCapTrig_Prep = 1,
};

enum Enm_ImgChType
{
	Enm_ImgCh_Ethernet = 0,
	Enm_ImgCh_Cameralink = 1,
	Enm_ImgCh_GigeVision = 2,
};

enum Enm_EthernetProtocol
{
	Enm_Ethernet_UDP = 0,
	Enm_Ethernet_TCP = 1,
};

enum Enm_FW_DeviceType
{
	Enm_FW_DeviceType_MainFPGA = 0x01,
	Enm_FW_DeviceType_ReadFPGA1 = 0x02,
	Enm_FW_DeviceType_ReadFPGA2 = 0x03,
	Enm_FW_DeviceType_ControlBox = 0x04,
	Enm_FW_DeviceType_MCU1 = 0x10,
	Enm_FW_DeviceType_AllInOne = 0xF0,
	Enm_FW_DeviceType_None = 0xFF,
};

enum Enm_CaliDataState
{
	Enm_CaliDataState_NoData = 0,
	Enm_CaliDataState_Valid = 1,
	Enm_CaliDataState_ValidWarn = 2,
	Enm_CaliDataState_OutOfDate = 3,
	Enm_CaliDataState_ParamMisMatch = 4,
};

enum Enm_NetworkInterface
{
	Enm_NetworkInterface_Unknown = 0,
	Enm_NetworkInterface_Cable = 1,
	Enm_NetworkInterface_Wifi = 2,
};

enum Enm_FileTypes
{
	Enm_File_Offset = 0x01,
	Enm_File_Gain = 0x02,
	Enm_File_Defect = 0x04,
	Enm_File_Lag = 0x05,
	Enm_File_ArmLog = 0xA0,
	Enm_File_Firmware = 0xA1,
};

enum Enm_Wifi_CountryCode
{
	Enm_Wifi_Country_CN = 0,
	Enm_Wifi_Country_DE = 1,
	Enm_Wifi_Country_FR = 2,
	Enm_Wifi_Country_GB = 3,
	Enm_Wifi_Country_HK = 4,
	Enm_Wifi_Country_IT = 5,
	Enm_Wifi_Country_KR = 6,
	Enm_Wifi_Country_NL = 7,
	Enm_Wifi_Country_RU = 8,
	Enm_Wifi_Country_US = 9,
};

enum Enm_Wifi_Frequency
{
	Enm_Wifi_Freq_2GHz = 0,
	Enm_Wifi_Freq_5GHz = 1,
};

enum Enm_Wifi_BandWidth
{
	Enm_Wifi_Band_HT20 = 0,
	Enm_Wifi_Band_HT40_Plus = 1,
	Enm_Wifi_Band_HT40_Minus = 2,
};

enum Enm_Wifi_Channel
{
	Enm_Wifi_Chnl_1 = 1,
	Enm_Wifi_Chnl_2 = 2,
	Enm_Wifi_Chnl_3 = 3,
	Enm_Wifi_Chnl_4 = 4,
	Enm_Wifi_Chnl_5 = 5,
	Enm_Wifi_Chnl_6 = 6,
	Enm_Wifi_Chnl_7 = 7,
	Enm_Wifi_Chnl_8 = 8,
	Enm_Wifi_Chnl_9 = 9,
	Enm_Wifi_Chnl_10 = 10,
	Enm_Wifi_Chnl_11 = 11,
	Enm_Wifi_Chnl_12 = 12,
	Enm_Wifi_Chnl_13 = 13,
	Enm_Wifi_Chnl_36 = 36,
	Enm_Wifi_Chnl_40 = 40,
	Enm_Wifi_Chnl_44 = 44,
	Enm_Wifi_Chnl_48 = 48,
	Enm_Wifi_Chnl_149 = 149,
	Enm_Wifi_Chnl_153 = 153,
	Enm_Wifi_Chnl_157 = 157,
	Enm_Wifi_Chnl_161 = 161,
	Enm_Wifi_Chnl_165 = 165,
};

enum Enm_Wifi_Security
{
	Enm_Wifi_WPA_WPA2_PSK = 0,
	Enm_Wifi_WPA_PSK = 1,
	Enm_Wifi_WPA2_PSK = 2,
};

enum Enm_Battery_Warn
{
	Enm_Battery_Normal = 0,
	Enm_Battery_LowPower = 1,
	Enm_Battery_PowerOff = 2,
};

enum Enm_FreesyncSubFlow
{
	Enm_FreesyncSubFlow_Reserved = 0,
	Enm_FreesyncSubFlow_NFNR = 1, 	 // ISync plus: Single ExpLine
	Enm_FreesyncSubFlow_FFNR = 2,
	Enm_FreesyncSubFlow_3 = 3, 	 // FSync Dual Gate: Multi ExpLine (distinguish odd/even column)
	Enm_FreesyncSubFlow_4 = 4, 	 // FSync Single Gate: Multi ExpLine (whole line)
};

enum Enm_SyncWorkState
{
	Enm_SyncWorkState_Unknown = 0,
	Enm_SyncWorkState_Ready = 1,
	Enm_SyncWorkState_Busy = 2,
};

enum Enm_FullWell
{
	Enm_FullWell_0 = 0,
	Enm_FullWell_1 = 1,
};

enum Enm_InnerSubFlow
{
	Enm_InnerSubFlow_ClearAcq = 0, 	 // Normal mode
	Enm_InnerSubFlow_CycleAcq = 1, 	 // Cycle self acquisition as freesync implementation
};

enum Enm_SoftwareSubFlow
{
	Enm_SoftwareSubFlow_Normal = 0, 	 // Normal mode
	Enm_SoftwareSubFlow_OverlieAcq = 1, 	 // Overlie the pixel value of cycle acquisition for long time exposure
};

enum Enm_ProdType
{
	Enm_Prd_Venu1717F = 0x0001, 	 // Venu1717F
	Enm_Prd_Mercu0909F = 0x0006, 	 // Mercu0909F
	Enm_Prd_Mammo1012F = 0x000B, 	 // Mammo1012F
	Enm_Prd_NDT0505F = 0x000C, 	 // NDT0505F
	Enm_Prd_Venu1417W = 0x0016, 	 // Venu1417W
	Enm_Prd_Senu1417P = 0x001C, 	 // Senu1417P
	Enm_Prd_Venu1717M2 = 0x001D, 	 // Venu1717M2
	Enm_Prd_Penu1417P = 0x0019, 	 // Penu1417P
	Enm_Prd_Mars1417V = 0x0020, 	 // Mars1417V1
	Enm_Prd_Penu1417P_plus = 0x0021, 	 // Penu1417P_plus
	Enm_Prd_Mars1717V = 0x0025, 	 // Mars1717V1
	Enm_Prd_Venu1717MF = 0x0026, 	 // Venu1717MF
	Enm_Prd_Sars1417 = 0x0027, 	 // Sars1417
	Enm_Prd_Mars1417X = 0x002A, 	 // Mars1417X
	Enm_Prd_Mars1717XU = 0x002D, 	 // Mars1717XU
	Enm_Prd_Pluto0406X = 0x002E, 	 // Pluto0406X
	Enm_Prd_Mars1012X = 0x0029, 	 // Mars1012X
	Enm_Prd_Mercu1717V = 0x0030, 	 // Mercu1717V
	Enm_Prd_Mercu1616TE = 0x0031, 	 // Mercu1616TE
	Enm_Prd_Mercu1616VE = 0x0032, 	 // Mercu1616VE
	Enm_Prd_Mars1717XF = 0x0033, 	 // Mars1717XF
	Enm_Prd_Mars1417XF = 0x0034, 	 // Mars1417XF
	Enm_Prd_Mercu0909FN = 0x0038, 	 // Mercu0909FN
	Enm_Prd_Venu1717MN = 0x003A, 	 // Venu1717MN
	Enm_Prd_Mars1417V2 = 0x003B, 	 // Mars1417V2
	Enm_Prd_Venu1012X = 0x003C, 	 // Venu1012X
	Enm_Prd_Mars1012VN = 0x003D, 	 // Mars1012VN
	Enm_Prd_Mars1717V2 = 0x003E, 	 // Mars1717V2
	Enm_Prd_Eart0505XN = 0x0044, 	 // Eart0505XN
	Enm_Prd_Venu1717X = 0x0048, 	 // Venu1717X
	Enm_Prd_Eart0505X = 0x0049, 	 // Eart0505X
	Enm_Prd_Venu1717MX = 0x0050, 	 // Venu1717MX
	Enm_Prd_Venu1012VD = 0x0058, 	 // Venu1012VD
};

//*
//** Parsed content end
//*

#endif
