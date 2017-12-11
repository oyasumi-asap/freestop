// スレッド

#ifndef __THREAD_H__
#define __THREAD_H__


// 定義
#define			MAX_THREAD			64
#define			SUSPEND_MODE		0
#define			RESUME_MODE			1


/*
	起動時のスレッドを取得する
	※できるだけ起動時に使ってください
*/
void Get_FirstThreads();

/*
	自分のスレッド以外を停止・再開させる
	mode は SUSPEND_MODE と RESUME_MODE の二つが使えます
*/
void Suspend_Resume_Threads(int mode);


#endif

