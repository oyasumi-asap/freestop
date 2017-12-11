#ifndef LIBMENU_H
#define LIBMENU_H

#include <stdbool.h>
#include <pspctrl.h>


#ifdef __cplusplus
extern "C" {
#endif



// libmInitBuffers �ő�1�����Ɏw��o����VRAM�������I�v�V����
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

//libmDebugScreenSetXY�Ŏw��\��X,Y�̍ő�l
#define	LIBM_SETX_MAX			59
#define	LIBM_SETY_MAX			33

//libmenu�����t�H���g�̏c���T�C�Y
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



// libmGetHandle�����s��� Action ���Q�Ƃ��ē����郁�j���[�̏��
typedef enum
{
	Menu_None = 0,	//�����Ȃ�
	Menu_Show,		//���j���[���\�����ꂽ
	Menu_Up,		//�A�C�e���I����Ԃ� "��" �Ɏw�肳��Ă���{�^���������ꂽ
	Menu_Down,		//�A�C�e���I����Ԃ� "��" �Ɏw�肳��Ă���{�^���������ꂽ
	Menu_Left,		//�A�C�e���I����Ԃ� "��" �Ɏw�肳��Ă���{�^���������ꂽ
	Menu_Right,		//�A�C�e���I����Ԃ� "�E" �Ɏw�肳��Ă���{�^���������ꂽ
	Menu_Select,	//�A�C�e���I����Ԃ� "�I��" �Ɏw�肳��Ă���{�^���������ꂽ
	Menu_Back,		//�A�C�e���I����Ԃ� "�߂�" �Ɏw�肳��Ă���{�^���������ꂽ
	Menu_Close		//���j���[������ꂽ
} MenuResult;


// libmOpt�Atype�֎w��\�ȃ^�C�v�ꗗ
typedef enum
{
	LIBM_OPT_NONE,	//����
	LIBM_OPT_LIST,	//�v���O������̕ϐ����烁�j���[�쐬
	LIBM_OPT_FUNC,	//����������ϐ��𓮓I�m�ۂ��ă��j���[�쐬
} optType;



typedef struct MenuItem
{
	struct MenuItem			*Parent;		//�A�C�e���̐e ( NULL = Root )
	struct MenuItem			*Next;			//�A�C�e���̎�
	struct MenuItem			*Prev;			//�A�C�e���̑O
	struct MenuItem			*Children;		//�A�C�e���̎q
	
	
	const char				*Name;			//�A�C�e�����i�\�������j
	int						Type;			//�A�C�e���̃^�C�v
	int						actionID;		//�A�C�e�����I�����ꂽ�肵�����ɁAhandleMenu�̕ԋp�l�֑���actionID
	int						Data;			//�A�C�e���l
	
	bool					Visible;		//�A�C�e���̉����( ture ,false )
	bool					Disable;		//�A�C�e���̗L���E���� ���
	
	
	//��A�N�e�B�u���̃A�C�e���`��F
	//�w�肪�Ȃ���� Context �̒ʏ�F
	
	struct
	{
		u32 				Font;			//�t�H���g
		u32 				Back;			//�w�i
		u32					Line;			//����
	}Color;
	
	
	union
	{
		//���W�I�{�^���p
		struct MenuItem		*RB_Group;		//�O���[�v��ւ̃|�C���^�[
		
		//���W�I�{�^���E�O���[�v�p
		struct MenuItem		*Group_Sel;		//�I�𒆃A�C�e���ւ̃|�C���^�[
		
		//�R���e�i�[�p
		struct
		{
			bool			IsOpen;			//�J���
			struct MenuItem	*Disp;			//�֘A�t���ď�ԕ\��������A�C�e���ւ̃|�C���^�[
		}CNT;
		
		//�`�F�b�N�{�b�N�X�p
		bool				CB_Checked; 	//�I�����
		
		//���X�g�{�b�N�X�p
		struct
		{
			const char		**List;			//���X�g�ւ̃|�C���^�[
			int				Index;			//�I�𒆂�index
		}SB;
		
		//�A�b�v�_�E���p
		struct
		{
			float			Now;			//���ݒn
			float			Step;			//Step�l (+/-)
			float			Min;			//�ŏ��l
			float			Max;			//�ő�l
			bool			type;			//�\���^�C�v ( true = ���� , false = ���� )
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
		//���j���[�p�A�C�e�������������瓮�I�m�ۂ���ꍇ�ɐݒ�
		void* (*malloc_p)(SceSize size) ;
		void  (*free_p) (void *ptr) ;
	}func;
	
	struct
	{
		//���j���[�p�A�C�e���m�ۂ��X�^�b�N��̕ϐ��ɂ���ꍇ�ɐݒ�
		MenuItem *val;
		int size;
	} list;
	
	//���j���[�p�A�C�e���̊m�ە��@
	optType type;	//LIBM_OPT_LIST => �X�^�b�N��̕ϐ��ALIBM_OPT_FUNC => ���I�m��
	
	int count;
	
} libmOpt;



typedef struct MenuContext
{
	//���j���[����p��Key
	struct
	{
		u32 				Show;			//�\��
		u32					Back;			//����A�߂�
		u32					Select;			//�I��
		u32					Up;				//��ֈړ�
		u32					Down;			//���ֈړ�
		u32					Left;			//���ֈړ�
		u32					Right;			//�E�ֈړ�
	}HotKey;
	
	struct
	{
		u32					Active_Font;	//�A�N�e�B�u �A�C�e���̃t�H���g�F
		u32					Active_Back;	//�A�N�e�B�u �A�C�e���̔w�i�F
		
		u32					Normal_Font;	//��A�N�e�B�u �A�C�e���̃t�H���g�F
		u32					Normal_Back;	//��A�N�e�B�u �A�C�e���̔w�i�F
		
		u32					Disable_Font;	//�����ȃA�C�e���̃t�H���g�F
		u32					Disable_Back;	//�����ȃA�C�e���̔w�i�F
	}Color;
	
	//���j���[�\���Ɋւ���ݒ�
	struct
	{
		bool				Type;			//���j���[�̃^�C�v ( true = �A�N�e�B�u�ȍ��ڂ̂� , false = �S�� )
		bool				AutoReturn;		//���j���[�ŏ㕔�E�����Ŏ����^�[�����邩�ǂ���
		u32					Lines;			//�s�ԁi�f�t�H���g = 1�A0���Ɖ����`�悪�����Ɂj
	}MenuInfo;
	
	MenuParams				Params;
	
	bool 					IsOpen;			//���j���[���\�������ǂ���(�\���A��\���j
	SceCtrlData				LastState;		//�A������h�~�p�A�O�p�b�h����ێ�
	MenuItem				*Root;			//root�A�C�e���ւ̃|�C���^�[
	MenuItem				*Active;		//���݃A�N�e�B�u�ȃA�C�e���ւ̃|�C���^
	
	libmOpt *opt;
	
} MenuContext;




typedef struct
{
	void	*buffer;	//�`��Ώۂ̊VRAM�A�h���X
	
	int	width;			//��ʕ�
	int	height;			//��ʍ�
	
	int	format;			//�s�N�Z���t�H�[�}�b�g
	int	lineWidth;		//��ʕ��i�`��Ɏg���Ă�����ۂ̕��j
	
	int	frameSize;		//1��ʂ�����̃T�C�Y(byte)
	int	lineSize;		//1���C��������̃T�C�Y(byte)
	u8	pixelSize;		//�s�N�Z���T�C�Y
	int opt;
	
} libm_vram_info;


/*	#########################################################
	#					���j���[�֘A						#
	#########################################################
*/


/*	libmSetActiveItem
    ���j���[��ŃA�N�e�B�u�ȃA�C�e����ݒ肷��
    
    @param: MenuContext *Context
    �Ώۂ̃��j���[�R���e�L�X�g
    
    @param: MenuItem *Item
    �A�N�e�B�u�w�肷��A�C�e�����w��
 */
#define	libmSetActive(Context,Item)		(Context)->Active = Item

/*	libmGetActiveItem
    ���j���[��ŃA�N�e�B�u�ȃA�C�e�����擾����
    
    @param: MenuContext *Context
    �Ώۂ̃��j���[ �R���e�L�X�g
    
    @return: ���݃A�N�e�B�u�ȃA�C�e�� (MenuItem*)
 */
#define	libmGetActive(Context)			(Context)->Active


/*	libmIsOpen
    ���j���[�̕\��/��\�� ��Ԃ��擾
    
    @param: MenuContext *Context
    �Ώۂ̃��j���[ �R���e�L�X�g
    
    @return : �\��/��\���̏��
    false = ��\���Atrue = �\��
 */
#define libmIsOpen(Context)				(Context)->IsOpen



#define	libmSetLineColor(item,color)	(item)->Color.Line = color

/*	libmIsInvalidItem
    �A�C�e�����I��s�� (�X�y�[�T�[�A��\���A�����j�ȕ����𒲂ׂ�
    
    @param: MenuItem *Item
    �Ώۂ̃A�C�e��
    
    @return : false = �I��s�A�C�e���Atrue = �I���\�A�C�e��
 */
bool libmIsInvalidItem(MenuItem *Item);

/*	libmGetPrevItem
    ����O�̃A�C�e������������
    
    @param: MenuItem *Item
    �����̊�Ƃ���A�C�e��
    
    @param: bool Invalid_Skip
    �����ȃA�C�e�� (�X�y�[�T�[�A��\���A�����j�����O���Č�������ǂ���
    
    @return : �L���ȃA�C�e���A�Ȃ���� NULL
 */
MenuItem* libmGetPrevItem( MenuItem *Item , bool Invalid_Skip );


/*	libmGetNextItem
    �����̃A�C�e������������
    
    @param: MenuItem *Item
    �����̊�Ƃ���A�C�e��
    
    @param: bool Invalid_Skip
    �����ȃA�C�e�� (�X�y�[�T�[�A��\���A�����j�����O���Č�������ǂ���
    
    @return : �L���ȃA�C�e���A�Ȃ���� NULL
 */
MenuItem* libmGetNextItem( MenuItem *Item , bool Invalid_Skip );



/*	libmGetBottomItem
    ����猟�����Ĉ�ԍŌ�ɂ���A�C�e���𒲂ׂ�
    
    @param: MenuItem *Item
    �����̊�Ƃ���A�C�e��
    
    @param: bool Invalid_Skip
    �����ȃA�C�e�� (�X�y�[�T�[�A��\���A�����j�����O���Č�������ǂ���
    
    @return : �L���ȃA�C�e���A�Ȃ���� NULL
 */
MenuItem* libmGetBottomItem( MenuItem *Item , bool Invalid_Skip );
	

/*	libmGetTopItem
    ����猟�����Ĉ�ԍŏ��ɂ���A�C�e���𒲂ׂ�
    
    @param: MenuItem *Item
    �����̊�Ƃ���A�C�e��
    
    @param: bool Invalid_Skip
    �����ȃA�C�e�� (�X�y�[�T�[�A��\���A�����j�����O���Č�������ǂ���
    
    @return : �L���ȃA�C�e���A�Ȃ���� NULL
 */
MenuItem* libmGetTopItem( MenuItem *Item , bool Invalid_Skip );

/*	libmSetOpt
    ���j���[ context�Aitem �쐬���̃I�v�V�������w��
    
    @param: MenuContext* Context
    �Ώۂ̃��j���[�R���e�L�X�g
    
    @param: libmOpt *opt
	
	�E�A�C�e���p�̃�������ϐ�����m�ۂ���ꍇ�͂��̂悤�Ȋ����Őݒ�
	opt.type = LIBM_OPT_LIST;
	opt.list.val	= item_list;			( item_list �� MenuItem* �̔z�� )
	opt.list.size 	= sizeof(item_list);	(opt.list.val�Ŏw�肵���z��T�C�Y�j
	libmSetOpt(&opt);
	
	�E�A�C�e���p�̃������𓮓I�m�ۂ���ꍇ�͂��̂悤�Ȋ����Őݒ�
	
	
	opt.type = LIBM_OPT_FUNC;
	opt.func.malloc_p	= �������m�ۊ֐�;
	opt.func.free_p		= ����������֐�;
	libmSetOpt(&opt);
	
    
    @return : �ݒ�o�������ǂ���
    false = ���s�Atrue = ����
 */

bool libmSetOpt(MenuContext* Context , libmOpt *opt );




/*	libmCreateContext
    ���j���[ �R���e�L�X�g���쐬
    
    @param: libmOpt *opt
    �Ώۂ̃��j���[�I�v�V����
    
    @return: �쐬�������j���[�R���e�L�X�g
    
    �쐬�ɐ�������ƈȉ��̒l�������l�Ƃ��Ď����ݒ肳���
    
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
    ���j���[ �R���e�L�X�g���폜�i����������j����
    
    @param: MenuContext* Context
    �폜���郁�j���[�R���e�L�X�g
    
    ��libmSetOpt�Ń��������I�m�ۂ���ݒ�̏ꍇ�̂݃��������
    
 */
void libmRemoveContext(MenuContext* Context);

/*  libmAddItem
    ���j���[ �R���e�L�X�g�ɃA�C�e����ǉ�����
    
    @param: MenuContext* Context
    �Ώۂ̃��j���[�R���e�L�X�g
    
    @param: MenuItem* Parent
    �A�C�e���ǉ��Ώۂ̐e(�A�C�e���j
	
    @param: MenuItem* Item
    �ǉ�����A�C�e��
    
    @param: u32 font_color
    �ǉ�����A�C�e���̃t�H���g�F
    0 ���w�肷��ƕ`�悵�Ȃ�
    
    @param: u32 back_color
    �ǉ�����A�C�e���̔w�i�F
    0 ���w�肷��ƕ`�悵�Ȃ�
    
    @param: int actionID
    �A�C�e�������삳�ꂽ�ہA��ʂ��邽�߂�ID(�A�N�V�����j
    
    @param: int Data
    �A�C�e�������삳�ꂽ�ہA��ʂ��邽�߂�ID(�A�C�e���ŗL�j
    
    @return: �쐬���ꂽ�A�C�e��
 */
MenuItem* libmAddItem(MenuContext* Context, MenuItem* Parent, MenuItem* Item,u32 font_color,u32 back_color, int actionID, int Data);

/*	libmRemoveItem
    �A�C�e�����폜�i����������j����
    
    @param: MenuContext* Context
    �Ώۂ̃��j���[�R���e�L�X�g
    
    @param: MenuItem* Item
    �Ώۂ̃A�C�e��
    
    ��libmSetOpt�Ń��������I�m�ۂ���ݒ�̏ꍇ�̂݃��������
    �@
    �@�Ώۂ��R���e�i�[�������ꍇ�́A
    �@���ɂ���S�A�C�e���������폜
 */
void libmRemoveItem( MenuContext *context , MenuItem* Item );

/*	libmGetHandle
    ���j���[��̓�����擾����i�`��͂���Ȃ��j
    
    @param: MenuContext* Context
    �Ώۂ̃��j���[�R���e�L�X�g
    
    @param: SceCtrlData* Input
    Key����( SceCtrlData )
    
    @return: MenuParams (�I�����ꂽ�A�C�e���A���j���[�̏�ԂȂǁj
 */

MenuParams* libmGetHandle(MenuContext* Context, SceCtrlData* Input);

/*  libmRender
    ��ʂփ��j���[��`�悷��
    
    @param: MenuContext* Context
    �Ώۂ̃��j���[ context
    
    @param:  int PosX
    ���j���[�̕\���ʒu X
    
    @param: int PosY
    ���j���[�̕\���ʒu Y
    
    
    ���j���[�\�����ɑ��X���b�h��~��L���ɂ��Ă���ꍇ
    �\��(���X���b�h��~)���� HOME�{�^�� ����������
    �Q�[���I�����\�ɂ��邽�߂Ɏ����ő��X���b�h�ĊJ + ���j���[�����
 */
void libmRender(MenuContext* Context,int PosX,int PosY ,char *buf ,int bufLen);




/*  libmCreateContainer
    �A�C�e���쐬�i�R���e�i�[�j
    
    @param: libmOpt *opt
    �Ώۂ̃��j���[�I�v�V����
    
    @param: const char* Name
    �R���e�i��
    
    @return : �쐬���ꂽ�A�C�e��
 */
MenuItem* libmCreateContainer(libmOpt *opt ,const char* Name);

/*  libmCreateRadioButton
    �A�C�e���쐬�i���W�I�{�^���j
    
    @param: libmOpt *opt
    �Ώۂ̃��j���[�I�v�V����
    
    @param: const char* Name
    ���W�I�{�^����
    
    @param: bool State
    �I�����
    
    @param: MenuItem * Group
  �@�֘A�t���郉�W�I�{�^�� �O���[�v
  �@libmCreateRadioButtonGroup�ō쐬
  �@�w��K�{
  �@
	@return : �쐬���ꂽ�A�C�e��
	
	libmAddItem�Ŏ��ۂɒǉ����鎞�́A
	Parent(�e)���R���e�i�[ �܂��� NULL�łȂ��Ɩ��������
 */
MenuItem* libmCreateRadioButton(libmOpt *opt ,const char* Name, bool State,MenuItem * Group);

/*  libmCreateRadioButtonGroup
    �A�C�e�����쐬�i���W�I�{�^�� �O���[�v)
    
    @param: libmOpt *opt
    �Ώۂ̃��j���[�I�v�V����
    
    @return : �쐬���ꂽ�A�C�e��
 */
MenuItem* libmCreateRadioButtonGroup(libmOpt *opt);



/*  libmCreateCheckBox
    �A�C�e���쐬�i�`�F�b�N�{�b�N�X�j
    
    @param: libmOpt *opt
    �Ώۂ̃��j���[�I�v�V����
    
    @param: const char* Name
    �`�F�b�N�{�b�N�X��
    
    @param: bool State
    �I�����
    
	@return : �쐬���ꂽ�A�C�e��
 */
MenuItem* libmCreateCheckBox(libmOpt *opt ,const char* Name, bool State);

/*  libmCreateSelectBox
    �A�C�e���쐬�i�`�F�b�N�{�b�N�X�j
    
    @param: libmOpt *opt
    �Ώۂ̃��j���[�I�v�V����
    
    @param: const char* Name
    �Z���N�g�{�b�N�X��
    
    @param: const char *List[]
    �Z���N�g�{�b�N�X�ɕ\������A�C�e�����X�g
    const char *�̔z��A�Ō��NULL���K�v
    
    @param: int Selected
    ���X�g����ŏ��ɑI���ς݂Ƃ���ʒu
    
    @return : �쐬���ꂽ�A�C�e��
 */
MenuItem* libmCreateSelectBox(libmOpt *opt ,const char* Name, const char *List[],int Selected);

/*  libmCreateUpDownCtrl
    �A�C�e�����쐬�i�A�b�v�_�E���R���g���[���j
    
    @param: libmOpt *opt
    �Ώۂ̃��j���[�I�v�V����
    
    @param: const char* Name
    �A�b�v�_�E���R���g���[����
    
    @param: float Now
    ���ݒl
    
    @param: float Step
    �����l
    
    @param: float Min
    �ŏ��l
    
    @param: float Max
    �ő�l
    
    @param: bool type
    false = �`�掞�Ɍ��ݒl�������_�\�������� ( 00.00 �̂悤�Ȋ����j
    true = �`�掞�Ɍ��ݒl�𐮐��\������
    
    @return : �쐬���ꂽ�A�C�e��
 */
MenuItem* libmCreateUpDownCtrl(libmOpt *opt ,const char* Name,float Now,float Step,float Min, float Max,bool type);

/*  libmCreateSpacer
    �A�C�e�����쐬�i�X�y�[�T�[�j
    
    @param: libmOpt *opt
    �Ώۂ̃��j���[�I�v�V����
    
    @param: const char* Name
    �X�y�[�T�[��
    
    @return : �쐬���ꂽ�A�C�e��
 */
MenuItem* libmCreateSpacer(libmOpt *opt ,const char* Name);

/*  libmCreateTriggerButton
    �A�C�e�����쐬�i�g���K�[�{�^��)
    
    @param: libmOpt *opt
    �Ώۂ̃��j���[�I�v�V����
    
	@param: const char* Name
    �g���K�[�{�^����
    
    @return : �쐬���ꂽ�A�C�e��
 */
MenuItem* libmCreateTriggerButton(libmOpt *opt ,const char* Name);



/*	#############################################################
	#							�`��							#
	#############################################################
*/




/*  libmInitBuffers
    ���݂̃f�B�X�v���C�̏�Ԃɍ��킹�ĕ`�揀��������
    
    @param: int opt
	��������ۂɐݒ肷��`��I�v�V����
	�ȉ�����L���ɂ���������ݒ�
	
	LIBM_DRAW_INIT8888		�s�N�Z���t�H�[�}�b�g8888�ŏ�����
	LIBM_DRAW_INIT4444		�s�N�Z���t�H�[�}�b�g4444�ŏ�����
	LIBM_DRAW_INIT5650		�s�N�Z���t�H�[�}�b�g5650�ŏ�����
	LIBM_DRAW_INIT5551		�s�N�Z���t�H�[�}�b�g5551�ŏ�����
	LIBM_DRAW_BLEND			�w�i���߂�L���ɂ���
	LIBM_DRAW_RETURN		�������`��ʒu����ʊO�������ꍇ�͐܂�Ԃ��ĕ\��������
	
	LIBM_DRAW_INIT			�s�N�Z���t�H�[�}�b�g8888�A�����͎����܂�Ԃ��ŉ�ʂ�����������
							��Ɏ���A�v����libmenu�̕����`��֐��� pspDebugScreenXXXX �̗l�Ɏg����悤�ɂ���
    
    @param: int sync
    �����Ώ�
    
	PSP_DISPLAY_SETBUF_IMMEDIATE
	PSP_DISPLAY_SETBUF_NEXTFRAME
	�̂ǂ��炩���w��
	
	��opt �� LIBM_DRAW_INIT���w�肵���ꍇ�́Async �� PSP_DISPLAY_SETBUF_NEXTFRAME ���w��
    
    @return: true = �����Afalse = ���s
 */
bool libmInitBuffers( int opt ,int sync );



/*  libmSwapBuffers
    libmRender �ŕ`��Ɏg���o�b�t�@�[�̕ύX
    
	���j���[�\�����ɃX���b�h��~����悤�ɐݒ肵���ꍇ
	������g���ƃ_�u���o�b�t�@�����O�̂悤�Ȃ��Ƃ��o���邪
	�����͓K���Ȃ̂ŕςȓ��������邩������Ȃ�
	
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
 */
void libmSwapBuffers();

/*  libmClearBuffers
    libmRender �ŕ`��Ɏg���o�b�t�@�[���N���A�i���F�j
    
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
 */
void libmClearBuffers();


/*  libmPrintXY
	
	libmenu�̓����t�H���g���g���� ������ ��`�悷��i���W�w�肠��j
	
	@params : int x
	�\���ʒu X
	
	@params : int y
	�\���ʒu Y
	
	@params : unsigned int fg
	�t�H���g�F(32bit)
	0 ���ƕ`�悵�Ȃ�
	
	@params : unsigned int bg
	�w�i�F(32bit)
	0 ���ƕ`�悵�Ȃ�
	
	@params : const char *str
    �`��Ώ� ������
    
    @return: �`�悵��������
    
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
 */
inline int libmPrintXY( int x, int y, u32 fg, u32 bg, const char *str );



/*  libmPrintfXY
	
	libmenu�̓����t�H���g���g���� �����t�������� ��`�悷��i���W�w�肠��j
	
	@params : int x
	�\���ʒu X
	
	@params : int y
	�\���ʒu Y
	
	@params : unsigned int fg
	�t�H���g�F(32bit)
	0 ���ƕ`�悵�Ȃ�
	
	@params : unsigned int bg
	�w�i�F(32bit)
	0 ���ƕ`�悵�Ȃ�
	
	@params : char *buf
	�������當����𐶐�����ۂɎg����o�b�t�@�[
	
	@params : int bufLen
	�o�b�t�@�[�T�C�Y�ichar *buf�j
	
	@params : format, ...
    �`��Ώ� �����t�� ������
    
    @return: �`�悵��������
    
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
 */
inline int libmPrintfXY( int x, int y, u32 fg, u32 bg, char *buf ,int bufLen ,const char *format, ... );



/*  libmPutCharXY
	
	libmenu�̓����t�H���g���g���� ���� ��`�悷��i���W�w�肠��j
	
	@params : int x
	�\���ʒu X
	
	@params : int y
	�\���ʒu Y
	
	@params : unsigned int fg
	�t�H���g�F(32bit)
	0 ���ƕ`�悵�Ȃ�
	
	@params : unsigned int bg
	�w�i�F(32bit)
	0 ���ƕ`�悵�Ȃ�
	
	@params : const char *str
    �`��Ώ� ����
    
    @return: �`�悵��������
    
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
 */
inline int libmPutCharXY( int x, int y, u32 fg, u32 bg, const char chr );




/*  libmDebugScreenInit
	
	��ʂɕ`�悷�鏀��������
	pspDebugScreenInit �Ǝ����悤�ȓ���
 */
#define	libmDebugScreenInit()		libmInitBuffers(LIBM_DRAW_INIT,PSP_DISPLAY_SETBUF_NEXTFRAME)




/*  libmDebugScreenPrint
	
	libmenu�̓����t�H���g���g���� ������ ��`�悷��i���W�w��Ȃ��j
	pspDebugScreenPrintf �Ǝ����悤�ȓ���
	
	@params : unsigned int fg
	�t�H���g�F(32bit)
	0 ���ƕ`�悵�Ȃ�
	
	@params : unsigned int bg
	�w�i�F(32bit)
	0 ���ƕ`�悵�Ȃ�
	
	@params : const char *str
    �`��Ώ� ������
    
    @return: �`�悵��������
    
	�����O�� libmInitBuffers �܂��� libmDebugScreenInit �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
 */
#define	libmDebugScreenPrint(fg,bg,str)							libmPrintXY(-1,-1,fg,bg,str)



/*  libmDebugScreenPrintf
	
	libmenu�̓����t�H���g���g���� �����t�������� ��`�悷��i���W�w��Ȃ��j
	pspDebugScreenPrintf �Ǝ����悤�ȓ���
	
	@params : unsigned int fg
	�t�H���g�F(32bit)
	0 ���ƕ`�悵�Ȃ�
	
	@params : unsigned int bg
	�w�i�F(32bit)
	0 ���ƕ`�悵�Ȃ�
	
	@params : char *buf
	�������當����𐶐�����ۂɎg����o�b�t�@�[
	
	@params : int bufLen
	�o�b�t�@�[�T�C�Y�ichar *buf�j
	
	@params : format, ...
    �`��Ώ� �����t�� ������
    
    @return: �`�悵��������
    
	�����O�� libmInitBuffers �܂��� libmDebugScreenInit �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
 */
#define	libmDebugScreenPrintf(fg,bg,buf,bufLen,format,...)		libmPrintfXY(-1,-1,fg,bg,buf,bufLen,format, __VA_ARGS__)


/*  libmDebugScreenPutChar
	
	libmenu�̓����t�H���g���g���� ���� ��`�悷��i���W�w��Ȃ��j
	pspDebugScreenPrintf �Ǝ����悤�ȓ���
	
	@params : unsigned int fg
	�t�H���g�F(32bit)
	0 ���ƕ`�悵�Ȃ�
	
	@params : unsigned int bg
	�w�i�F(32bit)
	0 ���ƕ`�悵�Ȃ�
	
	@params : const char *str
    �`��Ώ� ����
    
    @return: �`�悵��������
    
	�����O�� libmInitBuffers �܂��� libmDebugScreenInit �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
 */
#define	libmDebugScreenPutChar(fg,bg,chr)							libmPrintfXY(-1,-1,fg,bg,chr)


/*  libmDebugScreenSetXY
	
	���W�w��Ȃ��̕���/������`��֐���
	�`����J�n����ʒu���w�肷��
	
	@params : int x
	�`��J�n�ʒuX�@(0 - 59)
	1���� 8x8 �h�b�g�Ȃ̂�1�s�ő�60�����܂�
	
	@params : int y
	�`��J�n�ʒuY�@(0 - 33)
	1���� 8x8 �h�b�g�Ȃ̂�1��ő�34�����܂�
    
    @return: true = �����Afalse = ���s�iX/Y�ǂ��炩�̒l���ُ�j
 */
bool libmDebugScreenSetXY( int x ,int y );


/*	libmLine
	��ʂɐ���`�悷��
	
	@params : int sx
	�`��J�n�ʒuX
	
	@params : int sy
	�`��J�n�ʒuY
	
	@params : int ex
	�`��I���ʒuX
	
	@params : int ey
	�`��I���ʒuY
    
	@params : u32 color
	�`��F(32bit)
	
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
*/
inline void libmLine( int sx, int sy, int ex, int ey, u32 color );


/*	libmPoint
	��ʂɓ_��`�悷��
	���w�肵��32bit�F�����݂̉�ʐݒ�ɍ��킹�Ď����ϊ�����
	
	@params : void *adr
	�`�悷��VRAM�A�h���X
	(libmMakeDrawAddr�ō��W���A�h���X�֕ϊ�)
	
	@params : u32 color
	�`��F(32bit)
    
    �g�p��
    
    if( libmInitBuffers(false,PSP_DISPLAY_SETBUF_NEXTFRAME) )
    {
 		//X=50,Y=100�̈ʒu�ɐԂ��_��`�悷��
 		void *adr = libmMakeDrawAddr(50,100);
 		u32 color = libmMake32bitColor(255,0,0,255);
    	libmPoint(adr,color);
    }
    
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
*/
inline void libmPoint( void *adr, u32 src );


/*	libmPointEx
	��ʂɓ_��`�悷��
	����ʐݒ�ɍ��킹�ĕϊ��ς݂̐F���w��
	
	@params : void *adr
	�`�悷��VRAM�A�h���X
	(libmMakeDrawAddr�ō��W���A�h���X�֕ϊ�)
	
	@params : u32 color
	��ʐݒ�ɍ��킹�ĕϊ��ς݂̕`��F
	
    
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
*/
inline void libmPointEx( void *adr, u32 src );

/*	libmFillRect
	�w��͈͂��w��F�œh��Ԃ�
	
	@params : int sx
	�J�n�ʒuX (0-480)
	
	@params : int sy
	�J�n�ʒuY (0-272)
	
	@params : int ex
	�I���ʒuX (0-480)
	
	@params : int ey
	�I���ʒuY (0-272)
	
	@params : u32 color
	�`��F(32bit)
	
    
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
*/
inline void libmFillRect( int sx, int sy, int ex, int ey, u32 color );



/*	libmCircle
	��ʂɉ~��`�悷��
	
	@params : int x
	�J�n�ʒuX (0-480)
	
	@params : int y
	�J�n�ʒuY (0-272)
	
	@params : u32 radius
	�~�̔��a
	
	@params : u32 color
	�`��F(32bit)
	
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
*/
inline void libmCircle( int x, int y, u32 radius, u32 color );



/*	libmFrame
	��ʂɘg��`�悷��
	
	@params : int sx
	�J�n�ʒuX (0-480)
	
	@params : int sy
	�J�n�ʒuY (0-272)
	
	@params : int ex
	�I���ʒuX (0-480)
	
	@params : int ey
	�I���ʒuY (0-272)
	
	@params : u32 color
	�`��F(32bit)
	
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
*/
inline void libmFrame( int sx, int sy, int ex, int ey, u32 color );


/*	libmMakeDrawAddr
	���W�����݂̉�ʐݒ�ɍ��킹��VRAM�A�h���X�ɕϊ�����
	
	@params : int x
	�`��ʒuX (0-480)
	
	@params : int y
	�`��ʒuY (0-272)
	
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
*/
inline void* libmMakeDrawAddr( int x, int y );

/*	libmConvert8888_XXXX
	32bit�F(R:8 G:8 B:8 A:8)���e�t�H�[�}�b�g�̐F�֕ϊ�����
	
	@params : u32 src
	�ϊ����̐F
	
    @return: �w��t�H�[�}�b�g�֕ϊ����ꂽ�F
*/
inline u32 libmConvert8888_5650( u32 src );
inline u32 libmConvert8888_5551( u32 src );
inline u32 libmConvert8888_4444( u32 src );

/*	libmConvertXXXX_8888
	�e�t�H�[�}�b�g�̐F��32bit�F(R:8 G:8 B:8 A:8)�֕ϊ�����
	
	@params : u32 src
	�ϊ����̐F
	
    @return: �ϊ����ꂽ32bit�F(8888�t�H�[�}�b�g)
*/
inline u32 libmConvert4444_8888(u32 src);
inline u32 libmConvert5551_8888(u32 src);
inline u32 libmConvert5650_8888(u32 src);




/*	libmAlphaBlendXXXX
	�e�t�H�[�}�b�g�̐F(src�Adst)�Ƀ��u�����h����
	
	@params : u8 alpha
	�u�����h����A���t�@�l (0-255)
	
	@params : u32 src
	�u�����h����F�i���j
	���s�N�Z���t�H�[�}�b�g�ɍ��킹�ĕϊ��ς݂̐F
	
	@params : u32 dst
	�u�����h����F�i��j
	���s�N�Z���t�H�[�}�b�g�ɍ��킹�ĕϊ��ς݂̐F
	
    @return: ���u�����h���ꂽ�F
*/
inline u32 libmAlphaBlend8888( u8 alpha, u32 src, u32 dst );
inline u32 libmAlphaBlend4444( u8 alpha, u32 src, u32 dst );
inline u32 libmAlphaBlend5551( u8 alpha, u32 src, u32 dst );
inline u32 libmAlphaBlend5650( u8 alpha, u32 src, u32 dst );



/*	libmMake32bitColor
	RGBA����32bit(8888�t�H�[�}�b�g)�̐F�𐶐�����
	
	@params : u8 R
	Color Red (0-255)
	
	@params : u8 G
	Color Green (0-255)
	
	@params : u8 B
	Color Blue (0-255)
	
	@params : u8 A
	Alpha (0-255)
	
	@return : 8888�t�H�[�}�b�g��32bit�F
*/
#define	libmMake32bitColor(R,G,B,A)	 ((R & 255) | ((G & 255) << 8) | ((B & 255) << 16) | ((A & 255) << 24))


/*	libmGetColor
	�w�肵�����WXY����`��F���擾
	
	@params : int x
	���WX(0-480)
	
	@params : int y
	���WX(0-272)
	
	@return : �w�肵�����W���瓾���F
	
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
*/
inline u32 libmGetColor(void *addr);


/*	libmGetCurVInfo
	libmenu �e�`��֐��Ŏg���Ă��錻�݂̊e�ݒ���擾
	���擾�o������ɂ��Ă� libm_vram_info �̒�`�������Q��
	
	@params : libm_vram_info *info
	�擾��̕ϐ�
	
	@return : true = �����Afalse = ���s
	
	�����O�� libmInitBuffers �����s���ĂȂ��Ɛ��퓮�삵�Ȃ�
*/
bool libmGetCurVInfo(libm_vram_info *info);


/*	libmSetCurVInfo
	libmenu �e�`��֐��Ŏg���Ă��錻�݂̐ݒ��ύX����
	
	@params : format
	�s�N�Z���t�H�[�}�b�g
	
	@params : buf
	�`���̊VRAM�A�h���X
	
	@params : width
	�`�敝
*/
void libmSetCurVInfo(int format, void *buf, int width);


#ifdef __cplusplus
}
#endif

#endif
