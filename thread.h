// �X���b�h

#ifndef __THREAD_H__
#define __THREAD_H__


// ��`
#define			MAX_THREAD			64
#define			SUSPEND_MODE		0
#define			RESUME_MODE			1


/*
	�N�����̃X���b�h���擾����
	���ł��邾���N�����Ɏg���Ă�������
*/
void Get_FirstThreads();

/*
	�����̃X���b�h�ȊO���~�E�ĊJ������
	mode �� SUSPEND_MODE �� RESUME_MODE �̓���g���܂�
*/
void Suspend_Resume_Threads(int mode);


#endif

