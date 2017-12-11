#ifndef COMMON_H
#define COMMON_H

#include <pspctrl.h>
#include <pspctrl_kernel.h>
#include <pspdisplay.h>
#include <pspdisplay_kernel.h>
#include <pspge.h>
#include <pspimpose_driver.h>
#include <pspkernel.h>
#include <psppower.h>
#include <psprtc.h>
#include <pspsdk.h>
#include <pspsysclib.h>
#include <pspsysevent.h>
#include <psputilsforkernel.h>
#include <kubridge.h>
#include <systemctrl.h>
#include <systemctrl_se.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "libmenu.h"
#include "color.h"
#include "conf.h"

//マクロ--------------------------------------------------------
#define	LIBM_TSUSPEND				LIBM_TCMD_SUSPEND
#define	LIBM_TRESUME				LIBM_TCMD_RESUME

#define MAX_NUMBER_OF_THREADS 		64
#define	PSP_SYSEVENT_SUSPEND_QUERY	0x100
#define	ITEM_COUNT					66

#define MAKE_CALL(a, f)	_sw(0x0C000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a);
#define PSP_FIRMWARE(f) ((((f >> 8) & 0xF) << 24) | (((f >> 4) & 0xF) << 16) | ((f & 0xF) << 8) | 0x10)


//ボタンフック
extern int controller_hooked;
extern int disable_controller;

extern int btryLifeP;

typedef struct
{
	int width;
	int height;
	int linewidth;
	int pixelformat;
	u32 *buffer;
} CANVAS;



extern CANVAS canvas;
extern pspTime tick_time;
extern int btryLifeP;
extern int volume;
extern int btryH;
extern int btryM;
extern int Brightness;
extern int weekNumber;


// 設定ファイル用変数
extern bool ChangeLang;

int libmPrintXY_Right( int x, int y, u32 fg, u32 bg, const char *str );

void printMenu();
void setInfo();
void getInfo();
u32 GetSetBaseAddrNidVol(void);
char *StrShift( char *String, size_t nShift );
char *StrReplace( char *String, const char *From, const char *To );

int threadInfo(SceSize args, void *argp);
int threadPrint(SceSize args, void *argp);

int (*sceRtcGetCurrentClockLocalTime_Real) (pspTime *time);
int (*scePowerGetBatteryLifePercent_Real) (void);

extern MenuContext	Context;

//*********************************************************
// 文字列 String の文字列 From を文字列 To で置換する。
// 置換後の文字列 String のサイズが String の記憶領域を超える場合の動作は未定義。
//*********************************************************
char *StrReplace( char *String, const char *From, const char *To );

//スレッド停止
int libmExecTCmd( int cmd );
int Callback_Suspend(int ev_id, char *ev_name, void *param, int *result);

//画面フック
int (*sceDmacplusLcdcSetBaseAddrReal)(void *baseaddr);
u32 GetSetBaseAddr(u32 text_addr, u32 text_end);
u32 GetSetBaseAddrNidDisp(void);
int (*sceDisplaySetFrameBuf_Real)(void *topaddr, int bufferwidth, int pixelformat, int sync);

//void saveLibraryEntryTableInfo(SceUID uid, char *filename);
u32 GetSetBaseAddrNidDisp(void);
u32 GetSetBaseAddr(u32 text_addr, u32 text_end);

int (*sceDmacplusLcdcSetBaseAddrReal)(void *baseaddr);
int (*sceCtrlReadBufferPositive_Real)(SceCtrlData *pad_data, int count);
int (*sceCtrlReadBufferNegative_Real)(SceCtrlData *pad_data, int count);
int (*sceCtrlPeekBufferPositive_Real)(SceCtrlData *pad_data, int count);
int (*sceCtrlPeekBufferNegative_Real)(SceCtrlData *pad_data, int count);
int (*vshCtrlReadBufferPositive_Real)(SceCtrlData *pad_data, int count);
int (*sceKernelDevkitVersion_Real)(void);//5.03を識別するために使った

#endif