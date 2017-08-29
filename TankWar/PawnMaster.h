#pragma once
#include "BasePawn.h"

typedef BYTE PawnCommandType;
#define PAWN_COMMAND_TYPE_NONE 0x00
#define PAWN_COMMAND_TYPE_CREATE 0x0f
#define PAWN_COMMAND_TYPE_DESTORY 0xff

//����ṹֻ������PawnCommandTemplate�洢һ��Pawnָ�롣
struct PawnUnit
{
	BasePawn* pSavedPawn;
};

//����ָ������pawn�Ĵ���ṹ��
//����ṹ����PawnMaster�У�
//�ܹ����д���Pawn����
class PawnCommandTemplate
{
public:
	//pDesc��һ����̬������ڴ棬CreatePawn��Ҫ�����ͷ�����ڴ档
	virtual BasePawn* CreatePawn(PawnUnit * saveUnit, PawnProperty* pProperty, Scence* pScence) = 0;
	//pPawn��һ����̬������ڴ棬�����ٶ�Ӧ��Pawn֮��DestoryPawn��Ҫ�����ͷ�����ڴ档
	//�������Pawn��Ӧ�Ĵ洢��λ��
	virtual PawnUnit* DestoryPawn(BasePawn* pPawn, Scence* pScence) = 0;

public:
	//�洢���д����commandTemplate���ɵ�pawn����
	LinkedAllocator<PawnUnit> Manager;
};

struct PawnCommand 
{
	PawnCommandType CommandType;

	union {
		struct
		{
			//Ҫ���ɵ�Pawn���ͣ��������ֻ���������pawnMaster��ע��������͡�
			//ע�⣬���ֵ��Ӧ��CommandTemplateList�������Ԫ�أ�
			//���ǲ���Ϊ0�����Ի�ȡԪ�ص�ʱ��Ҫע���һ��ȡ�������ж�Ӧ��ֵ��
			PawnType pawnType;
			//Pawn�������趨ָ�룬���ָ��ָ����ڴ��ɾ����Pawn���ͷ���ͻ��ա�
			PawnProperty* pProperty;
		} CreateCommand;

		struct  
		{
			PawnType pawnType;
			//Ҫɾ����Pawn�����ָ�롣
			BasePawn* pPawn_to_destory;
		} DestoryCommand;
	};
};

//������Ͷ���ר�������Զ�������Pawn��������
class PawnMaster
{
public:
	PawnMaster(UINT MaxCommandNum, Scence* pScence);
	~PawnMaster();

	//ע��һ��Pawn����������ģ�壬����Ϊ���Pawn���ͷ����PawnType��
	PawnType AddCommandTemplate(std::unique_ptr<PawnCommandTemplate> pCommandTemplate);
	//��¼һ������ָ������Pawn��ָ�
	void CreatePawn(PawnType pawnType, PawnProperty* pProperty);
	//��¼һ��ɾ��Pawn��ָ�
	void DestroyPawn(PawnType pawnType, BasePawn* pThePawn);
	//ִ��ָ�������Pawn��
	void Executee();

protected:
	//������������
	void CreatingPawn(UINT commandIndex);
	void DestoringPawn(UINT commandIndex);

protected:
	//Pawn����ĳ�����������������󽫻��ṩ��Pawn���;����ȥ��ȡ�������塣
	Scence* m_pScence;
	//δִ�е�����������
	UINT UnExecuteeCommandNum = 0;
	//�洢ָ��棬�ڹ��캯���й̶�һ����С��
	std::vector<PawnCommand> CommandBuffer;

public:
	//����ģ�壬������ʱ��ӡ�
	std::vector<std::unique_ptr<PawnCommandTemplate>> CommandTemplateList;
};

