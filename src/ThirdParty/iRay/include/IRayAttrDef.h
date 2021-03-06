/**
* File: IRayAttrDef.h
*
* Purpose: IRay attribute definition
*
*
* @author Haitao.Ning
* @version 1.0 2015/4/23
*
* Copyright (C) 2009, 2015, iRay Technology (Shanghai) Ltd.
*
*/
#ifndef _IRAY_ATTRIBUTE_DEF_H_
#define _IRAY_ATTRIBUTE_DEF_H_

const int Cfg_ProtocolEdition = 1;
const int Cfg_ProductNo = 2;
const int Cfg_SN = 3;
const int Cfg_UseServiceProcess = 8;
const int Cfg_DetectorImp = 4;
const int Cfg_ConnImp = 5;
const int Cfg_CaliImp = 6;
const int Cfg_LogLevel = 7;
const int Cfg_HostIP = 101;
const int Cfg_HostPort = 102;
const int Cfg_RemoteIP = 103;
const int Cfg_RemotePort = 104;
const int Cfg_ComPort = 105;
const int Cfg_PleoraConnStr = 106;
const int Cfg_PleoraPacketSize = 107;
const int Cfg_WinpcapConnStr = 108;
const int Cfg_PleoraMaxFps = 109;
const int Cfg_RepeatCmdEnable = 150;
const int Cfg_FTP_Download_HostIP = 201;
const int Cfg_FTP_Download_HostPort = 202;
const int Cfg_FTP_Download_User = 203;
const int Cfg_FTP_Download_PWD = 204;
const int Cfg_FTP_Download_LocalPath = 205;
const int Cfg_FTP_Upload_HostIP = 206;
const int Cfg_FTP_Upload_HostPort = 207;
const int Cfg_FTP_Upload_User = 208;
const int Cfg_FTP_Upload_PWD = 209;
const int Cfg_FTP_Upload_LocalPath = 210;
const int Cfg_OffsetAlarmMinute = 301;
const int Cfg_GainAlarmTime = 302;
const int Cfg_DefectAlarmTime = 303;
const int Cfg_CaliValidity_PreWarnMinute = 304;
const int Cfg_CaliValidity_Enable = 305;
const int Cfg_DefaultSubset = 306;
const int Cfg_DefaultCorrectOption = 307;
const int Cfg_DefectStatistical_DummyTop = 308;
const int Cfg_DefectStatistical_DummyBottom = 309;
const int Cfg_DefectStatistical_DummyLeft = 310;
const int Cfg_DefectStatistical_DummyRight = 311;
const int Cfg_ClearAcqParam_DelayTime = 501;
const int Cfg_FpsCheck_Enable = 502;
const int Cfg_FpsCheck_Tolerance = 503;
const int Cfg_FWUpdTimeOut = 504;
const int Cfg_OfflineInspectTimeout = 505;
const int Cfg_AllowReconnectByOnlineNotice = 506;
const int Cfg_ResetTimeout = 507;
const int Cfg_PreviewImage_Enable = 508;
const int Cfg_PushImageAtExpTimeout_Enable = 509;
const int Cfg_RetransferCount = 510;
const int Cfg_ConnRecoverTimeout = 511;
const int Cfg_TemperatureHighThreshold = 512;
const int Cfg_AllowMismatchSN = 513;
const int Cfg_ImagePacketGapTimeout = 514;
const int Cfg_FwAllowedDefectPoints = 515;
const int Cfg_PostOffsetStart_DelayTime = 516;
const int Cfg_TotalAcqTimeout = 517;
const int Cfg_PreExpImageAcqTimeout = 518;
const int Cfg_CleanupProcessTime = 519;
const int Cfg_SeqAcq_AutoStopToSyncExp = 520;
const int Cfg_Acq2SubFlow = 521;
const int Attr_Prod_Name = 1001;
const int Attr_Prod_Description = 1002;
const int Attr_Prod_FullWidth = 1003;
const int Attr_Prod_FullHeight = 1004;
const int Attr_Prod_PhysicalPixelSize = 1005;
const int Attr_Prod_BitDepth = 1006;
const int Attr_Prod_DataBytesPerPacket = 1007;
const int Attr_Prod_TotalPacketNumber = 1008;
const int Attr_Prod_DummyTop = 1009;
const int Attr_Prod_DummyBottom = 1010;
const int Attr_Prod_DummyLeft = 1011;
const int Attr_Prod_DummyRight = 1012;
const int Attr_Prod_AfeChSize = 1014;
const int Attr_Prod_GateSize = 1016;
const int Attr_Prod_GateEdge = 1017;
const int Attr_Prod_DriveMode = 1013;
const int Attr_Prod_ReXferMode = 1015;
const int Attr_UROM_ProductNo = 2001;
const int Attr_UROM_MainVersion = 2002;
const int Attr_UROM_ReadVersion = 2003;
const int Attr_UROM_McuVersion = 2004;
const int Attr_UROM_ArmVersion = 2005;
const int Attr_UROM_KernelVersion = 2006;
const int Attr_UROM_ProtocolVersion = 2007;
const int Attr_UROM_MasterBuildTime = 2008;
const int Attr_UROM_SlaveBuildTime = 2009;
const int Attr_UROM_McuBuildTime = 2010;
const int Attr_UROM_RowPreDelayTime = 2011;
const int Attr_UROM_RowPostDelayTime = 2012;
const int Attr_UROM_IntegrateTime = 2013;
const int Attr_UROM_ZoomMode = 2014;
const int Attr_UROM_ExpEnable_SignalLevel = 2015;
const int Attr_UROM_SelfClearEnable = 2016;
const int Attr_UROM_SelfClearSpanTime = 2017;
const int Attr_UROM_SequenceIntervalTime = 2018;
const int Attr_UROM_TriggerMode = 2019;
const int Attr_UROM_DynamicFlag = 2020;
const int Attr_UROM_TubeReadyTime = 2021;
const int Attr_UROM_SequenceIntervalTime_HighPrecision = 2022;
const int Attr_UROM_SetDelayTime = 2023;
const int Attr_UROM_ExpWindowTime = 2025;
const int Attr_UROM_SyncExpTime = 2027;
const int Attr_UROM_SyncExpTime_HighPrecision = 2028;
const int Attr_UROM_VT = 2029;
const int Attr_UROM_PGA = 2030;
const int Attr_UROM_PrepCapMode = 2032;
const int Attr_UROM_SelfCapEnable = 2033;
const int Attr_UROM_FluroSync = 2034;
const int Attr_UROM_SrcPort = 2035;
const int Attr_UROM_SrcIP = 2036;
const int Attr_UROM_SrcMAC = 2037;
const int Attr_UROM_DestPort = 2038;
const int Attr_UROM_DestIP = 2039;
const int Attr_UROM_DestMAC = 2040;
const int Attr_UROM_SyncboxIP = 2041;
const int Attr_UROM_PreviewImgMode = 2044;
const int Attr_UROM_HWOffsetType = 2045;
const int Attr_UROM_AcquireDelayTime = 2046;
const int Attr_UROM_BinningMode = 2047;
const int Attr_UROM_ExpMode = 2050;
const int Attr_UROM_AecMainTime = 2051;
const int Attr_UROM_DynaOffsetGapTime = 2052;
const int Attr_UROM_DynaOffsetEnable = 2053;
const int Attr_UROM_ImagePktGapTime = 2054;
const int Attr_UROM_OutModeCapTrig = 2069;
const int Attr_UROM_HvgPrepOn = 2055;
const int Attr_UROM_HvgXRayEnable = 2056;
const int Attr_UROM_HvgXRayOn = 2057;
const int Attr_UROM_HvgXRaySyncOut = 2058;
const int Attr_UROM_HvgXRaySyncIn = 2059;
const int Attr_UROM_CbxBuildTime = 2060;
const int Attr_UROM_SubProductNo = 2061;
const int Attr_UROM_SerialNo = 2062;
const int Attr_UROM_ImageChType = 2063;
const int Attr_UROM_ImageChProtocol = 2064;
const int Attr_UROM_HWGainEnable = 2065;
const int Attr_UROM_ExpTimeValidPercent = 2066;
const int Attr_UROM_FreesyncCenterThreshold = 2067;
const int Attr_UROM_FreesyncEdgeThreshold = 2068;
const int Attr_UROM_FreesyncSubFlow = 2070;
const int Attr_UROM_PowSeriesCorrectEnable = 2071;
const int Attr_UROM_PulseClearTimes = 2072;
const int Attr_UROM_ROIColStartPos = 2073;
const int Attr_UROM_ROIColEndPos = 2074;
const int Attr_UROM_ROIRowStartPos = 2075;
const int Attr_UROM_ROIRowEndPos = 2076;
const int Attr_UROM_FullWell = 2077;
const int Attr_UROM_InnerSubFlow = 2078;
const int Attr_UROM_SoftwareSubFlow = 2079;
const int Attr_UROM_IntegrateTime_W = 2540;
const int Attr_UROM_ZoomMode_W = 2501;
const int Attr_UROM_ExpEnable_SignalLevel_W = 2502;
const int Attr_UROM_SelfClearEnable_W = 2503;
const int Attr_UROM_SelfClearSpanTime_W = 2504;
const int Attr_UROM_SequenceIntervalTime_W = 2505;
const int Attr_UROM_TriggerMode_W = 2506;
const int Attr_UROM_DynamicFlag_W = 2507;
const int Attr_UROM_TubeReadyTime_W = 2508;
const int Attr_UROM_SetDelayTime_W = 2510;
const int Attr_UROM_SequenceIntervalTime_HighPrecision_W = 2511;
const int Attr_UROM_ExpWindowTime_W = 2512;
const int Attr_UROM_PGA_W = 2513;
const int Attr_UROM_PrepCapMode_W = 2514;
const int Attr_UROM_SelfCapEnable_W = 2515;
const int Attr_UROM_FluroSync_W = 2516;
const int Attr_UROM_SrcIP_W = 2518;
const int Attr_UROM_SrcMAC_W = 2519;
const int Attr_UROM_DestPort_W = 2520;
const int Attr_UROM_DestIP_W = 2521;
const int Attr_UROM_DestMAC_W = 2522;
const int Attr_UROM_PreviewImgMode_W = 2523;
const int Attr_UROM_HWOffsetType_W = 2544;
const int Attr_UROM_SyncboxIP_W = 2543;
const int Attr_UROM_AcquireDelayTime_W = 2524;
const int Attr_UROM_BinningMode_W = 2525;
const int Attr_UROM_ExpMode_W = 2528;
const int Attr_UROM_AecMainTime_W = 2529;
const int Attr_UROM_DynaOffsetGapTime_W = 2530;
const int Attr_UROM_DynaOffsetEnable_W = 2531;
const int Attr_UROM_ImagePktGapTime_W = 2542;
const int Attr_UROM_OutModeCapTrig_W = 2541;
const int Attr_UROM_HvgPrepOn_W = 2532;
const int Attr_UROM_HvgXRayEnable_W = 2533;
const int Attr_UROM_HvgXRayOn_W = 2534;
const int Attr_UROM_HvgXRaySyncOut_W = 2535;
const int Attr_UROM_HvgXRaySyncIn_W = 2536;
const int Attr_UROM_ExpTimeValidPercent_W = 2537;
const int Attr_UROM_FreesyncCenterThreshold_W = 2538;
const int Attr_UROM_FreesyncEdgeThreshold_W = 2539;
const int Attr_UROM_PowSeriesCorrectEnable_W = 2545;
const int Attr_UROM_ROIColStartPos_W = 2546;
const int Attr_UROM_ROIColEndPos_W = 2547;
const int Attr_UROM_ROIRowStartPos_W = 2548;
const int Attr_UROM_ROIRowEndPos_W = 2549;
const int Attr_UROM_FullWell_W = 2550;
const int Attr_UROM_PulseClearTimes_W = 2551;
const int Attr_UROM_InnerSubFlow_W = 2552;
const int Attr_UROM_SoftwareSubFlow_W = 2553;
const int Attr_FROM_ProductNo = 3001;
const int Attr_FROM_MainVersion = 3002;
const int Attr_FROM_ReadVersion = 3003;
const int Attr_FROM_McuVersion = 3004;
const int Attr_FROM_ArmVersion = 3005;
const int Attr_FROM_KernelVersion = 3006;
const int Attr_FROM_ProtocolVersion = 3007;
const int Attr_FROM_MasterBuildTime = 3008;
const int Attr_FROM_SlaveBuildTime = 3009;
const int Attr_FROM_McuBuildTime = 3010;
const int Attr_FROM_RowPreDelayTime = 3011;
const int Attr_FROM_RowPostDelayTime = 3012;
const int Attr_FROM_IntegrateTime = 3013;
const int Attr_FROM_ZoomMode = 3014;
const int Attr_FROM_ExpEnable_SignalLevel = 3015;
const int Attr_FROM_SelfClearEnable = 3016;
const int Attr_FROM_SelfClearSpanTime = 3017;
const int Attr_FROM_SequenceIntervalTime = 3018;
const int Attr_FROM_TriggerMode = 3019;
const int Attr_FROM_DynamicFlag = 3020;
const int Attr_FROM_TubeReadyTime = 3021;
const int Attr_FROM_SequenceIntervalTime_HighPrecision = 3022;
const int Attr_FROM_SetDelayTime = 3023;
const int Attr_FROM_ExpWindowTime = 3025;
const int Attr_FROM_SyncExpTime = 3027;
const int Attr_FROM_SyncExpTime_HighPrecision = 3028;
const int Attr_FROM_VT = 3029;
const int Attr_FROM_PGA = 3030;
const int Attr_FROM_PrepCapMode = 3032;
const int Attr_FROM_SelfCapEnable = 3033;
const int Attr_FROM_FluroSync = 3034;
const int Attr_FROM_SrcPort = 3035;
const int Attr_FROM_SrcIP = 3036;
const int Attr_FROM_SrcMAC = 3037;
const int Attr_FROM_DestPort = 3038;
const int Attr_FROM_DestIP = 3039;
const int Attr_FROM_DestMAC = 3040;
const int Attr_FROM_SyncboxIP = 3041;
const int Attr_FROM_PreviewImgMode = 3044;
const int Attr_FROM_HWOffsetType = 3045;
const int Attr_FROM_AcquireDelayTime = 3046;
const int Attr_FROM_BinningMode = 3047;
const int Attr_FROM_ExpMode = 3050;
const int Attr_FROM_AecMainTime = 3051;
const int Attr_FROM_DynaOffsetGapTime = 3052;
const int Attr_FROM_DynaOffsetEnable = 3053;
const int Attr_FROM_ImagePktGapTime = 3054;
const int Attr_FROM_OutModeCapTrig = 3069;
const int Attr_FROM_HvgPrepOn = 3055;
const int Attr_FROM_HvgXRayEnable = 3056;
const int Attr_FROM_HvgXRayOn = 3057;
const int Attr_FROM_HvgXRaySyncOut = 3058;
const int Attr_FROM_HvgXRaySyncIn = 3059;
const int Attr_FROM_CbxBuildTime = 3060;
const int Attr_FROM_SubProductNo = 3061;
const int Attr_FROM_SerialNo = 3062;
const int Attr_FROM_ImageChType = 3063;
const int Attr_FROM_ImageChProtocol = 3064;
const int Attr_FROM_HWGainEnable = 3065;
const int Attr_FROM_ExpTimeValidPercent = 3066;
const int Attr_FROM_FreesyncCenterThreshold = 3067;
const int Attr_FROM_FreesyncEdgeThreshold = 3068;
const int Attr_FROM_FreesyncSubFlow = 3070;
const int Attr_FROM_AutoSleepIdleTime = 3071;
const int Attr_FROM_FSyncParalClearTimes = 3072;
const int Attr_FROM_FSyncFastScanCpvCycle = 3073;
const int Attr_FROM_FSyncTriggerCheckTimeout = 3074;
const int Attr_FROM_FSyncSegmentThreshold = 3075;
const int Attr_FROM_FSyncLineThreshold = 3076;
const int Attr_FROM_FSyncFalseTriggerUnresponseStageTime = 3077;
const int Attr_FROM_FSyncParalClearLine = 3078;
const int Attr_FROM_PowSeriesCorrectEnable = 3079;
const int Attr_FROM_PulseClearTimes = 3080;
const int Attr_FROM_ROIColStartPos = 3081;
const int Attr_FROM_ROIColEndPos = 3082;
const int Attr_FROM_ROIRowStartPos = 3083;
const int Attr_FROM_ROIRowEndPos = 3084;
const int Attr_FROM_FullWell = 3085;
const int Attr_FROM_InnerSubFlow = 3086;
const int Attr_FROM_SoftwareSubFlow = 3087;
const int Attr_FROM_Debug1 = 3200;
const int Attr_FROM_Debug2 = 3201;
const int Attr_FROM_Debug3 = 3202;
const int Attr_FROM_Debug4 = 3203;
const int Attr_FROM_Debug5 = 3204;
const int Attr_FROM_Debug6 = 3205;
const int Attr_FROM_Debug7 = 3206;
const int Attr_FROM_Debug8 = 3207;
const int Attr_FROM_Debug9 = 3208;
const int Attr_FROM_Debug10 = 3209;
const int Attr_FROM_Debug11 = 3210;
const int Attr_FROM_Debug12 = 3211;
const int Attr_FROM_Debug13 = 3212;
const int Attr_FROM_Debug14 = 3213;
const int Attr_FROM_Debug15 = 3214;
const int Attr_FROM_Debug16 = 3215;
const int Attr_FROM_Debug17 = 3216;
const int Attr_FROM_Debug18 = 3217;
const int Attr_FROM_Debug19 = 3218;
const int Attr_FROM_Debug20 = 3219;
const int Attr_FROM_Debug21 = 3220;
const int Attr_FROM_Debug22 = 3221;
const int Attr_FROM_Debug23 = 3222;
const int Attr_FROM_Debug24 = 3223;
const int Attr_FROM_Debug25 = 3224;
const int Attr_FROM_Debug26 = 3225;
const int Attr_FROM_Debug27 = 3226;
const int Attr_FROM_Debug28 = 3227;
const int Attr_FROM_Debug29 = 3228;
const int Attr_FROM_Debug30 = 3229;
const int Attr_FROM_Debug31 = 3230;
const int Attr_FROM_Debug32 = 3231;
const int Attr_FROM_Test1 = 3232;
const int Attr_FROM_Test2 = 3233;
const int Attr_FROM_Test3 = 3234;
const int Attr_FROM_Test4 = 3235;
const int Attr_FROM_Test5 = 3236;
const int Attr_FROM_Test6 = 3237;
const int Attr_FROM_Test7 = 3238;
const int Attr_FROM_Test8 = 3239;
const int Attr_FROM_Test9 = 3240;
const int Attr_FROM_Test10 = 3241;
const int Attr_FROM_Test11 = 3242;
const int Attr_FROM_Test12 = 3243;
const int Attr_FROM_Test13 = 3244;
const int Attr_FROM_Test14 = 3245;
const int Attr_FROM_Test15 = 3246;
const int Attr_FROM_Test16 = 3247;
const int Attr_FROM_RowPreDelayTime_W = 3511;
const int Attr_FROM_RowPostDelayTime_W = 3512;
const int Attr_FROM_IntegrateTime_W = 3513;
const int Attr_FROM_ZoomMode_W = 3514;
const int Attr_FROM_ExpEnable_SignalLevel_W = 3515;
const int Attr_FROM_SelfClearEnable_W = 3516;
const int Attr_FROM_SelfClearSpanTime_W = 3517;
const int Attr_FROM_SequenceIntervalTime_W = 3518;
const int Attr_FROM_TriggerMode_W = 3519;
const int Attr_FROM_DynamicFlag_W = 3520;
const int Attr_FROM_TubeReadyTime_W = 3521;
const int Attr_FROM_SequenceIntervalTime_HighPrecision_W = 3522;
const int Attr_FROM_SetDelayTime_W = 3523;
const int Attr_FROM_ExpWindowTime_W = 3525;
const int Attr_FROM_VT_W = 3529;
const int Attr_FROM_PGA_W = 3530;
const int Attr_FROM_PrepCapMode_W = 3532;
const int Attr_FROM_SelfCapEnable_W = 3533;
const int Attr_FROM_FluroSync_W = 3534;
const int Attr_FROM_SrcIP_W = 3536;
const int Attr_FROM_SrcMAC_W = 3537;
const int Attr_FROM_DestPort_W = 3538;
const int Attr_FROM_DestIP_W = 3539;
const int Attr_FROM_DestMAC_W = 3540;
const int Attr_FROM_SyncboxIP_W = 3541;
const int Attr_FROM_PreviewImgMode_W = 3544;
const int Attr_FROM_HWOffsetType_W = 3545;
const int Attr_FROM_AcquireDelayTime_W = 3546;
const int Attr_FROM_BinningMode_W = 3547;
const int Attr_FROM_ExpMode_W = 3550;
const int Attr_FROM_AecMainTime_W = 3551;
const int Attr_FROM_DynaOffsetGapTime_W = 3552;
const int Attr_FROM_DynaOffsetEnable_W = 3553;
const int Attr_FROM_ImagePktGapTime_W = 3554;
const int Attr_FROM_OutModeCapTrig_W = 3569;
const int Attr_FROM_HvgPrepOn_W = 3555;
const int Attr_FROM_HvgXRayEnable_W = 3556;
const int Attr_FROM_HvgXRayOn_W = 3557;
const int Attr_FROM_HvgXRaySyncOut_W = 3558;
const int Attr_FROM_HvgXRaySyncIn_W = 3559;
const int Attr_FROM_CbxBuildTime_W = 3560;
const int Attr_FROM_SubProductNo_W = 3561;
const int Attr_FROM_SerialNo_W = 3562;
const int Attr_FROM_ImageChType_W = 3563;
const int Attr_FROM_ImageChProtocol_W = 3564;
const int Attr_FROM_HWGainEnable_W = 3565;
const int Attr_FROM_ExpTimeValidPercent_W = 3566;
const int Attr_FROM_FreesyncCenterThreshold_W = 3567;
const int Attr_FROM_FreesyncEdgeThreshold_W = 3568;
const int Attr_FROM_FreesyncSubFlow_W = 3570;
const int Attr_FROM_AutoSleepIdleTime_W = 3571;
const int Attr_FROM_FSyncParalClearTimes_W = 3572;
const int Attr_FROM_FSyncFastScanCpvCycle_W = 3573;
const int Attr_FROM_FSyncTriggerCheckTimeout_W = 3574;
const int Attr_FROM_FSyncSegmentThreshold_W = 3575;
const int Attr_FROM_FSyncLineThreshold_W = 3576;
const int Attr_FROM_FSyncFalseTriggerUnresponseStageTime_W = 3577;
const int Attr_FROM_FSyncParalClearLine_W = 3578;
const int Attr_FROM_PowSeriesCorrectEnable_W = 3579;
const int Attr_FROM_PulseClearTimes_W = 3580;
const int Attr_FROM_ROIColStartPos_W = 3581;
const int Attr_FROM_ROIColEndPos_W = 3582;
const int Attr_FROM_ROIRowStartPos_W = 3583;
const int Attr_FROM_ROIRowEndPos_W = 3584;
const int Attr_FROM_FullWell_W = 3585;
const int Attr_FROM_InnerSubFlow_W = 3586;
const int Attr_FROM_SoftwareSubFlow_W = 3587;
const int Attr_FROM_Debug1_W = 3700;
const int Attr_FROM_Debug2_W = 3701;
const int Attr_FROM_Debug3_W = 3702;
const int Attr_FROM_Debug4_W = 3703;
const int Attr_FROM_Debug5_W = 3704;
const int Attr_FROM_Debug6_W = 3705;
const int Attr_FROM_Debug7_W = 3706;
const int Attr_FROM_Debug8_W = 3707;
const int Attr_FROM_Debug9_W = 3708;
const int Attr_FROM_Debug10_W = 3709;
const int Attr_FROM_Debug11_W = 3710;
const int Attr_FROM_Debug12_W = 3711;
const int Attr_FROM_Debug13_W = 3712;
const int Attr_FROM_Debug14_W = 3713;
const int Attr_FROM_Debug15_W = 3714;
const int Attr_FROM_Debug16_W = 3715;
const int Attr_FROM_Debug17_W = 3716;
const int Attr_FROM_Debug18_W = 3717;
const int Attr_FROM_Debug19_W = 3718;
const int Attr_FROM_Debug20_W = 3719;
const int Attr_FROM_Debug21_W = 3720;
const int Attr_FROM_Debug22_W = 3721;
const int Attr_FROM_Debug23_W = 3722;
const int Attr_FROM_Debug24_W = 3723;
const int Attr_FROM_Debug25_W = 3724;
const int Attr_FROM_Debug26_W = 3725;
const int Attr_FROM_Debug27_W = 3726;
const int Attr_FROM_Debug28_W = 3727;
const int Attr_FROM_Debug29_W = 3728;
const int Attr_FROM_Debug30_W = 3729;
const int Attr_FROM_Debug31_W = 3730;
const int Attr_FROM_Debug32_W = 3731;
const int Attr_FROM_Test1_W = 3732;
const int Attr_FROM_Test2_W = 3733;
const int Attr_FROM_Test3_W = 3734;
const int Attr_FROM_Test4_W = 3735;
const int Attr_FROM_Test5_W = 3736;
const int Attr_FROM_Test6_W = 3737;
const int Attr_FROM_Test7_W = 3738;
const int Attr_FROM_Test8_W = 3739;
const int Attr_FROM_Test9_W = 3740;
const int Attr_FROM_Test10_W = 3741;
const int Attr_FROM_Test11_W = 3742;
const int Attr_FROM_Test12_W = 3743;
const int Attr_FROM_Test13_W = 3744;
const int Attr_FROM_Test14_W = 3745;
const int Attr_FROM_Test15_W = 3746;
const int Attr_FROM_Test16_W = 3747;
const int Attr_Wifi_AP_SSID = 4001;
const int Attr_Wifi_AP_Key = 4002;
const int Attr_Wifi_AP_CountryCode = 4003;
const int Attr_Wifi_AP_FrequencySel = 4004;
const int Attr_Wifi_AP_BandWidthSel = 4005;
const int Attr_Wifi_AP_ChannelSel = 4006;
const int Attr_Wifi_AP_SecuritySel = 4007;
const int Attr_Wifi_AP_ApModeEn = 4008;
const int Attr_Wifi_AP_DhcpServerEn = 4009;
const int Attr_Wifi_Client_ListNum = 4010;
const int Attr_Wifi_Client_CurrentSel = 4011;
const int Attr_Wifi_Client_SSID0 = 4012;
const int Attr_Wifi_Client_Key0 = 4013;
const int Attr_Wifi_Client_SSID1 = 4014;
const int Attr_Wifi_Client_Key1 = 4015;
const int Attr_Wifi_Client_SSID2 = 4016;
const int Attr_Wifi_Client_Key2 = 4017;
const int Attr_Wifi_Client_SSID3 = 4018;
const int Attr_Wifi_Client_Key3 = 4019;
const int Attr_Wifi_Client_SSID4 = 4020;
const int Attr_Wifi_Client_Key4 = 4021;
const int Attr_Wifi_Client_SSID5 = 4022;
const int Attr_Wifi_Client_Key5 = 4023;
const int Attr_Wifi_Client_SSID6 = 4024;
const int Attr_Wifi_Client_Key6 = 4025;
const int Attr_Wifi_Client_SSID7 = 4026;
const int Attr_Wifi_Client_Key7 = 4027;
const int Attr_Wifi_Client_SSID8 = 4028;
const int Attr_Wifi_Client_Key8 = 4029;
const int Attr_Wifi_Client_SSID9 = 4030;
const int Attr_Wifi_Client_Key9 = 4031;
const int Attr_Wifi_AP_SSID_W = 4501;
const int Attr_Wifi_AP_Key_W = 4502;
const int Attr_Wifi_AP_CountryCode_W = 4503;
const int Attr_Wifi_AP_FrequencySel_W = 4504;
const int Attr_Wifi_AP_BandWidthSel_W = 4505;
const int Attr_Wifi_AP_ChannelSel_W = 4506;
const int Attr_Wifi_AP_SecuritySel_W = 4507;
const int Attr_Wifi_AP_ApModeEn_W = 4508;
const int Attr_Wifi_AP_DhcpServerEn_W = 4509;
const int Attr_Wifi_Client_ListNum_W = 4510;
const int Attr_Wifi_Client_CurrentSel_W = 4511;
const int Attr_Wifi_Client_SSID0_W = 4512;
const int Attr_Wifi_Client_Key0_W = 4513;
const int Attr_Wifi_Client_SSID1_W = 4514;
const int Attr_Wifi_Client_Key1_W = 4515;
const int Attr_Wifi_Client_SSID2_W = 4516;
const int Attr_Wifi_Client_Key2_W = 4517;
const int Attr_Wifi_Client_SSID3_W = 4518;
const int Attr_Wifi_Client_Key3_W = 4519;
const int Attr_Wifi_Client_SSID4_W = 4520;
const int Attr_Wifi_Client_Key4_W = 4521;
const int Attr_Wifi_Client_SSID5_W = 4522;
const int Attr_Wifi_Client_Key5_W = 4523;
const int Attr_Wifi_Client_SSID6_W = 4524;
const int Attr_Wifi_Client_Key6_W = 4525;
const int Attr_Wifi_Client_SSID7_W = 4526;
const int Attr_Wifi_Client_Key7_W = 4527;
const int Attr_Wifi_Client_SSID8_W = 4528;
const int Attr_Wifi_Client_Key8_W = 4529;
const int Attr_Wifi_Client_SSID9_W = 4530;
const int Attr_Wifi_Client_Key9_W = 4531;
const int Attr_WorkDir = 5001;
const int Attr_State = 5002;
const int Attr_ConnState = 5003;
const int Attr_CurrentTask = 5004;
const int Attr_CurrentTransaction = 5005;
const int Attr_FsmState = 5006;
const int Attr_Width = 5007;
const int Attr_Height = 5008;
const int Attr_PrevImg_Width = 5009;
const int Attr_PrevImg_Height = 5010;
const int Attr_Authority = 5011;
const int Attr_ConnState_CmdCh = 5012;
const int Attr_ConnState_ImgCh = 5013;
const int Attr_ElapsedExpWindowTime = 5014;
const int Attr_FWUpdateProgress = 5015;
const int Attr_ImageTransProgress = 5016;
const int Attr_RdResult_T1 = 5017;
const int Attr_RdResult_T2 = 5018;
const int Attr_RdResult_Humidity = 5019;
const int Attr_RdResult_Shock_Threshold = 5020;
const int Attr_CurrentSubset = 5021;
const int Attr_CurrentCorrectOption = 5022;
const int Attr_OffsetValidityState = 5023;
const int Attr_GainValidityState = 5024;
const int Attr_DefectValidityState = 5025;
const int Attr_LagValidityState = 5026;
const int Attr_GhostValidityState = 5027;
const int Attr_OffsetTotalFrames = 5028;
const int Attr_OffsetValidFrames = 5029;
const int Attr_GainTotalFrames = 5030;
const int Attr_GainValidFrames = 5031;
const int Attr_DefectTotalFrames = 5032;
const int Attr_DefectValidFrames = 5033;
const int Attr_LagTotalFrames = 5034;
const int Attr_LagValidFrames = 5035;
const int Attr_Battery_Exist = 5036;
const int Attr_Battery_Remaining = 5037;
const int Attr_Battery_ChargingStatus = 5038;
const int Attr_Battery_PowerWarnStatus = 5039;
const int Attr_NetworkInterface = 5040;
const int Attr_WifiStatu_LinkedAP = 5041;
const int Attr_WifiStatu_WorkingBand = 5042;
const int Attr_WifiStatu_WorkingSignalIntensity = 5043;
const int Attr_WifiStatu_WorkingLinkQuality = 5044;
const int Attr_WifiStatu_WorkingTxPower = 5045;
const int Attr_HwTmpl_Offset_Enable = 5046;
const int Attr_HwTmpl_Offset_ValidIndex = 5047;
const int Attr_HwTmpl_Offset_FileCount = 5048;
const int Attr_HwTmpl_Gain_Enable = 5049;
const int Attr_HwTmpl_Gain_ValidIndex = 5050;
const int Attr_HwTmpl_Gain_FileCount = 5051;
const int Attr_HwTmpl_MostGain_Enable = 5052;
const int Attr_HwTmpl_MostGain_ValidIndex = 5053;
const int Attr_HwTmpl_MostGain_FileCount = 5054;
const int Attr_HwTmpl_Defect_Enable = 5055;
const int Attr_HwTmpl_Defect_ValidIndex = 5056;
const int Attr_HwTmpl_Defect_FileCount = 5057;
const int Attr_HwTmpl_Lag_Enable = 5058;
const int Attr_HwTmpl_Lag_ValidIndex = 5059;
const int Attr_HwTmpl_Lag_FileCount = 5060;
const int Attr_CorrectionPrepared = 5061;
const int Attr_RdResult_OutExpState = 5062;
const int Attr_RdResult_AutoSleepState = 5063;
const int Attr_Battery_ExternalPower = 5064;
const int Attr_GCU_OnlineState = 5065;

#endif
