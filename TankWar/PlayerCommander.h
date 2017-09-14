#pragma once
#include "BasePawn.h"

typedef unsigned int PlayerControlType;
#define PLAYER_CONTROL_TYPE_NONE 0

struct MouseState;

//���Ƶ�Ԫ����Ӧ�û����롣
class ControlCommandTemplate
{
public:
	//����ƶ�
	virtual void MouseMove(BasePawn* pPawn, MouseState mouseState, const GameTimer& gt) = 0;
	//����W��
	virtual void HitKey_W(BasePawn* pPawn, const GameTimer& gt) = 0;
	//����A��
	virtual void HitKey_A(BasePawn* pPawn, const GameTimer& gt) = 0;
	//����S��
	virtual void HitKey_S(BasePawn* pPawn, const GameTimer& gt) = 0;
	//����D��
	virtual void HitKey_D(BasePawn* pPawn, const GameTimer& gt) = 0;
	//���һ��������
	virtual void HitMouseButton_Left(BasePawn * pPawn, const GameTimer& gt) = 0;
	//���һ������Ҽ�
	virtual void HitMouseButton_Right(BasePawn * pPawn, const GameTimer& gt) = 0;
	//����������
	virtual void PressMouseButton_Left(BasePawn* pPawn, const GameTimer& gt) = 0;
	//��������Ҽ�
	virtual void PressMouseButton_Right(BasePawn* pPawn, const GameTimer& gt) = 0;
};

//��֪�������ͣ�Ŀǰֻ����wasd��������Ҽ���
enum KeyType: BYTE
{
	W = 'W',	
	A = 'A', 
	S = 'S', 
	D = 'D', 
	M_L = VK_LBUTTON,	//������ 
	M_R = VK_RBUTTON		//����Ҽ�
};

//��һ���ֽ���Ϊ����״̬ת���ı�־
typedef unsigned char StateChange;
#define STATE_CHANGE(CurrentState, NewState) (CurrentState =(CurrentState << 4 & 0xf0)	| (NewState & 0x0f))

//���尴����4��״̬ת����
//��ѹ���ͷ�
#define PRESS_TO_RELEASE	(0xf0)
//�ͷŵ���ѹ
#define RELEASE_TO_PRESS	(0x0f)
//�����ͷ�
#define RELEASE_TO_RELEASE	(0x00)
//���ְ�ѹ
#define PRESS_TO_PRESS		(0xff)
//��ѹ״̬
#define PRESS				PRESS_TO_PRESS
//�ͷ�״̬
#define RELEASE				RELEASE_TO_RELEASE
//����������ƶ�״̬��ֵ
#define MOVE				PRESS
//���ֹͣ
#define STOP				RELEASE
//�����ƶ�����ֹ
#define MOVE_TO_STOP		PRESS_TO_RELEASE
//���Ӿ�ֹ���ƶ�
#define STOP_TO_MOVE		RELEASE_TO_PRESS
//��걣�־�ֹ
#define STOP_TO_STOP		RELEASE_TO_RELEASE
//��걣���ƶ�
#define MOVE_TO_MOVE		PRESS_TO_PRESS

struct KeyState
{
	KeyType keyType;
	//������ת��״̬��
	StateChange stateChange = RELEASE;
public:
	KeyState(KeyType type);
	//newStateָ��Ϊ��ǰ������״̬�Ϳ����ˣ�
	//ʹ��RELEASE_TO_RELEASE��ʾ��ǰ״̬���ڰ����ͷ�
	//����PRESS_TO_PRESS��ʾ��ǰ״̬���ڰ�ѹ��
	void ChangeState(StateChange newState);
};

struct MouseState
{
	//����ƶ�״̬��PRESS��ʾ��
	StateChange moveState;
	POINT LastMousePos;
	POINT CurrMousePos;
	//��ǰ�����Ƿ������Ľ��㣬û�н��������²�����MouseMove������Ĭ��Ϊfalse��
	bool IsCaptured = false;
public:
	//�������λ�ã�һ���������λ�ã��ͱ�ʾ����ƶ��������������ƶ�״̬��
	void UpdateLocation(LONG newX, LONG newY);
};

//��ҿ��Ƶ�Ԫ
struct PlayerControlUnit
{
	//�����Ƶ�Pawn��
	BasePawn* pControledPawn;
	//�������õ�������ģ�����ͣ����ģ�����ͱ�����Pawn����ע�ᣬ���ֵ����Ϊ0��
	//0����Ƿ��������͡�
	PlayerControlType ControlType;
};

class PlayerCommander
{
public:
	//maxControlUnitNum����ܹ����Ƶ�Pawn������
	PlayerCommander(UINT maxControlUnitNum);
	~PlayerCommander();
	//��ӿ���ģ�壬����Ϊ�������ģ���������ͱ�ǩ��
	PlayerControlType AddCommandTemplate(std::unique_ptr<ControlCommandTemplate> pCommandTemplate);
	//�������һ����ָ��Pawn�Ŀ��Ƶ�λ������������Ƶ�λ��ָ�룬��������ڴ档
	PlayerControlUnit* NewPlayerControl(PlayerControlType controlType, BasePawn* pControledPawn);
	//ɾ��ָ��Pawn�����PlayerCommander�еĿ��Ƶ�Ԫ��
	void DeletePlayerControl(PlayerControlUnit* pTheUnit);
	//��ⰴ����Ӧ�����°���״̬��
	void DetactKeyState();
	//ִ�п��ƣ����ݵ�ǰ��ҵ����룬�ж϶�pawnִ�����ֺ�����
	void Executee(const GameTimer& gt);

protected:
	//���ݰ���״ִ̬��������Ƶ�Ԫ�����
	void ExecuteeCommandTeplate(PlayerControlUnit* pUnit, const GameTimer& gt);
	
protected:
	//���Ƶ�Ԫ��
	LinkedAllocator<PlayerControlUnit> UnitAllocator;
	//��������ģ�塣
	std::vector<std::unique_ptr<ControlCommandTemplate>> CommandTemplateList;

public:
	//�洢����״̬��������������ǰһ�κ���һ�ε����ꡣ
	MouseState mouseState;
	//���а�����״̬��
	std::vector<KeyState> keyStates;

};

