#ifndef LIBMENU_H
#define LIBMENU_H

#include <stdbool.h>
#include <pspctrl.h>


#ifdef __cplusplus
extern "C" {
#endif



// libmInitBuffers で第1引数に指定出来るVRAM初期化オプション
enum
{
	LIBM_DRAW_INIT8888	= 0x01 ,
	LIBM_DRAW_INIT4444	= 0x02 ,
	LIBM_DRAW_INIT5650	= 0x04 ,
	LIBM_DRAW_INIT5551	= 0x08 ,
	LIBM_DRAW_BLEND		= 0x10 ,
	LIBM_DRAW_RETURN	= 0x20 ,
};

#define	LIBM_FMT_MASK			(LIBM_DRAW_INIT8888 | LIBM_DRAW_INIT4444 | LIBM_DRAW_INIT5650 | LIBM_DRAW_INIT5551)


#define	LIBM_DRAW_INIT			(LIBM_FMT_MASK | LIBM_DRAW_RETURN)

//libmDebugScreenSetXYで指定可能なX,Yの最大値
#define	LIBM_SETX_MAX			59
#define	LIBM_SETY_MAX			33

//libmenu内部フォントの縦横サイズ
#define LIBM_CHAR_WIDTH			8
#define LIBM_CHAR_HEIGHT		8

#define	LIBM_NO_DRAW			0

#define	LIBM_VIEW_ALL			false
#define	LIBM_VIEW_ACTIVE		true


typedef enum
{
	MenuContainer,
	RadioButton,
	ToggleButton,
	CheckBox,
	Spacer,
	SelectBox,
	UpDownCtrl,
	TriggerButton,
	RadioButtonGroup
} MenuItemType;



// libmGetHandleを実行後に Action を参照して得られるメニューの状態
typedef enum
{
	Menu_None = 0,	//何もなし
	Menu_Show,		//メニューが表示された
	Menu_Up,		//アイテム選択状態で "上" に指定されているボタンが押された
	Menu_Down,		//アイテム選択状態で "下" に指定されているボタンが押された
	Menu_Left,		//アイテム選択状態で "左" に指定されているボタンが押された
	Menu_Right,		//アイテム選択状態で "右" に指定されているボタンが押された
	Menu_Select,	//アイテム選択状態で "選択" に指定されているボタンが押された
	Menu_Back,		//アイテム選択状態で "戻る" に指定されているボタンが押された
	Menu_Close		//メニューが閉じられた
} MenuResult;


// libmOpt、typeへ指定可能なタイプ一覧
typedef enum
{
	LIBM_OPT_NONE,	//無効
	LIBM_OPT_LIST,	//プログラム上の変数からメニュー作成
	LIBM_OPT_FUNC,	//メモリから変数を動的確保してメニュー作成
} optType;



typedef struct MenuItem
{
	struct MenuItem			*Parent;		//アイテムの親 ( NULL = Root )
	struct MenuItem			*Next;			//アイテムの次
	struct MenuItem			*Prev;			//アイテムの前
	struct MenuItem			*Children;		//アイテムの子
	
	
	const char				*Name;			//アイテム名（表示される）
	int						Type;			//アイテムのタイプ
	int						actionID;		//アイテムが選択されたりした時に、handleMenuの返却値へ送るactionID
	int						Data;			//アイテム値
	
	bool					Visible;		//アイテムの可視状態( ture ,false )
	bool					Disable;		//アイテムの有効・無効 状態
	
	
	//非アクティブ時のアイテム描画色
	//指定がなければ Context の通常色
	
	struct
	{
		u32 				Font;			//フォント
		u32 				Back;			//背景
		u32					Line;			//下線
	}Color;
	
	
	union
	{
		//ラジオボタン用
		struct MenuItem		*RB_Group;		//グループ先へのポインター
		
		//ラジオボタン・グループ用
		struct MenuItem		*Group_Sel;		//選択中アイテムへのポインター
		
		//コンテナー用
		struct
		{
			bool			IsOpen;			//開閉状態
			struct MenuItem	*Disp;			//関連付けて状態表示させるアイテムへのポインター
		}CNT;
		
		//チェックボックス用
		bool				CB_Checked; 	//選択状態
		
		//リストボックス用
		struct
		{
			const char		**List;			//リストへのポインター
			int				Index;			//選択中のindex
		}SB;
		
		//アップダウン用
		struct
		{
			float			Now;			//現在地
			float			Step;			//Step値 (+/-)
			float			Min;			//最小値
			float			Max;			//最大値
			bool			type;			//表示タイプ ( true = 整数 , false = 小数 )
		}UD;
		
	}Ctrl;
	
} MenuItem;


typedef struct MenuAction
{
	MenuResult	Action;
	MenuItem	*Item;
} MenuParams;


typedef struct
{
	struct
	{
		//メニュー用アイテムをメモリから動的確保する場合に設定
		void* (*malloc_p)(SceSize size) ;
		void  (*free_p) (void *ptr) ;
	}func;
	
	struct
	{
		//メニュー用アイテム確保をスタック上の変数にする場合に設定
		MenuItem *val;
		int size;
	} list;
	
	//メニュー用アイテムの確保方法
	optType type;	//LIBM_OPT_LIST => スタック上の変数、LIBM_OPT_FUNC => 動的確保
	
	int count;
	
} libmOpt;



typedef struct MenuContext
{
	//メニュー操作用のKey
	struct
	{
		u32 				Show;			//表示
		u32					Back;			//閉じる、戻る
		u32					Select;			//選択
		u32					Up;				//上へ移動
		u32					Down;			//下へ移動
		u32					Left;			//左へ移動
		u32					Right;			//右へ移動
	}HotKey;
	
	struct
	{
		u32					Active_Font;	//アクティブ アイテムのフォント色
		u32					Active_Back;	//アクティブ アイテムの背景色
		
		u32					Normal_Font;	//非アクティブ アイテムのフォント色
		u32					Normal_Back;	//非アクティブ アイテムの背景色
		
		u32					Disable_Font;	//無効なアイテムのフォント色
		u32					Disable_Back;	//無効なアイテムの背景色
	}Color;
	
	//メニュー表示に関する設定
	struct
	{
		bool				Type;			//メニューのタイプ ( true = アクティブな項目のみ , false = 全体 )
		bool				AutoReturn;		//メニュー最上部・下部で自動ターンするかどうか
		u32					Lines;			//行間（デフォルト = 1、0だと下線描画が無効に）
	}MenuInfo;
	
	MenuParams				Params;
	
	bool 					IsOpen;			//メニューが表示中かどうか(表示、非表示）
	SceCtrlData				LastState;		//連続動作防止用、前パッド情報を保持
	MenuItem				*Root;			//rootアイテムへのポインター
	MenuItem				*Active;		//現在アクティブなアイテムへのポインタ
	
	libmOpt *opt;
	
} MenuContext;




typedef struct
{
	void	*buffer;	//描画対象の基準VRAMアドレス
	
	int	width;			//画面幅
	int	height;			//画面高
	
	int	format;			//ピクセルフォーマット
	int	lineWidth;		//画面幅（描画に使われている実際の幅）
	
	int	frameSize;		//1画面あたりのサイズ(byte)
	int	lineSize;		//1ラインあたりのサイズ(byte)
	u8	pixelSize;		//ピクセルサイズ
	int opt;
	
} libm_vram_info;


/*	#########################################################
	#					メニュー関連						#
	#########################################################
*/


/*	libmSetActiveItem
    メニュー上でアクティブなアイテムを設定する
    
    @param: MenuContext *Context
    対象のメニューコンテキスト
    
    @param: MenuItem *Item
    アクティブ指定するアイテムを指定
 */
#define	libmSetActive(Context,Item)		(Context)->Active = Item

/*	libmGetActiveItem
    メニュー上でアクティブなアイテムを取得する
    
    @param: MenuContext *Context
    対象のメニュー コンテキスト
    
    @return: 現在アクティブなアイテム (MenuItem*)
 */
#define	libmGetActive(Context)			(Context)->Active


/*	libmIsOpen
    メニューの表示/非表示 状態を取得
    
    @param: MenuContext *Context
    対象のメニュー コンテキスト
    
    @return : 表示/非表示の状態
    false = 非表示、true = 表示
 */
#define libmIsOpen(Context)				(Context)->IsOpen



#define	libmSetLineColor(item,color)	(item)->Color.Line = color

/*	libmIsInvalidItem
    アイテムが選択不可 (スペーサー、非表示、無効）な物かを調べる
    
    @param: MenuItem *Item
    対象のアイテム
    
    @return : false = 選択不可アイテム、true = 選択可能アイテム
 */
bool libmIsInvalidItem(MenuItem *Item);

/*	libmGetPrevItem
    基準より前のアイテムを検索する
    
    @param: MenuItem *Item
    検索の基準とするアイテム
    
    @param: bool Invalid_Skip
    無効なアイテム (スペーサー、非表示、無効）を除外して検索するどうか
    
    @return : 有効なアイテム、なければ NULL
 */
MenuItem* libmGetPrevItem( MenuItem *Item , bool Invalid_Skip );


/*	libmGetNextItem
    基準より後のアイテムを検索する
    
    @param: MenuItem *Item
    検索の基準とするアイテム
    
    @param: bool Invalid_Skip
    無効なアイテム (スペーサー、非表示、無効）を除外して検索するどうか
    
    @return : 有効なアイテム、なければ NULL
 */
MenuItem* libmGetNextItem( MenuItem *Item , bool Invalid_Skip );



/*	libmGetBottomItem
    基準から検索して一番最後にあるアイテムを調べる
    
    @param: MenuItem *Item
    検索の基準とするアイテム
    
    @param: bool Invalid_Skip
    無効なアイテム (スペーサー、非表示、無効）を除外して検索するどうか
    
    @return : 有効なアイテム、なければ NULL
 */
MenuItem* libmGetBottomItem( MenuItem *Item , bool Invalid_Skip );
	

/*	libmGetTopItem
    基準から検索して一番最初にあるアイテムを調べる
    
    @param: MenuItem *Item
    検索の基準とするアイテム
    
    @param: bool Invalid_Skip
    無効なアイテム (スペーサー、非表示、無効）を除外して検索するどうか
    
    @return : 有効なアイテム、なければ NULL
 */
MenuItem* libmGetTopItem( MenuItem *Item , bool Invalid_Skip );

/*	libmSetOpt
    メニュー context、item 作成時のオプションを指定
    
    @param: MenuContext* Context
    対象のメニューコンテキスト
    
    @param: libmOpt *opt
	
	・アイテム用のメモリを変数から確保する場合はこのような感じで設定
	opt.type = LIBM_OPT_LIST;
	opt.list.val	= item_list;			( item_list は MenuItem* の配列 )
	opt.list.size 	= sizeof(item_list);	(opt.list.valで指定した配列サイズ）
	libmSetOpt(&opt);
	
	・アイテム用のメモリを動的確保する場合はこのような感じで設定
	
	
	opt.type = LIBM_OPT_FUNC;
	opt.func.malloc_p	= メモリ確保関数;
	opt.func.free_p		= メモリ解放関数;
	libmSetOpt(&opt);
	
    
    @return : 設定出来たかどうか
    false = 失敗、true = 成功
 */

bool libmSetOpt(MenuContext* Context , libmOpt *opt );




/*	libmCreateContext
    メニュー コンテキストを作成
    
    @param: libmOpt *opt
    対象のメニューオプション
    
    @return: 作成したメニューコンテキスト
    
    作成に成功すると以下の値が初期値として自動設定される
    
	context->HotKey.Show			= PSP_CTRL_NOTE ;
	context->HotKey.Back			= PSP_CTRL_CIRCLE;
	context->HotKey.Select			= PSP_CTRL_CROSS;
	context->HotKey.Up				= PSP_CTRL_UP;
	context->HotKey.Down			= PSP_CTRL_DOWN;
	context->HotKey.Left			= PSP_CTRL_LEFT;
	context->HotKey.Right			= PSP_CTRL_RIGHT;
	
	context->MenuInfo.Type			= false;
	
	context->MenuInfo.AutoReturn	= true;
	
	context->Color.Active_Font		= GREEN;
	context->Color.Normal_Font		= WHITE;
	
	context->Color.Active_Back		= BLACK;
	context->Color.Normal_Back		= Invalid;
	
	context->Color.Disable_Font		= GRAY;
	context->Color.Disable_Back		= Invalid;
	
	context->MenuInfo.Lines		 	= 1;
 */
MenuContext* libmCreateContext(libmOpt *opt);

/*	libmRemoveContext
    メニュー コンテキストを削除（メモリ解放）する
    
    @param: MenuContext* Context
    削除するメニューコンテキスト
    
    ※libmSetOptでメモリ動的確保する設定の場合のみメモリ解放
    
 */
void libmRemoveContext(MenuContext* Context);

/*  libmAddItem
    メニュー コンテキストにアイテムを追加する
    
    @param: MenuContext* Context
    対象のメニューコンテキスト
    
    @param: MenuItem* Parent
    アイテム追加対象の親(アイテム）
	
    @param: MenuItem* Item
    追加するアイテム
    
    @param: u32 font_color
    追加するアイテムのフォント色
    0 を指定すると描画しない
    
    @param: u32 back_color
    追加するアイテムの背景色
    0 を指定すると描画しない
    
    @param: int actionID
    アイテムが操作された際、区別するためのID(アクション）
    
    @param: int Data
    アイテムが操作された際、区別するためのID(アイテム固有）
    
    @return: 作成されたアイテム
 */
MenuItem* libmAddItem(MenuContext* Context, MenuItem* Parent, MenuItem* Item,u32 font_color,u32 back_color, int actionID, int Data);

/*	libmRemoveItem
    アイテムを削除（メモリ解放）する
    
    @param: MenuContext* Context
    対象のメニューコンテキスト
    
    @param: MenuItem* Item
    対象のアイテム
    
    ※libmSetOptでメモリ動的確保する設定の場合のみメモリ解放
    　
    　対象がコンテナーだった場合は、
    　中にある全アイテムも同時削除
 */
void libmRemoveItem( MenuContext *context , MenuItem* Item );

/*	libmGetHandle
    メニュー上の動作を取得する（描画はされない）
    
    @param: MenuContext* Context
    対象のメニューコンテキスト
    
    @param: SceCtrlData* Input
    Key入力( SceCtrlData )
    
    @return: MenuParams (選択されたアイテム、メニューの状態など）
 */

MenuParams* libmGetHandle(MenuContext* Context, SceCtrlData* Input);

/*  libmRender
    画面へメニューを描画する
    
    @param: MenuContext* Context
    対象のメニュー context
    
    @param:  int PosX
    メニューの表示位置 X
    
    @param: int PosY
    メニューの表示位置 Y
    
    
    メニュー表示時に他スレッド停止を有効にしている場合
    表示(他スレッド停止)中に HOMEボタン が押されると
    ゲーム終了を可能にするために自動で他スレッド再開 + メニューを閉じる
 */
void libmRender(MenuContext* Context,int PosX,int PosY ,char *buf ,int bufLen);




/*  libmCreateContainer
    アイテム作成（コンテナー）
    
    @param: libmOpt *opt
    対象のメニューオプション
    
    @param: const char* Name
    コンテナ名
    
    @return : 作成されたアイテム
 */
MenuItem* libmCreateContainer(libmOpt *opt ,const char* Name);

/*  libmCreateRadioButton
    アイテム作成（ラジオボタン）
    
    @param: libmOpt *opt
    対象のメニューオプション
    
    @param: const char* Name
    ラジオボタン名
    
    @param: bool State
    選択状態
    
    @param: MenuItem * Group
  　関連付けるラジオボタン グループ
  　libmCreateRadioButtonGroupで作成
  　指定必須
  　
	@return : 作成されたアイテム
	
	libmAddItemで実際に追加する時は、
	Parent(親)がコンテナー または NULLでないと無視される
 */
MenuItem* libmCreateRadioButton(libmOpt *opt ,const char* Name, bool State,MenuItem * Group);

/*  libmCreateRadioButtonGroup
    アイテムを作成（ラジオボタン グループ)
    
    @param: libmOpt *opt
    対象のメニューオプション
    
    @return : 作成されたアイテム
 */
MenuItem* libmCreateRadioButtonGroup(libmOpt *opt);



/*  libmCreateCheckBox
    アイテム作成（チェックボックス）
    
    @param: libmOpt *opt
    対象のメニューオプション
    
    @param: const char* Name
    チェックボックス名
    
    @param: bool State
    選択状態
    
	@return : 作成されたアイテム
 */
MenuItem* libmCreateCheckBox(libmOpt *opt ,const char* Name, bool State);

/*  libmCreateSelectBox
    アイテム作成（チェックボックス）
    
    @param: libmOpt *opt
    対象のメニューオプション
    
    @param: const char* Name
    セレクトボックス名
    
    @param: const char *List[]
    セレクトボックスに表示するアイテムリスト
    const char *の配列、最後にNULLが必要
    
    @param: int Selected
    リストから最初に選択済みとする位置
    
    @return : 作成されたアイテム
 */
MenuItem* libmCreateSelectBox(libmOpt *opt ,const char* Name, const char *List[],int Selected);

/*  libmCreateUpDownCtrl
    アイテムを作成（アップダウンコントロール）
    
    @param: libmOpt *opt
    対象のメニューオプション
    
    @param: const char* Name
    アップダウンコントロール名
    
    @param: float Now
    現在値
    
    @param: float Step
    増減値
    
    @param: float Min
    最小値
    
    @param: float Max
    最大値
    
    @param: bool type
    false = 描画時に現在値を小数点表示させる ( 00.00 のような感じ）
    true = 描画時に現在値を整数表示する
    
    @return : 作成されたアイテム
 */
MenuItem* libmCreateUpDownCtrl(libmOpt *opt ,const char* Name,float Now,float Step,float Min, float Max,bool type);

/*  libmCreateSpacer
    アイテムを作成（スペーサー）
    
    @param: libmOpt *opt
    対象のメニューオプション
    
    @param: const char* Name
    スペーサー名
    
    @return : 作成されたアイテム
 */
MenuItem* libmCreateSpacer(libmOpt *opt ,const char* Name);

/*  libmCreateTriggerButton
    アイテムを作成（トリガーボタン)
    
    @param: libmOpt *opt
    対象のメニューオプション
    
	@param: const char* Name
    トリガーボタン名
    
    @return : 作成されたアイテム
 */
MenuItem* libmCreateTriggerButton(libmOpt *opt ,const char* Name);



/*	#############################################################
	#							描画							#
	#############################################################
*/




/*  libmInitBuffers
    現在のディスプレイの状態に合わせて描画準備をする
    
    @param: int opt
	準備する際に設定する描画オプション
	以下から有効にしたい物を設定
	
	LIBM_DRAW_INIT8888		ピクセルフォーマット8888で初期化
	LIBM_DRAW_INIT4444		ピクセルフォーマット4444で初期化
	LIBM_DRAW_INIT5650		ピクセルフォーマット5650で初期化
	LIBM_DRAW_INIT5551		ピクセルフォーマット5551で初期化
	LIBM_DRAW_BLEND			背景透過を有効にする
	LIBM_DRAW_RETURN		文字が描画位置が画面外だった場合は折り返して表示させる
	
	LIBM_DRAW_INIT			ピクセルフォーマット8888、文字は自動折り返しで画面を初期化する
							主に自作アプリでlibmenuの文字描画関数を pspDebugScreenXXXX の様に使えるようにする
    
    @param: int sync
    同期対象
    
	PSP_DISPLAY_SETBUF_IMMEDIATE
	PSP_DISPLAY_SETBUF_NEXTFRAME
	のどちらかを指定
	
	※opt に LIBM_DRAW_INITを指定した場合は、sync に PSP_DISPLAY_SETBUF_NEXTFRAME を指定
    
    @return: true = 成功、false = 失敗
 */
bool libmInitBuffers( int opt ,int sync );



/*  libmSwapBuffers
    libmRender で描画に使うバッファーの変更
    
	メニュー表示時にスレッド停止するように設定した場合
	これを使うとダブルバッファリングのようなことが出来るが
	実装は適当なので変な動きをするかもしれない
	
	※事前に libmInitBuffers を実行してないと正常動作しない
 */
void libmSwapBuffers();

/*  libmClearBuffers
    libmRender で描画に使うバッファーをクリア（黒色）
    
	※事前に libmInitBuffers を実行してないと正常動作しない
 */
void libmClearBuffers();


/*  libmPrintXY
	
	libmenuの内部フォントを使って 文字列 を描画する（座標指定あり）
	
	@params : int x
	表示位置 X
	
	@params : int y
	表示位置 Y
	
	@params : unsigned int fg
	フォント色(32bit)
	0 だと描画しない
	
	@params : unsigned int bg
	背景色(32bit)
	0 だと描画しない
	
	@params : const char *str
    描画対象 文字列
    
    @return: 描画した文字数
    
	※事前に libmInitBuffers を実行してないと正常動作しない
 */
inline int libmPrintXY( int x, int y, u32 fg, u32 bg, const char *str );



/*  libmPrintfXY
	
	libmenuの内部フォントを使って 書式付き文字列 を描画する（座標指定あり）
	
	@params : int x
	表示位置 X
	
	@params : int y
	表示位置 Y
	
	@params : unsigned int fg
	フォント色(32bit)
	0 だと描画しない
	
	@params : unsigned int bg
	背景色(32bit)
	0 だと描画しない
	
	@params : char *buf
	書式から文字列を生成する際に使われるバッファー
	
	@params : int bufLen
	バッファーサイズ（char *buf）
	
	@params : format, ...
    描画対象 書式付き 文字列
    
    @return: 描画した文字数
    
	※事前に libmInitBuffers を実行してないと正常動作しない
 */
inline int libmPrintfXY( int x, int y, u32 fg, u32 bg, char *buf ,int bufLen ,const char *format, ... );



/*  libmPutCharXY
	
	libmenuの内部フォントを使って 文字 を描画する（座標指定あり）
	
	@params : int x
	表示位置 X
	
	@params : int y
	表示位置 Y
	
	@params : unsigned int fg
	フォント色(32bit)
	0 だと描画しない
	
	@params : unsigned int bg
	背景色(32bit)
	0 だと描画しない
	
	@params : const char *str
    描画対象 文字
    
    @return: 描画した文字数
    
	※事前に libmInitBuffers を実行してないと正常動作しない
 */
inline int libmPutCharXY( int x, int y, u32 fg, u32 bg, const char chr );




/*  libmDebugScreenInit
	
	画面に描画する準備をする
	pspDebugScreenInit と似たような動作
 */
#define	libmDebugScreenInit()		libmInitBuffers(LIBM_DRAW_INIT,PSP_DISPLAY_SETBUF_NEXTFRAME)




/*  libmDebugScreenPrint
	
	libmenuの内部フォントを使って 文字列 を描画する（座標指定なし）
	pspDebugScreenPrintf と似たような動作
	
	@params : unsigned int fg
	フォント色(32bit)
	0 だと描画しない
	
	@params : unsigned int bg
	背景色(32bit)
	0 だと描画しない
	
	@params : const char *str
    描画対象 文字列
    
    @return: 描画した文字数
    
	※事前に libmInitBuffers または libmDebugScreenInit を実行してないと正常動作しない
 */
#define	libmDebugScreenPrint(fg,bg,str)							libmPrintXY(-1,-1,fg,bg,str)



/*  libmDebugScreenPrintf
	
	libmenuの内部フォントを使って 書式付き文字列 を描画する（座標指定なし）
	pspDebugScreenPrintf と似たような動作
	
	@params : unsigned int fg
	フォント色(32bit)
	0 だと描画しない
	
	@params : unsigned int bg
	背景色(32bit)
	0 だと描画しない
	
	@params : char *buf
	書式から文字列を生成する際に使われるバッファー
	
	@params : int bufLen
	バッファーサイズ（char *buf）
	
	@params : format, ...
    描画対象 書式付き 文字列
    
    @return: 描画した文字数
    
	※事前に libmInitBuffers または libmDebugScreenInit を実行してないと正常動作しない
 */
#define	libmDebugScreenPrintf(fg,bg,buf,bufLen,format,...)		libmPrintfXY(-1,-1,fg,bg,buf,bufLen,format, __VA_ARGS__)


/*  libmDebugScreenPutChar
	
	libmenuの内部フォントを使って 文字 を描画する（座標指定なし）
	pspDebugScreenPrintf と似たような動作
	
	@params : unsigned int fg
	フォント色(32bit)
	0 だと描画しない
	
	@params : unsigned int bg
	背景色(32bit)
	0 だと描画しない
	
	@params : const char *str
    描画対象 文字
    
    @return: 描画した文字数
    
	※事前に libmInitBuffers または libmDebugScreenInit を実行してないと正常動作しない
 */
#define	libmDebugScreenPutChar(fg,bg,chr)							libmPrintfXY(-1,-1,fg,bg,chr)


/*  libmDebugScreenSetXY
	
	座標指定なしの文字/文字列描画関数で
	描画を開始する位置を指定する
	
	@params : int x
	描画開始位置X　(0 - 59)
	1文字 8x8 ドットなので1行最大60文字まで
	
	@params : int y
	描画開始位置Y　(0 - 33)
	1文字 8x8 ドットなので1列最大34文字まで
    
    @return: true = 成功、false = 失敗（X/Yどちらかの値が異常）
 */
bool libmDebugScreenSetXY( int x ,int y );


/*	libmLine
	画面に線を描画する
	
	@params : int sx
	描画開始位置X
	
	@params : int sy
	描画開始位置Y
	
	@params : int ex
	描画終了位置X
	
	@params : int ey
	描画終了位置Y
    
	@params : u32 color
	描画色(32bit)
	
	※事前に libmInitBuffers を実行してないと正常動作しない
*/
inline void libmLine( int sx, int sy, int ex, int ey, u32 color );


/*	libmPoint
	画面に点を描画する
	※指定した32bit色を現在の画面設定に合わせて自動変換して
	
	@params : void *adr
	描画するVRAMアドレス
	(libmMakeDrawAddrで座標をアドレスへ変換)
	
	@params : u32 color
	描画色(32bit)
    
    使用例
    
    if( libmInitBuffers(false,PSP_DISPLAY_SETBUF_NEXTFRAME) )
    {
 		//X=50,Y=100の位置に赤い点を描画する
 		void *adr = libmMakeDrawAddr(50,100);
 		u32 color = libmMake32bitColor(255,0,0,255);
    	libmPoint(adr,color);
    }
    
	※事前に libmInitBuffers を実行してないと正常動作しない
*/
inline void libmPoint( void *adr, u32 src );


/*	libmPointEx
	画面に点を描画する
	※画面設定に合わせて変換済みの色を指定
	
	@params : void *adr
	描画するVRAMアドレス
	(libmMakeDrawAddrで座標をアドレスへ変換)
	
	@params : u32 color
	画面設定に合わせて変換済みの描画色
	
    
	※事前に libmInitBuffers を実行してないと正常動作しない
*/
inline void libmPointEx( void *adr, u32 src );

/*	libmFillRect
	指定範囲を指定色で塗りつぶす
	
	@params : int sx
	開始位置X (0-480)
	
	@params : int sy
	開始位置Y (0-272)
	
	@params : int ex
	終了位置X (0-480)
	
	@params : int ey
	終了位置Y (0-272)
	
	@params : u32 color
	描画色(32bit)
	
    
	※事前に libmInitBuffers を実行してないと正常動作しない
*/
inline void libmFillRect( int sx, int sy, int ex, int ey, u32 color );



/*	libmCircle
	画面に円を描画する
	
	@params : int x
	開始位置X (0-480)
	
	@params : int y
	開始位置Y (0-272)
	
	@params : u32 radius
	円の半径
	
	@params : u32 color
	描画色(32bit)
	
	※事前に libmInitBuffers を実行してないと正常動作しない
*/
inline void libmCircle( int x, int y, u32 radius, u32 color );



/*	libmFrame
	画面に枠を描画する
	
	@params : int sx
	開始位置X (0-480)
	
	@params : int sy
	開始位置Y (0-272)
	
	@params : int ex
	終了位置X (0-480)
	
	@params : int ey
	終了位置Y (0-272)
	
	@params : u32 color
	描画色(32bit)
	
	※事前に libmInitBuffers を実行してないと正常動作しない
*/
inline void libmFrame( int sx, int sy, int ex, int ey, u32 color );


/*	libmMakeDrawAddr
	座標を現在の画面設定に合わせてVRAMアドレスに変換する
	
	@params : int x
	描画位置X (0-480)
	
	@params : int y
	描画位置Y (0-272)
	
	※事前に libmInitBuffers を実行してないと正常動作しない
*/
inline void* libmMakeDrawAddr( int x, int y );

/*	libmConvert8888_XXXX
	32bit色(R:8 G:8 B:8 A:8)を各フォーマットの色へ変換する
	
	@params : u32 src
	変換元の色
	
    @return: 指定フォーマットへ変換された色
*/
inline u32 libmConvert8888_5650( u32 src );
inline u32 libmConvert8888_5551( u32 src );
inline u32 libmConvert8888_4444( u32 src );

/*	libmConvertXXXX_8888
	各フォーマットの色を32bit色(R:8 G:8 B:8 A:8)へ変換する
	
	@params : u32 src
	変換元の色
	
    @return: 変換された32bit色(8888フォーマット)
*/
inline u32 libmConvert4444_8888(u32 src);
inline u32 libmConvert5551_8888(u32 src);
inline u32 libmConvert5650_8888(u32 src);




/*	libmAlphaBlendXXXX
	各フォーマットの色(src、dst)にαブレンドする
	
	@params : u8 alpha
	ブレンドするアルファ値 (0-255)
	
	@params : u32 src
	ブレンドする色（元）
	※ピクセルフォーマットに合わせて変換済みの色
	
	@params : u32 dst
	ブレンドする色（先）
	※ピクセルフォーマットに合わせて変換済みの色
	
    @return: αブレンドされた色
*/
inline u32 libmAlphaBlend8888( u8 alpha, u32 src, u32 dst );
inline u32 libmAlphaBlend4444( u8 alpha, u32 src, u32 dst );
inline u32 libmAlphaBlend5551( u8 alpha, u32 src, u32 dst );
inline u32 libmAlphaBlend5650( u8 alpha, u32 src, u32 dst );



/*	libmMake32bitColor
	RGBAから32bit(8888フォーマット)の色を生成する
	
	@params : u8 R
	Color Red (0-255)
	
	@params : u8 G
	Color Green (0-255)
	
	@params : u8 B
	Color Blue (0-255)
	
	@params : u8 A
	Alpha (0-255)
	
	@return : 8888フォーマットの32bit色
*/
#define	libmMake32bitColor(R,G,B,A)	 ((R & 255) | ((G & 255) << 8) | ((B & 255) << 16) | ((A & 255) << 24))


/*	libmGetColor
	指定した座標XYから描画色を取得
	
	@params : int x
	座標X(0-480)
	
	@params : int y
	座標X(0-272)
	
	@return : 指定した座標から得た色
	
	※事前に libmInitBuffers を実行してないと正常動作しない
*/
inline u32 libmGetColor(void *addr);


/*	libmGetCurVInfo
	libmenu 各描画関数で使われている現在の各設定を取得
	※取得出来る情報については libm_vram_info の定義部分を参照
	
	@params : libm_vram_info *info
	取得先の変数
	
	@return : true = 成功、false = 失敗
	
	※事前に libmInitBuffers を実行してないと正常動作しない
*/
bool libmGetCurVInfo(libm_vram_info *info);


/*	libmSetCurVInfo
	libmenu 各描画関数で使われている現在の設定を変更する
	
	@params : format
	ピクセルフォーマット
	
	@params : buf
	描画先の基準VRAMアドレス
	
	@params : width
	描画幅
*/
void libmSetCurVInfo(int format, void *buf, int width);


#ifdef __cplusplus
}
#endif

#endif
