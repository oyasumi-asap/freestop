#include "common.h"

CANVAS canvas;		// 表示バッファ
pspTime tick_time;

int btryLifeP;
int volume;
int btryH;
int btryM;

void getInfo()
{
	// 現在時刻の取得
	sceRtcGetCurrentClockLocalTime( &tick_time );
	//バッテリー％
	btryLifeP = scePowerGetBatteryLifePercent();
	// バッテリー残り時間
	int btryLifeT = scePowerGetBatteryLifeTime();
	btryH = btryLifeT / 60;
	btryM = btryLifeT - btryH * 60;
	//音量
	volume = sceImposeGetParam(PSP_IMPOSE_MAIN_VOLUME);
}

/*----------------------------------------------------------*/

//http://katsura-kotonoha.sakura.ne.jp/prog/c/tip0000e.shtml
//部分文字列を置換する　　より引用
//*********************************************************
// 文字列 String を nShift だけ移動する。
// 移動先へのポインタを返す。
//*********************************************************
char *StrShift( char *String, size_t nShift )
{
	char *start = String;
	char *stop  = String + strlen( String );
	memmove( start + nShift, start, stop-start+1 );

	return String + nShift;
}//StrShift

//*********************************************************
// 文字列 String の文字列 From を文字列 To で置換する。
// 置換後の文字列 String のサイズが String の記憶領域を超える場合の動作は未定義。
//*********************************************************
char *StrReplace( char *String, const char *From, const char *To )
{
	int   nToLen;   // 置換する文字列の長さ
	int   nFromLen; // 検索する文字列の長さ
	int   nShift;
	char *start;    // 検索を開始する位置
	char *stop;     // 文字列 String の終端
	char *p;

	nToLen   = strlen( To );
	nFromLen = strlen( From );
	nShift   = nToLen - nFromLen;
	start    = String;
	stop     = String + strlen( String );

	// 文字列 String の先頭から文字列 From を検索
	while( NULL != ( p = strstr( start, From ) ) )
	{
		// 文字列 To が複写できるようにする
		start = StrShift( p + nFromLen, nShift );
		stop  = stop + nShift;

		// 文字列 To を複写
		memmove( p, To, nToLen );
	}

	return String;
}//StrReplace

// 関数
u32 GetSetBaseAddrNidDisp(void)
{
	u32 devkit = sceKernelDevkitVersion();

	switch(devkit)
	{
		case PSP_FIRMWARE(0x500):
		case PSP_FIRMWARE(0x503):
		case PSP_FIRMWARE(0x550):
			return 0xBA1605D2;

		case PSP_FIRMWARE(0x620):
			return 0x6CDDF704;

		case PSP_FIRMWARE(0x631):
		case PSP_FIRMWARE(0x635):
		case PSP_FIRMWARE(0x637):
		case PSP_FIRMWARE(0x638):
		case PSP_FIRMWARE(0x639):
		case PSP_FIRMWARE(0x660):
			return 0x8A0E1D73;
		default :
			return 0x29B50A82;
	}

	return 0;
}

u32 GetSetBaseAddr(u32 text_addr, u32 text_end)
{
	u32 addr = 0;

	for(; text_addr < text_end; text_addr += 4)
	{
		if(_lw(text_addr) == 0x00000000 && _lw(text_addr + 4) == 0x00112880)
		{
			addr = (text_addr - 0xC);
			break;
		}
	}

	return addr;
}

