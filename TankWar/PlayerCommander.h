#pragma once
#include "BasePawn.h"

typedef unsigned int PlayerControlType;

//控制单元，响应用户输入。
class ControlCommandTemplate
{
public:
	//鼠标移动
	virtual void MouseMove(BasePawn* pPawn, float lastX, float lastY, float currX, float currY, WPARAM btnState) = 0;
	//按下W键
	virtual void HitKey_W(BasePawn* pPawn) = 0;
	//按下A键
	virtual void HitKey_A(BasePawn* pPawn) = 0;
	//按下S键
	virtual void HitKey_S(BasePawn* pPawn) = 0;
	//按下D键
	virtual void HitKey_D(BasePawn* pPawn) = 0;
	//按下鼠标左键
	virtual void PressMouseButton_Left(BasePawn* pPawn) = 0;
	//按下鼠标右键
	virtual void PressMouseButton_Right(BasePawn* pPawn) = 0;
};

//已知按键类型，目前只考虑wasd和鼠标左右键。
enum KeyType
{
	W,	
	A, 
	S, 
	D, 
	M_L,	//鼠标左键 
	M_R		//鼠标右键
};

//用一个字节作为按键状态转换的标志
typedef unsigned char StateChange;
//定义按键的4种状态转换。
//按压到释放
#define PRESS_TO_RELEASE	(0xf0)
//释放到按压
#define RELEASE_TO_PRESS	(0x0f)
//保持释放
#define RELEASE_TO_RELEASE	(0x00)
//保持按压
#define PRESS_TO_PRESS		(0xff)


struct KeyState
{
	KeyType keyType;
	//按键的转换状态。
	StateChange stateChange;
};

struct MouseState
{
	POINT LastMousePos;
	POINT CurrMousePos;
};

//玩家控制单元
struct PlayerControlUnit
{
	//被控制的Pawn。
	BasePawn* pControledPawn;
	//控制所用到的命令模板类型，这个模板类型必须有Pawn自行注册。
	PlayerControlType ControlType;
};

class PlayerCommander
{
public:
	PlayerCommander();
	~PlayerCommander();
	//添加控制模板，返回为这个控制模板分配的类型标签。
	PlayerControlType AddCommandTemplate(std::unique_ptr<ControlCommandTemplate> pCommandTemplate);
	//删除指定Pawn在这个PlayerCommander中的控制单元。
	void DeletePlayerControl(BasePawn* pThePawn);
	//检测按键相应，更新按键
	void DetactKeyboardState();
	//执行控制，根据当前玩家的输入，判断对pawn执行那种函数。
	void Executee();
	
protected:
	//控制单元。
	LinkedAllocator<PlayerControlUnit> UnitAllocator;
	//控制命令模板。
	std::vector<ControlCommandTemplate> CommandTemplateList;

	//关于控制状态的属性，比如上次鼠标的位置，那些按键按下。
public:
	//存储鼠标的状态，包括鼠标前一次和这一次的坐标。
	MouseState mouseState;

};

