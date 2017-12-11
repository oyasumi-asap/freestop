	.set noreorder

#include "pspstub.s"

	STUB_START "scePower",0x40090000,0x20005
	STUB_FUNC  0x2085D15D,scePowerGetBatteryLifePercent
	STUB_FUNC  0x8EFB3FA2,scePowerGetBatteryLifeTime
	STUB_FUNC  0xB4432BC8,scePowerGetBatteryChargingStatus
	STUB_FUNC  0xAC32C9CC,scePowerRequestSuspend
	STUB_FUNC  0x2B7C7CF4,scePowerRequestStandby
	STUB_END

	STUB_START "sceRtc",0x40090000,0x00020005
	STUB_FUNC  0x57726BC1,sceRtcGetDayOfWeek
	STUB_FUNC  0xE7C27D1B,sceRtcGetCurrentClockLocalTime
	STUB_END

	STUB_START "sceCtrl",0x40090000,0x00060005
	STUB_FUNC  0x6A2774F3,sceCtrlSetSamplingCycle
	STUB_FUNC  0x1F4011E6,sceCtrlSetSamplingMode
	STUB_END

	STUB_START "IoFileMgrForKernel",0x40090000,0x00080005
	STUB_FUNC  0xACE946E8,sceIoGetstat
	STUB_FUNC  0x6A638D83,sceIoRead
	STUB_FUNC  0x27EB27B8,sceIoLseek
	STUB_FUNC  0x109F50BC,sceIoOpen
	STUB_FUNC  0x810C4BC3,sceIoClose
	STUB_FUNC  0x42EC03AC,sceIoWrite
	STUB_FUNC  0xF27A9C51,sceIoRemove
	STUB_FUNC  0x68963324,sceIoLseek32
	STUB_END

	STUB_START "sceDisplay",0x40090000,0x00040005
	STUB_FUNC  0x984C27E7,sceDisplayWaitVblankStart
	STUB_FUNC  0xDEA197D4,sceDisplayGetMode
	STUB_FUNC  0xEEDA2E54,sceDisplayGetFrameBuf
	STUB_FUNC  0x31C4BAA8,sceDisplayGetBrightness
	STUB_END

	STUB_START "sceVshBridge",0x40090000,0x00010005
	STUB_FUNC  0xC6395C03,vshCtrlReadBufferPositive
	STUB_END

	STUB_START "sceImpose_driver",0x40090000,0x00010005
	STUB_FUNC  0x531C9778,sceImposeGetParam
	STUB_END

	STUB_START "sceSysreg_driver",0x40090000,0x00020005
	STUB_FUNC  0x8E2FB536,sceSysregAudioIoDisable
	STUB_FUNC  0xBB26CF1F,sceSysregAudioIoEnable
	STUB_END
	
	STUB_START "SysMemUserForUser",0x40090000,0x00010005
	STUB_FUNC  0x3FC9AE6A,sceKernelDevkitVersion
	STUB_FUNC  0xa291f107,sceKernelMaxFreeMemSize
	STUB_END

