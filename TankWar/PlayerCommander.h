#pragma once
#include "BasePawn.h"

typedef unsigned int PlayerControlType;

//���Ƶ�Ԫ����Ӧ�û����롣
class ControlCommandTemplate
{
public:
	//����ƶ�
	virtual void MouseMove(BasePawn* pPawn, float lastX, float lastY, float currX, float currY, WPARAM btnState) = 0;
	//����W��
	virtual void HitKey_W(BasePawn* pPawn) = 0;
	//����A��
	virtual void HitKey_A(BasePawn* pPawn) = 0;
	//����S��
	virtual void HitKey_S(BasePawn* pPawn) = 0;
	//����D��
	virtual void HitKey_D(BasePawn* pPawn) = 0;
	//����������
	virtual void PressMouseButton_Left(BasePawn* pPawn) = 0;
	//��������Ҽ�
	virtual void PressMouseButton_Right(BasePawn* pPawn) = 0;
};

//��֪�������ͣ�Ŀǰֻ����wasd��������Ҽ���
enum KeyType
{
	W,	
	A, 
	S, 
	D, 
	M_L,	//������ 
	M_R		//����Ҽ�
};

//��һ���ֽ���Ϊ����״̬ת���ı�־
typedef unsigned char StateChange;
//���尴����4��״̬ת����
//��ѹ���ͷ�
#define PRESS_TO_RELEASE	(0xf0)
//�ͷŵ���ѹ
#define RELEASE_TO_PRESS	(0x0f)
//�����ͷ�
#define RELEASE_TO_RELEASE	(0x00)
//���ְ�ѹ
#define PRESS_TO_PRESS		(0xff)


struct KeyState
{
	KeyType keyType;
	//������ת��״̬��
	StateChange stateChange;
};

struct MouseState
{
	POINT LastMousePos;
	POINT CurrMousePos;
};

//��ҿ��Ƶ�Ԫ
struct PlayerControlUnit
{
	//�����Ƶ�Pawn��
	BasePawn* pControledPawn;
	//�������õ�������ģ�����ͣ����ģ�����ͱ�����Pawn����ע�ᡣ
	PlayerControlType ControlType;
};

class PlayerCommander
{
public:
	PlayerCommander();
	~PlayerCommander();
	//��ӿ���ģ�壬����Ϊ�������ģ���������ͱ�ǩ��
	PlayerControlType AddCommandTemplate(std::unique_ptr<ControlCommandTemplate> pCommandTemplate);
	//ɾ��ָ��Pawn�����PlayerCommander�еĿ��Ƶ�Ԫ��
	void DeletePlayerControl(BasePawn* pThePawn);
	//��ⰴ����Ӧ�����°���
	void DetactKeyboardState();
	//ִ�п��ƣ����ݵ�ǰ��ҵ����룬�ж϶�pawnִ�����ֺ�����
	void Executee();
	
protected:
	//���Ƶ�Ԫ��
	LinkedAllocator<PlayerControlUnit> UnitAllocator;
	//��������ģ�塣
	std::vector<ControlCommandTemplate> CommandTemplateList;

	//���ڿ���״̬�����ԣ������ϴ�����λ�ã���Щ�������¡�
public:
	//�洢����״̬���������ǰһ�κ���һ�ε����ꡣ
	MouseState mouseState;

};

