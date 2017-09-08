#include "ShellPawn.h"

//静态对象初始化
LinkedAllocator<ShellPawn> ShellPawn::PawnAllocator(MAX_SHELL_PAWN_NUM);

ShellPawn::ShellPawn()
{
}


ShellPawn::~ShellPawn()
{
}

void ShellPawn::Register()
{
	//注册PawnMaster。
	gShellPawnType =
		gPawnMaster->AddCommandTemplate(
			std::make_unique<ShellPawnTemplate>());

	//注册AI控制单位。
	gShellAIControlType =
		gAICommander->AddAITemplate(
			std::make_unique<ShellAITemplate>());
}

ShellProperty * ShellPawn::NewProperty()
{
	return gShellPropertyAllocator.Malloc();
}

ControlItem * ShellPawn::RootControl()
{
	return m_arr_ControlItem[CONTROLITEM_INDEX_SHELL_ROOT];
}


//*************************弹药单位生成模板*************************************
ShellPawnTemplate::ShellPawnTemplate()
{
}

ShellPawnTemplate::~ShellPawnTemplate()
{
}

BasePawn * ShellPawnTemplate::CreatePawn(PawnProperty* pProperty, Scence* pScence)
{
	auto pShellProperty = reinterpret_cast<ShellProperty * >(pProperty);
	auto pNewPawn = ShellPawn::PawnAllocator.Malloc();

	pNewPawn->m_pawnType = gShellPawnType;

	if (pProperty == nullptr)
	{
		//使用默认属性。
		pShellProperty = gShellPropertyAllocator.Malloc();

		pShellProperty->	MoveSpeed	= gShellDefaultMoveSpeed;
		pShellProperty->	CurrState	= gShellDefaultState;
		pShellProperty->	StartPos	= gShellDefaultStartPos;
		pShellProperty->	RestDist	= gShellDefaultRestDist;
		pShellProperty->	DeltaDist	= gShellDefaultDeltaDist;
	}

	//设置属性。
	pNewPawn->m_pProperty = pShellProperty;

	//创建一个根控制器。
	pNewPawn->m_arr_ControlItem[CONTROLITEM_INDEX_SHELL_ROOT] =
		pScence->NewControlItem("Shell", "Shell", "box");
	pNewPawn->RootControl()->Show();
	//根据属性中设定的位置，调整子弹的方向
	ReLocateShell(pNewPawn);

	//为根控制器创建一个骨骼。
	pNewPawn->m_arr_Bones[BONE_INDEX_SHELL_ROOT] =
		gBoneCommander->NewBone(pNewPawn->RootControl());

	//创建AI控制单位。
	pNewPawn->m_pAIUnit =
		gAICommander->NewAIUnit(gShellAIControlType, pNewPawn);
	pNewPawn->m_pAIUnit->CurrState = STORY_FRAGMENT_SHELL_MOVE;

	//创建射线检测单位。
	pNewPawn->m_pRayDetect =
		gCollideCommander->NewRay(pNewPawn->RootControl(), 0.0f);

	return pNewPawn;
}

void ShellPawnTemplate::DestoryPawn(BasePawn * pPawn, Scence * pScence)
{
	auto pShell = reinterpret_cast<ShellPawn * >(pPawn);

	//回收属性空间
	gShellPropertyAllocator.Free(pShell->m_pProperty);

	//删除控制器。
	pScence->DeleteControlItem(pShell->RootControl());

	//删除骨骼。
	gBoneCommander->DeleteBone(pShell->m_arr_Bones[BONE_INDEX_SHELL_ROOT]);

	//删除AI控件。
	gAICommander->DeleteAIUnit(pShell->m_pAIUnit);

	//删除射线单位
	gCollideCommander->DeleteRay(pShell->m_pRayDetect);

	//指针置空。
	pShell->m_pProperty = nullptr;
	pShell->m_pAIUnit = nullptr;
	pShell->m_pRayDetect = nullptr;
	for (int i = 0; i < _countof(pShell->m_arr_ControlItem); ++i)
	{
		pShell->m_arr_ControlItem[i] = nullptr;
	}
	for (int i = 0; i < _countof(pShell->m_arr_Bones); ++i)
	{
		pShell->m_arr_Bones[i] = nullptr;
	}


	//回收pawn对象
	ShellPawn::PawnAllocator.Free(pShell);
}

void ShellPawnTemplate::ReLocateShell(ShellPawn * pPawn)
{
	//加载初始的位置。
	XMMATRIX startPos = XMLoadFloat4x4(&pPawn->m_pProperty->StartPos);

	//使用StartPos来设置子弹的初始位置。
	//XMStoreFloat4x4(&pPawn->RootControl()->World, startPos);

	//速度向量。
	XMVECTOR speedVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	//使用初始位置矩阵来旋转速度向量，来获取世界速度向量。
	speedVector = XMVector4Transform(speedVector, startPos);

	//存储速度向量，此刻StartPos的内存将会被speedVector覆盖一部分，
	XMStoreFloat4(&pPawn->m_pProperty->MoveDirectionXYZW, speedVector);


	//存储坐标
	XMStoreFloat3(&pPawn->RootControl()->Translation, startPos.r[3]);
	//计算局部旋转，不考虑绕Z轴的旋转
	float ry = 0.0f;
	float rx = 0.0f;
	RadianOfRotationMatrix(startPos, rx, ry);
	//旋转ShellPawn，让ShellPawn的朝向和速度方向一致。
	pPawn->RootControl()->RotatePitch(rx);
	pPawn->RootControl()->RotateYaw(ry);

	//为了防止初始的时候世界坐标还是位于原点，施加的旋转和位移没能及时的更新到世界矩阵中，
	//这里手动更新一下世界矩阵，
	//注意：这里的更新只是暂时的，在下一个游戏循环的时候，世界矩阵将会使用我们设置过的
	//局部旋转和局部坐标。
	XMStoreFloat4x4(&pPawn->RootControl()->World, startPos);
}

ShellAITemplate::ShellAITemplate()
{
	//普通移动状态。
	StoryFragment moveFragment;
	moveFragment.State = STORY_FRAGMENT_SHELL_MOVE;
	moveFragment.Posibility = 0.5f;
	moveFragment.ConsistTime = 500.0f;
	//没有下一个状态，状态手动转换。
	moveFragment.NextState = STORY_FRAGMENT_NEXT_NONE;

	//跟随状态。
	StoryFragment followFragment;
	moveFragment.State = STORY_FRAGMENT_SHELL_FOLLOW;
	moveFragment.Posibility = 0.5f;
	moveFragment.ConsistTime = 500.0f;
	//没有下一个状态，状态手动转换。
	moveFragment.NextState = STORY_FRAGMENT_NEXT_NONE;

	StoryBoard.push_back(moveFragment);
	StoryBoard.push_back(followFragment);
}

ShellAITemplate::~ShellAITemplate()
{
}

void ShellAITemplate::Runing(
	BasePawn * pPawn, AIStatue statue,
	float consumedTime, const GameTimer& gt)
{
	auto pShell = reinterpret_cast<ShellPawn * >(pPawn);

	switch (statue)
	{
	case STORY_FRAGMENT_SHELL_MOVE:
		//计算当前帧的移动距离，以及剩余的移动距离，
		caculateDeltaDist(pShell, gt);
		//移动状态，向着指定的目标进行移动。
		move(pShell, gt);
		//检测射线碰撞，删除碰撞到的一个Pawn。
		collideDetect(pShell, gt);
		break;

	case STORY_FRAGMENT_SHELL_FOLLOW:
		//跟随状态，不自行移动，但是检测射线碰撞，删除碰撞到的一个Pawn。
		collideDetect(pShell, gt);
		break;

	default:
		ASSERT(false && "ShellPawn处于非法状态，无法执行操作。");
		break;
	}
}

void ShellAITemplate::move(ShellPawn * pShell, const GameTimer & gt)
{
	//读取速度向量，注：单位向量。
	XMVECTOR speedVector = XMLoadFloat4(
		&pShell->m_pProperty->MoveDirectionXYZW);

	XMFLOAT4 speedVectorFloat4;

	//乘以速度和时间。
	speedVector *= pShell->m_pProperty->DeltaDist;
	XMStoreFloat4(&speedVectorFloat4, speedVector);

	//移动。
	pShell->RootControl()->MoveXYZ(
		speedVectorFloat4.x,
		speedVectorFloat4.y,
		speedVectorFloat4.z);
}

void ShellAITemplate::collideDetect(ShellPawn * pShell, const GameTimer & gt)
{
	if (pShell->m_pProperty->CurrState == ShellState::Dead)
	{
		//弹药死亡，取消碰撞检查。
		return;
	}

	auto rayDetect = pShell->m_pRayDetect;
	gCollideCommander->CollideDetect(rayDetect, COLLIDE_TYPE_BOX_1);

	if (rayDetect->Result.CollideHappended)
	{
		//发生碰撞，删除被碰撞到的Pawn对象。
		gPawnMaster->DestroyPawn(PAWN_TYPE_NONE, rayDetect->Result.pCollideBox->pPawn);
		//删除弹药自身
		gPawnMaster->DestroyPawn(PAWN_TYPE_NONE, pShell);
		//标记属性状态为Dead。
		pShell->m_pProperty->CurrState = ShellState::Dead;
	}
}

void ShellAITemplate::caculateDeltaDist(ShellPawn * pShell, const GameTimer & gt)
{
	float deltaDist =
		pShell->m_pProperty->DeltaDist = 
		pShell->m_pProperty->MoveSpeed * gt.DeltaTime();

	pShell->m_pProperty->RestDist -= deltaDist;

	if (pShell->m_pProperty->RestDist < 0.0f)
	{
		//超出移动距离，删除弹药自身
		gPawnMaster->DestroyPawn(PAWN_TYPE_NONE, pShell);
		//标记属性状态为Dead。
		pShell->m_pProperty->CurrState = ShellState::Dead;
		return;
	}

	pShell->m_pRayDetect->RayLength = deltaDist;
	pShell->m_pRayDetect->MinusLength = - deltaDist / 2;
}
