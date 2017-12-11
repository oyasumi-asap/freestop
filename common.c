#include "common.h"

CANVAS canvas;		// �\���o�b�t�@
pspTime tick_time;

int btryLifeP;
int volume;
int btryH;
int btryM;

void getInfo()
{
	// ���ݎ����̎擾
	sceRtcGetCurrentClockLocalTime( &tick_time );
	//�o�b�e���[��
	btryLifeP = scePowerGetBatteryLifePercent();
	// �o�b�e���[�c�莞��
	int btryLifeT = scePowerGetBatteryLifeTime();
	btryH = btryLifeT / 60;
	btryM = btryLifeT - btryH * 60;
	//����
	volume = sceImposeGetParam(PSP_IMPOSE_MAIN_VOLUME);
}

/*----------------------------------------------------------*/

//http://katsura-kotonoha.sakura.ne.jp/prog/c/tip0000e.shtml
//�����������u������@�@�����p
//*********************************************************
// ������ String �� nShift �����ړ�����B
// �ړ���ւ̃|�C���^��Ԃ��B
//*********************************************************
char *StrShift( char *String, size_t nShift )
{
	char *start = String;
	char *stop  = String + strlen( String );
	memmove( start + nShift, start, stop-start+1 );

	return String + nShift;
}//StrShift

//*********************************************************
// ������ String �̕����� From �𕶎��� To �Œu������B
// �u����̕����� String �̃T�C�Y�� String �̋L���̈�𒴂���ꍇ�̓���͖���`�B
//*********************************************************
char *StrReplace( char *String, const char *From, const char *To )
{
	int   nToLen;   // �u�����镶����̒���
	int   nFromLen; // �������镶����̒���
	int   nShift;
	char *start;    // �������J�n����ʒu
	char *stop;     // ������ String �̏I�[
	char *p;

	nToLen   = strlen( To );
	nFromLen = strlen( From );
	nShift   = nToLen - nFromLen;
	start    = String;
	stop     = String + strlen( String );

	// ������ String �̐擪���當���� From ������
	while( NULL != ( p = strstr( start, From ) ) )
	{
		// ������ To �����ʂł���悤�ɂ���
		start = StrShift( p + nFromLen, nShift );
		stop  = stop + nShift;

		// ������ To �𕡎�
		memmove( p, To, nToLen );
	}

	return String;
}//StrReplace

// �֐�
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

