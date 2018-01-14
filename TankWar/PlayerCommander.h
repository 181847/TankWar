#pragma once
#include "BasePawn.h"

typedef unsigned int PlayerControlType;
#define PLAYER_CONTROL_TYPE_NONE 0

struct MouseState;

//控制单元，响应用户输入。
class ControlCommandTemplate
{
public:
	//鼠标移动
	virtual void MouseMove(BasePawn* pPawn, MouseState mouseState, const GameTimer& gt) = 0;
	//按下W键
	virtual void HitKey_W(BasePawn* pPawn, const GameTimer& gt) = 0;
	//按下A键
	virtual void HitKey_A(BasePawn* pPawn, const GameTimer& gt) = 0;
	//按下S键
	virtual void HitKey_S(BasePawn* pPawn, const GameTimer& gt) = 0;
	//按下D键
	virtual void HitKey_D(BasePawn* pPawn, const GameTimer& gt) = 0;
	//点击一次鼠标左键
	virtual void HitMouseButton_Left(BasePawn * pPawn, const GameTimer& gt) = 0;
	//点击一次鼠标右键
	virtual void HitMouseButton_Right(BasePawn * pPawn, const GameTimer& gt) = 0;
	//按下鼠标左键
	virtual void PressMouseButton_Left(BasePawn* pPawn, const GameTimer& gt) = 0;
	//按下鼠标右键
	virtual void PressMouseButton_Right(BasePawn* pPawn, const GameTimer& gt) = 0;
};

//已知按键类型，目前只考虑wasd和鼠标左右键。
enum KeyType: BYTE
{
	W = 'W',	
	A = 'A', 
	S = 'S', 
	D = 'D', 
	M_L = VK_LBUTTON,	//鼠标左键 
	M_R = VK_RBUTTON		//鼠标右键
};

//用一个字节作为按键状态转换的标志
typedef unsigned char StateChange;
#define STATE_CHANGE(CurrentState, NewState) (CurrentState =(CurrentState << 4 & 0xf0)	| (NewState & 0x0f))

//定义按键的4种状态转换。
//按压到释放
#define PRESS_TO_RELEASE	(0xf0)
//释放到按压
#define RELEASE_TO_PRESS	(0x0f)
//保持释放
#define RELEASE_TO_RELEASE	(0x00)
//保持按压
#define PRESS_TO_PRESS		(0xff)
//按压状态
#define PRESS				PRESS_TO_PRESS
//释放状态
#define RELEASE				RELEASE_TO_RELEASE
//适用于鼠标移动状态的值
#define MOVE				PRESS
//鼠标停止
#define STOP				RELEASE
//鼠标从移动到静止
#define MOVE_TO_STOP		PRESS_TO_RELEASE
//鼠标从静止到移动
#define STOP_TO_MOVE		RELEASE_TO_PRESS
//鼠标保持静止
#define STOP_TO_STOP		RELEASE_TO_RELEASE
//鼠标保持移动
#define MOVE_TO_MOVE		PRESS_TO_PRESS

struct KeyState
{
	KeyType keyType;
	//按键的转换状态。
	StateChange stateChange = RELEASE;
public:
	KeyState(KeyType type);
	//newState指定为当前的最新状态就可以了，
	//使用RELEASE_TO_RELEASE表示当前状态处于按键释放
	//或者PRESS_TO_PRESS表示当前状态处于按压。
	void ChangeState(StateChange newState);
};

struct MouseState
{
	//鼠标移动状态用PRESS表示，
	StateChange moveState;
	POINT LastMousePos;
	POINT CurrMousePos;
	//当前窗口是否获得鼠标的焦点，没有焦点的情况下不发动MouseMove方法，默认为false。
	bool IsCaptured = false;
public:
	//更新鼠标位置，一旦更新鼠标位置，就表示鼠标移动，将会更新鼠标移动状态。
	void UpdateLocation(LONG newX, LONG newY);
};

//玩家控制单元
struct PlayerControlUnit
{
	//被控制的Pawn。
	BasePawn* pControledPawn;
	//控制所用到的命令模板类型，这个模板类型必须由Pawn自行注册，这个值不能为0，
	//0代表非法控制类型。
	PlayerControlType ControlType;
};

class PlayerCommander
{
public:
	//maxControlUnitNum最多能够控制的Pawn数量。
	PlayerCommander(UINT maxControlUnitNum);
	~PlayerCommander();
	//添加控制模板，返回为这个控制模板分配的类型标签。
	PlayerControlType AddCommandTemplate(std::unique_ptr<ControlCommandTemplate> pCommandTemplate);
	//申请添加一个对指定Pawn的控制单位，返回这个控制单位的指针，方便回收内存。
	PlayerControlUnit* NewPlayerControl(PlayerControlType controlType, BasePawn* pControledPawn);
	//删除指定Pawn在这个PlayerCommander中的控制单元。
	void DeletePlayerControl(PlayerControlUnit* pTheUnit);
	//检测按键相应，更新按键状态。
	void DetactKeyState();
	//执行控制，根据当前玩家的输入，判断对pawn执行那种函数。
	void Executee(const GameTimer& gt);

protected:
	//根据按键状态执行这个控制单元的命令。
	void ExecuteeCommandTeplate(PlayerControlUnit* pUnit, const GameTimer& gt);
	
protected:
	//控制单元。
	LinkedAllocator<PlayerControlUnit> UnitAllocator;
	//控制命令模板。
	std::vector<std::unique_ptr<ControlCommandTemplate>> CommandTemplateList;

public:
	//存储鼠标的状态，这里面包括鼠标前一次和这一次的坐标。
	MouseState mouseState;
	//所有按键的状态。
	std::vector<KeyState> keyStates;

};

