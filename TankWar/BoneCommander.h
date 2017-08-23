#pragma once
#include "ControlItem.h"
#include "LinkedAllocator.h"
#include <DirectXMath.h>
using namespace DirectX;

typedef unsigned int UINT;

//����״ֵ̬��
//ÿһ֡�����ϣ���������Ҫ����һ��״̬��
//���뱣֤���й���״̬��һ����
typedef unsigned char FlipState;
#define FLIP_STATE_1 0x0f
#define FLIP_STATE_2 0xf0
#define FLIP_THE_STATE(TheState) (TheState = ~TheState)

//��һ�����ṹ�����ڸ���ControlItem�ľֲ����ꡣ
struct Bone
{
	Bone* pRoot;
	ControlItem* pControlItem;
	FlipState flipState;
public:
	//��һ��Ҫע�ⲻ���ù���ѭ����
	void LinkTo(Bone* pRoot);
};

class BoneCommander
{
public:
	BoneCommander(UINT maxBoneNum);
	~BoneCommander();
	//���ɹ�����Ĭ�Ϸ�ת״̬��Commander�ĵ�ǰ״̬��
	Bone* NewBone(ControlItem* pControlItem);
	//ɾ�����������չ�����
	void DeleteBone(Bone* pTheBone);
	//���¹������ݡ�
	void Update();
	//����ָ�����������Ƶ�ControlItem�ľֲ��任��
	//�����������ĸ�������û�и��£����ȸ��¸�������
	void UpdateTheBone(Bone* pBone);

protected:
	//��������ء�
	LinkedAllocator<Bone> BoneAllocator;
	//��ǰ���й�����״̬�����ֵ����ͳһ���й������ݵĸ��£�
	//ÿ�θ��¶���Ҫ���¹�����Ӧ�����FlipState��
	//��֤��BoneCommander�е�CurrStateһ�¡�
	FlipState CurrState;
};

