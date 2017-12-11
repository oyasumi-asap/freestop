#include <pspkernel.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspsysevent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libmenu.h"

#include "thread.h"
#include "common.h"
#include "color.h"
#include "conf.h"

PSP_MODULE_INFO("freestop", PSP_MODULE_KERNEL, 1, 1);

// ini用変数 iniの項目分必要
#define INI_PATH "/freestop.ini"
#define CONF_NUM 15
INI_Key conf[CONF_NUM];

char iniPath[128];

u32 stopkey = 0;
u32 closekey = 0;
u32 exitkey = 0;
bool Japanese = false;
bool English = false;
bool NoDisplay = false;
bool ResetOn = false;
bool BrightOn = false;
bool Bright0 = false;
bool BeforeTime  = false;
bool BeforeVol  = false;
bool BeforeBattery  = false;
bool AfterTime  = false;
bool AfterVol  = false;
bool AfterBattery  = false;

//print
char printBattery[64];
char printVolInfo[64];
char printTime[512];
char printVolGauge[64];


enum
{
	NoAction
};

enum
{
	LIBM_TCMD_SUSPEND	,	//他スレッドを停止
	LIBM_TCMD_RESUME	,	//他スレッドを再開
	LIBM_TCMD_DUMP		,	//他スレッドを停止・再開させる為のセーフリストを作る
	LIBM_TCMD_GET		,	//他スレッドの停止・再開状態を取得
};


char str[512];
static libmOpt opt;

//プラグイン スタック上にアイテム変数を常置させる
MenuContext	Context;
static MenuItem item_list[ITEM_COUNT];

int beforebright;

//プロトタイプ宣言
int threadMain(SceSize args, void *argp);
int module_start(SceSize args, void *argp);
int module_stop(SceSize args, void *argp);
void MenuCreate();
int sceDmacplusLcdcSetBaseAddrPatched(void *baseaddr);


void MenuCreate()
{
	//メニュー アイテムをメモリから動的確保しない設定にする
	memset(&opt,0,sizeof(opt));
	opt.type		= LIBM_OPT_LIST;
	opt.list.val	= item_list;
	opt.list.size 	= sizeof(item_list);
	
	libmSetOpt(&Context,&opt);
	
	//メニュー操作用の各Pad設定
		
	//表示 HOME
	Context.HotKey.Show				= stopkey;
	//戻る HOME
	Context.HotKey.Back				= closekey;
	//選択 ×
	Context.HotKey.Select			= PSP_CTRL_NOTE | PSP_CTRL_START | PSP_CTRL_LTRIGGER;
	
	//上へ ↑
	Context.HotKey.Up				= PSP_CTRL_UP;
	//下へ ↓
	Context.HotKey.Down				= PSP_CTRL_DOWN;
	//左へ ←
	Context.HotKey.Left				= PSP_CTRL_LEFT;
	//右へ
	Context.HotKey.Right				= PSP_CTRL_RIGHT;
	
	//メニュータイプは「全体表示」
	Context.MenuInfo.Type 			= LIBM_VIEW_ALL;
	//アイテムが最上部・下部へ来たら自動でターンさせる
	Context.MenuInfo.AutoReturn		= true;
		
	//アクティブな項目のフォント色、背景色
	Context.Color.Active_Font		= RED;
	Context.Color.Active_Back		= BLACK;
		
	//非アクティブな項目のフォント色、背景色
	Context.Color.Normal_Font		= RED;
	Context.Color.Normal_Back		= BLACK;
		
	//行間
	Context.MenuInfo.Lines	= 0;
	
	MenuItem* MAIN = libmAddItem(&Context,NULL,libmCreateSpacer(&opt,""),Invalid,Invalid,NoAction,0);

	//アクティブアイテム 初期値を設定
	libmSetActive( &Context ,MAIN);
}

//プラグイン以外の他スレッドを停止・再開させる
int libmExecTCmd( int cmd )
{
	static int thid_status = LIBM_TRESUME;
	static bool thid_IsDump = false;
	static bool this_IsBusy = false;
	
	static int thid_list_first[MAX_NUMBER_OF_THREADS];
	static int thid_count_first;
	
	static int thid_list_Current[MAX_NUMBER_OF_THREADS];
	static int thid_count_Current;
	
	if( cmd ==  LIBM_TCMD_DUMP )
	{
		thid_IsDump = sceKernelGetThreadmanIdList( SCE_KERNEL_TMID_Thread, thid_list_first, MAX_NUMBER_OF_THREADS, &thid_count_first ) >= 0;
		
		return thid_IsDump;
	}
	else if( (cmd == LIBM_TCMD_SUSPEND || cmd == LIBM_TCMD_RESUME) && thid_IsDump && cmd != thid_status && !this_IsBusy )
	{
		thid_status = cmd;
		this_IsBusy = true;
		
		int i, j;
		int (*func)(SceUID) =  ( cmd == LIBM_TCMD_SUSPEND ? sceKernelSuspendThread : sceKernelResumeThread );
		SceKernelThreadInfo thinfo;
		
		
		if( cmd == LIBM_TCMD_SUSPEND )
		{
			//停止
			
			int Current_id	= sceKernelGetThreadId();
			
			//現在のスレッドリストを作る
			if( sceKernelGetThreadmanIdList( SCE_KERNEL_TMID_Thread, thid_list_Current, MAX_NUMBER_OF_THREADS, &thid_count_Current ) < 0 )
			{
				//現在のスレッドリスト取得失敗
				this_IsBusy = false;
				return 0;
			}
			
			
			//現在のスレッド一覧と、プラグイン起動時にダンプしたセーフリストを比較
			//セーフリストと現スレッドに一致するものは対象外へ
			for( i = 0; i < thid_count_Current; i++ )
			{
				memset(&thinfo, 0, sizeof(SceKernelThreadInfo));
				thinfo.size = sizeof(SceKernelThreadInfo);
				sceKernelReferThreadStatus(thid_list_Current[i], &thinfo);
				
				if
				(
				//	( strcasecmp( "SceImpose",thinfo.name) == 0 )
				//||
					( thinfo.attr & 0x100000 ) == 0x100000
				||
					( thid_list_Current[i] == Current_id )
				)
				{
					thid_list_Current[i] = 0;
					continue;
				}
				
				for( j = 0; j < thid_count_first; j++ )
				{
					if( thid_list_Current[i] == thid_list_first[j] )
					{
						thid_list_Current[i] = 0;
						break;
					}
				}
			}
		}
		
		
		//最終的な現在のスレッドリストにあるスレッドへ停止・再開の操作
		for( i = 0; i < thid_count_Current; i++ )
		{
			if( thid_list_Current[i] ) ( func )( thid_list_Current[i] );
		}
		
		this_IsBusy = false;
		
		return 1;
	}
	else if( cmd == LIBM_TCMD_GET )
	{
		return thid_status;
	}
	
	return 0;
}


int threadPrint(SceSize args, void *argp){
	while(1){
			printMenu();
		sceKernelDelayThread( 1000 );
	}
return 0;
}

int Callback_Suspend(int ev_id, char *ev_name, void *param, int *result)
{
	// 返り値に 0 ではなく -1 を返すと、スリープを無効化できる
	
	// スリープ時
	if( ev_id == PSP_SYSEVENT_SUSPEND_QUERY && libmExecTCmd(LIBM_TCMD_GET) == LIBM_TSUSPEND )
	{
		libmExecTCmd( LIBM_TCMD_RESUME );
		Context.IsOpen = false;
	}
	
	return 0;
}

//Main
int threadMain(SceSize args, void *argp)
{			
	//ここらへんよく分からないorz
	while(1){
		if(
			sceKernelFindModuleByName("sceKernelLibrary") &&
			sceKernelFindModuleByName("sceIOFileManager") &&
			sceKernelFindModuleByName("sceController_Service") &&
			sceKernelFindModuleByName("sceDisplay_Service") &&
			sceKernelFindModuleByName("sceSystemMemoryManager")
		)
			break;

		sceKernelDelayThread(1000000);
	}
	
	if(BrightOn == false){
	libmPrintXY(187,114,BLACK,BLACK,"");
	}else{
	sceDisplayGetBrightness(&beforebright,0);
	}
	
	if(Bright0 == false){
	libmPrintXY(187,114,BLACK,BLACK,"");
	}else{
	sceDisplayGetBrightness(&beforebright,0);
	}
	
	PspSysEventHandler events;
		
	SceCtrlData pad;
	MenuParams* Params;
	
	events.size			= 0x40;
	events.name			= "MSE_Suspend";
	events.type_mask	= 0x0000FF00;
	events.handler		= Callback_Suspend;
	
	sceKernelRegisterSysEventHandler(&events);
			
	//メニュー作成
	MenuCreate();
	
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
						
	//モンハンなどの場合
	SceUID print_thid = sceKernelCreateThread("PrintMenu", threadPrint, 1,0x3000, 0, NULL);
	if(threadInfo >= 0)sceKernelStartThread(print_thid, args, argp);

	while( 1 )
	{
	   	sceCtrlPeekBufferPositive(&pad, 1);
		
		//Key入力を渡して、メニュー内の動作を調べる（結果を得る）
		Params = libmGetHandle(&Context, &pad);
		if(Params->Action == Menu_Close)
		{	
			sceKernelDelayThread(100000);
			Suspend_Resume_Threads(1);
			if(BrightOn == false){
			libmPrintXY(187,114,BLACK,BLACK,"");
			}else{		
			sceDisplaySetBrightness(beforebright,0);
			}
			if(Bright0 == false){
			libmPrintXY(187,114,BLACK,BLACK,"");
			}else{		
			sceDisplaySetBrightness(beforebright,0);
			}
		}		
		
		//0.001秒待つ		
		sceKernelDelayThread(1000);
	}
	
	return sceKernelExitDeleteThread( 0 );
}

int threadInfo(SceSize args, void *argp)
{
 while(1){
	int i;
	int flag1;

	getInfo();
		
	//充電されるまで待つ
	flag1 = scePowerIsPowerOnline();
	

		//音量表示
		sprintf(printVolInfo,"%2d",volume);
		sprintf(printVolGauge," ");

		for(i=0; i<volume; i++){
			strcat( printVolGauge,"｜");
		}
		for(; i<30; i++){
			strcat( printVolGauge,"・");
		}
				
		//バッテリー残量表示
		if(flag1 == 0){
			sprintf(printBattery,"  %3d%%(%d:%02d)   ",btryLifeP,btryH,btryM);
		}
		else if(flag1 == 1)
		{
			sprintf(printBattery,"  Charging Now ");		
		}
		
			//時間表示
			sprintf(printTime," %2d:%02d:%02d    ",tick_time.hour,tick_time.minutes,tick_time.seconds);
							
		sceKernelDelayThread(100 * 1000);
		//0.1秒ディレイ
}
  return 0;
}



void printMenu(void)
{		
	SceCtrlData pad;
	sceCtrlPeekBufferPositive(&pad, 1);
		
	if(libmInitBuffers(0,PSP_DISPLAY_SETBUF_NEXTFRAME)){	
	if(BeforeTime == false){
	libmPrintXY(0,264,GREEN,0,"");
	}else{
	libmPrintXY(0,264,GREEN,0,printTime);
	}
						
	if(BeforeVol == false){
	libmPrintXY(0,264,GREEN,0,"");
	}else{
	libmPrintXY(112,264,GREEN,0,printVolGauge);
	libmPrintXY(98,264,GREEN,0,printVolInfo);
	}
			
	if(BeforeBattery == false){
	libmPrintXY(0,264,GREEN,0,"");
	}else{
	libmPrintXY(360,264,GREEN,0,printBattery);
	}
	
	if( libmIsOpen(&Context)){		
	
			if(AfterTime == false){
			libmPrintXY(0,264,GREEN,0,"");
			}else{
			libmPrintXY(0,264,WHITE,BLACK,printTime);
			}
			
			if(AfterVol == false){
			libmPrintXY(0,264,GREEN,0,"");
			}else{
			libmPrintXY(98,264,WHITE,BLACK,printVolInfo);
			libmPrintXY(112,264,WHITE,BLACK,printVolGauge);
			}
						
			if(AfterBattery == false){
			libmPrintXY(0,264,GREEN,0,"");
			}else{
			libmPrintXY(360,264,WHITE,BLACK,printBattery);
			}
			
			if(BrightOn == false){
			libmPrintXY(187,114,BLACK,BLACK,"");
			}else{
			sceDisplaySetBrightness(45,0);
			}
			
			if(Bright0 == false){
			libmPrintXY(187,114,BLACK,BLACK,"");
			}else{		
			sceDisplaySetBrightness(0,0);
			}
			
			if(Japanese == false){
			libmPrintXY(0,264,GREEN,0,"");
			}else{
			libmPrintXY(187,114,BLACK,BLACK,"...............");
			libmPrintXY(187,126,BLACK,BLACK,"...............");
			libmPrintXY(275,120,BLACK,BLACK,"..");
			libmPrintXY(187,120,BLACK,BLACK,"..");
			libmPrintXY(195,120,WHITE,BLACK,"  一 時 停 止 中   ");
			libmRender(&Context,140,110,str,256);
			Suspend_Resume_Threads(0);
			}
			
			if(English == false){
			libmPrintXY(0,264,GREEN,0,"");
			}else{
			libmPrintXY(187,114,BLACK,BLACK,"...............");
			libmPrintXY(187,126,BLACK,BLACK,"...............");
			libmPrintXY(275,120,BLACK,BLACK,"..");
			libmPrintXY(187,120,BLACK,BLACK,"..");
			libmPrintXY(195,120,WHITE,BLACK,"   Stopping   ");
			libmRender(&Context,140,110,str,256);
			Suspend_Resume_Threads(0);
			}
			
			if(NoDisplay == false){
			libmPrintXY(0,264,GREEN,0,"");
			}else{
			libmRender(&Context,140,110,str,256);
			Suspend_Resume_Threads(0);
			}
			
			if (pad.Buttons & PSP_CTRL_HOME){
			Suspend_Resume_Threads(1);
			Context.IsOpen = false;
			}
			
			if(ResetOn == false){
			libmPrintXY(187,114,BLACK,BLACK,"");
			}else{
			
				if ( (exitkey) && ((pad.Buttons& exitkey) == exitkey) ){
				Suspend_Resume_Threads(1);
				Context.IsOpen = false;
				
				if(BrightOn == false){
				libmPrintXY(187,114,BLACK,BLACK,"");
				}else{
				sceDisplaySetBrightness(beforebright,0);
				}
				
				if(Bright0 == false){
				libmPrintXY(187,114,BLACK,BLACK,"");
				}else{		
				sceDisplaySetBrightness(beforebright,0);
				}
				sceKernelExitVSHVSH(0);
				}
			}
			sceDisplayWaitVblankStart();
		}	
	}	
}


int module_start(SceSize args, void *argp)
{	
	char *temp;

	//読み込むiniファイルのパスを決定
	strcpy(iniPath, argp);
	// ms0:/seplugins/foo.prxが読み込み(実行されたとすると), iniPathに"ms0:/seplugins/foo.prx"がコピーされる
	temp = strrchr(iniPath, '/');
	// iniPathの一番後ろにある '/' を探し、そのポインターをtempに
	if( temp != NULL ) *temp = '\0';
	// temp == NULLは'/'が探せなかったとき,つまり temp != NULL は'/'が探せたとき
	// '/'をEOS('\0')に変える、つまりiniPathは"ms0:/seplugins"となる
	strcat(iniPath, INI_PATH);
	//iniPathの末尾に"/***.ini"を結合、つまりつまりiniPathは"ms0:/seplugins/***.ini"となる	

	// iniファイルの読み込みの下準備
	INI_Init_Key(conf);//初期化(必須)

	INI_Add_Button(conf, "stopkey", &stopkey, PSP_CTRL_UP | PSP_CTRL_RTRIGGER | PSP_CTRL_LTRIGGER);
	INI_Add_Button(conf, "closekey", &closekey , PSP_CTRL_CROSS );
	INI_Add_Button(conf, "exitkey", &exitkey , PSP_CTRL_RTRIGGER | PSP_CTRL_START );
	INI_Add_Bool(conf, "Japanese", &Japanese, true );
	INI_Add_Bool(conf, "English", &English, false );
	INI_Add_Bool(conf, "NoDisplay", &NoDisplay, false );
	INI_Add_Bool(conf, "BeforeTime", &BeforeTime, false );
	INI_Add_Bool(conf, "BeforeVol", &BeforeVol, false );
	INI_Add_Bool(conf, "BeforeBattery", &BeforeBattery, false );
	INI_Add_Bool(conf, "AfterTime", &AfterTime, false );
	INI_Add_Bool(conf, "AfterVol", &AfterVol, false );
	INI_Add_Bool(conf, "AfterBattery", &AfterBattery, false );
	INI_Add_Bool(conf, "ResetOn", &ResetOn, false );
	INI_Add_Bool(conf, "BrightOn", &BrightOn, false );
	INI_Add_Bool(conf, "Bright0", &Bright0, false );
	
	// iniファイルの読み込み
	if(INI_Read_Conf(iniPath, conf) == 1){
	// iniファイルへ書き込み
		INI_Write_Conf(iniPath, conf, "\r\n");
	}
	
	//NidResolve
	sceDmacplusLcdcSetBaseAddrReal		= (void *)FindProc("sceLowIO_Driver", "sceDmacplus_driver", GetSetBaseAddrNidDisp());

	//メインスレッドのスタックサイズをアイテムリストの変数サイズ分 増やしておく必要がある
	SceUID main_thid = sceKernelCreateThread("Main", threadMain, 16,0x3000, 0, NULL);
	if(main_thid >= 0)sceKernelStartThread(main_thid, args, argp);
	
	//情報取得スレッド開始
	SceUID info_thid = sceKernelCreateThread("GetInfo", threadInfo, 16,0x3000, 0, NULL);
	if(threadInfo >= 0)sceKernelStartThread(info_thid, args, argp);	
	
	//パッチなど
	u32 orgaddr = FindProc("sceDisplay_Service", "sceDisplay", 0x289D82FE);
	sceDisplaySetFrameBuf_Real = (void *)orgaddr;
	
	//現在の動作を取得
	Get_FirstThreads();

    return 0;
}


int module_stop(SceSize args, void *argp)
{
    return 0;
}

