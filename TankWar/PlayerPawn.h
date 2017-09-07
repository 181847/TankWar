#pragma once
#include "BasePawn.h"
#include "PawnMaster.h"
#include "PlayerCommander.h"
#include "BoneCommander.h"
#include "CollideCommander.h"
#include "ShellProperty.h"

//可生成的PawnPlayer的数量
#define MAX_PLAYER_PAWN_NUM ((unsigned int)5)
#define CONTROLITEM_NUM_PLAYER_PAWN 4

//玩家的可控制ControlItem的根节点序号，注意玩家当中的摄像机和可渲染根节点是分开的。
#define CONTROLITEM_INDEX_PLAYER_PAWN_ROOT 0
//炮台
#define CONTROLITEM_INDEX_PLAYER_PAWN_BATTERY 1
//车身的控制器
#define CONTROLITEM_INDEX_PLAYER_PAWN_MAINBODY 2
//炮管控制器
#define CONTROLITEM_INDEX_PLAYER_PAWN_BARREL 3

//骨骼数组序号
#define BONE_INDEX_PLAYER_PAWN_ROOT 0
//摄像机坐标。
#define BONE_INDEX_PLAYER_PAWN_CAMERA_POS 1
//摄像机目标。
#define BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET 2
//炮台
#define BONE_INDEX_PLAYER_PAWN_BATTERY 3
//车身
#define BONE_INDEX_PLAYER_PAWN_MAINBODY 4
//炮管
#define BONE_INDEX_PLAYER_PAWN_BARREL 5

//射线最大长度
#define MAX_RAY_LENGTH 500


//玩家属性定义
struct PlayerProperty : public PawnProperty
{
	//直线速度
	float MoveSpeed;
	//车身旋转速度
	float RotationSpeed;
};

class PlayerPawn: public BasePawn
{
public:
	PlayerPawn();
	~PlayerPawn();

//这一部分负责关于PlayerPawn类的静态属性
public :
	//在PawnMaster中的类型标识。
	static PawnType pawnType;
	//玩家的控制模式。
	static PlayerControlType m_playerControlType;

	//负责本类生成和销毁指令的PawnMaster。
	static PawnMaster *			pPawnMaster;
	static PlayerCommander *	pPlayerCommander;
	static BoneCommander *		pBoneCommander;
	static CollideCommander *	pCollideCommander;

	//属性分配池
	static MyStaticAllocator<PlayerProperty> m_propertyAllocator;
	//类对象分配池
	static LinkedAllocator<PlayerPawn> m_PlayerPawnAllocator;

	//特殊的静态属性，标记一个装甲车的创建类型，当用户按下鼠标左键的时候就可以创建一个ArmoredCarPawn。
	static PawnType refCarType;

	//炮管的最大上扬角。
	static const float Radian_Pitch_Barrel_Max;
	//炮管的最小上扬角。
	static const float Radian_Pitch_Barrel_Min;

public:
	//执行所有注册方法，包括下面的注册PawnMaster。
	static void RegisterAll(
		PawnMaster *		pPawnMaster, 
		PlayerCommander *	pPlayerCommander, 
		BoneCommander *		pBoneCommander
		, CollideCommander *	pCollideCommander
	);
	//注册PawnMaster，并从PawnMaster中获取一个PawnType。
	static void RegisterPawnMaster(PawnMaster * pPawnMaster);
	static PlayerProperty* NewProperty();

	//玩家生成模板
	friend class PlayerPawnCommandTemplate;
	//玩家控制模板
	friend class PlayerControlCommandTemplate;


//这一部分是关于Player的非静态属性。
protected:
	//在PawnMaster中存储当前Pawn对象的单位指针。
	PawnUnit* m_pSavedUnit;
	//摄像机对象;
	MyCamera* m_pCamera;
	//在PlayerCommander中的控制单元指针。
	PlayerControlUnit * m_pPlayerControl;
	//PlayerPawn可控制的ControlItem数量，注意保存的都是指针。
	ControlItem* m_arr_ControlItem[CONTROLITEM_NUM_PLAYER_PAWN];
	//所有骨骼，骨骼对应ControlItem，注意这个骨骼数组中有两个控制摄像机的骨骼。
	Bone* m_arr_Bones[CONTROLITEM_NUM_PLAYER_PAWN + 2];

public:
	//指向这个PlayerPawn对象的相关游戏属性，比如前进速度，车身旋转速度，炮弹冷却时间……
	PlayerProperty* m_pProperty;

public:
	//PlayerPawn的根控制器，控制整体的位移，这个不显示。
	ControlItem * RootControl();
	//炮台控制器。
	ControlItem * Battery();
	//车身控制器，控制车身方向。
	ControlItem * MainBody();
	//炮管控制器。
	ControlItem * Barrel();
};



//用于PawnMaster中自动化生成Pawn的命令模板，禁止直接生成Pawn对象。
class PlayerPawnCommandTemplate : public PawnCommandTemplate
{
public:
	PlayerPawnCommandTemplate();
	~PlayerPawnCommandTemplate();

public:
	virtual BasePawn* CreatePawn(PawnProperty* pProperty, Scence* pScence);
	//pPawn是一个动态分配的内存，在销毁对应的Pawn之后，DestoryPawn需要自行释放这个内存到m_playerAllocator，
	//同时Pawn中的PawnProperty也需要被放回对应的内存池内。
	virtual void DestoryPawn(BasePawn* pPawn, Scence* pScence);

protected:
	//在PlayerCommander添加一个玩家的控制器。
	void AddPlayerControl(PlayerPawn* pPlayerPawn);
	//在BoneCommander中添加骨骼，让这个PlayerPawn的所有网格以及摄像头形成联动。
	void AddBones(PlayerPawn* pPlayerPawn);

	void DeletePlayerControl(PlayerPawn* pPlayerPawn);
	void DeleteBones(PlayerPawn* pPlayerPawn);
};

//控制单元，响应用户输入。
class PlayerControlCommandTemplate : public ControlCommandTemplate
{
public:
	//鼠标移动
	virtual void MouseMove(BasePawn* pPawn, MouseState mouseState, const GameTimer& gt);
	//按下W键
	virtual void HitKey_W(BasePawn* pPawn, const GameTimer& gt);
	//按下A键
	virtual void HitKey_A(BasePawn* pPawn, const GameTimer& gt);
	//按下S键
	virtual void HitKey_S(BasePawn* pPawn, const GameTimer& gt);
	//按下D键
	virtual void HitKey_D(BasePawn* pPawn, const GameTimer& gt);
	//点击一次鼠标左键
	virtual void HitMouseButton_Left(BasePawn * pPawn, const GameTimer& gt);
	//点击一次鼠标右键
	virtual void HitMouseButton_Right(BasePawn * pPawn, const GameTimer& gt);
	//按下鼠标左键
	virtual void PressMouseButton_Left(BasePawn* pPawn, const GameTimer& gt);
	//按下鼠标右键
	virtual void PressMouseButton_Right(BasePawn* pPawn, const GameTimer& gt);

public:
	//便捷方法。

	//旋转定位炮台的方向，使得炮台的水平方向尝试对准发生碰撞的位置。
	//注意不是直接指向，而是慢慢的旋转炮台到指定的位置。
	static void rotateBattery(
		PlayerPawn * pPlayerPawn, 
		XMFLOAT3 targetFloat3,
		const GameTimer& gt);

	//垂直旋转炮管，使得炮管对阵发生碰撞的位置。
	//注意不是直接指向，而是慢慢的旋转炮管到指定的位置。
	static void rotateBarrel(
		PlayerPawn * pPlayerPawn,
		XMFLOAT3 targetFloat3, 
		const GameTimer& gt);
};
