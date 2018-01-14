#pragma once
#include "ControlItem.h"
#include "LinkedAllocator.h"
#include <DirectXMath.h>
using namespace DirectX;

typedef unsigned int UINT;

//骨骼状态值，
//每一帧画面上，骨骼就需要更新一次状态，
//必须保证所有骨骼状态是一样的
typedef unsigned char FlipState;
#define FLIP_STATE_1 0x0f
#define FLIP_STATE_2 0xf0
#define FLIP_THE_STATE(TheState) (TheState = ~TheState)

//单一骨骼结构，用于更新ControlItem的局部坐标。
struct Bone
{
	Bone* pRoot;
	ControlItem* pControlItem;
	FlipState flipState;
public:
	//请一定要注意不能让骨骼循环。
	void LinkTo(Bone* pRoot);
};

class BoneCommander
{
public:
	BoneCommander(UINT maxBoneNum);
	~BoneCommander();
	//生成骨骼，默认翻转状态是Commander的当前状态。
	Bone* NewBone(ControlItem* pControlItem);
	//删除骨骼，回收骨骼。
	void DeleteBone(Bone* pTheBone);
	//更新骨骼数据。
	void Update();
	//更新指定骨骼所控制的ControlItem的局部变换，
	//如果这个骨骼的父骨骼还没有更新，就先更新父骨骼。
	void UpdateTheBone(Bone* pBone);

protected:
	//骨骼分配池。
	LinkedAllocator<Bone> BoneAllocator;
	//当前所有骨骼的状态，这个值用来统一所有骨骼数据的更新，
	//每次更新都需要更新骨骼对应的这个FlipState，
	//保证和BoneCommander中的CurrState一致。
	FlipState CurrState;
};

