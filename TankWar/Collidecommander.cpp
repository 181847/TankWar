#include "CollideCommander.h"


CollideCommander::CollideCommander(UINT MaxRayCount, UINT MaxBoxCount)
	:RayAllocator(MaxRayCount), BoxAllocator(MaxBoxCount)
{
}

CollideCommander::~CollideCommander()
{
}

RayDetect * CollideCommander::NewRay(ControlItem * pCItem, float rayLength)
{
	auto pNewRay = RayAllocator.Malloc();
	pNewRay->RayLength					= rayLength;
	pNewRay->controlPD					= pCItem;
	pNewRay->Result.CollideHappended	= false;
	pNewRay->Result.CollideLocation		= { 0.0f, 0.0f, 0.0f };
	pNewRay->Result.pCollideBox			= nullptr;
	return pNewRay;
}

void CollideCommander::DeleteRay(RayDetect * pDeleteRay)
{
	//清空数据。
	pDeleteRay->RayLength				= 0.0f;
	pDeleteRay->controlPD				= nullptr;
	pDeleteRay->Result.CollideHappended = false;
	pDeleteRay->Result.pCollideBox		= nullptr;
	RayAllocator.Free(pDeleteRay);
}

CollideBox * CollideCommander::NewCollideBox(
	CollideType type, ControlItem * pCItem, BasePawn * pPawn)
{
	auto pNewBox = BoxAllocator.Malloc();
	pNewBox->pCItem				= pCItem;
	pNewBox->pPawn				= pPawn;
	pNewBox->Type				= type;
	pNewBox->SquareSphereRadius = 0.0f;
	pNewBox->Size.Xmin			= 0.0f;
	pNewBox->Size.Xmax			= 0.0f;
	pNewBox->Size.Ymin			= 0.0f;
	pNewBox->Size.Ymax			= 0.0f;
	pNewBox->Size.Zmin			= 0.0f;
	pNewBox->Size.Zmax			= 0.0f;

	return pNewBox;
}

void CollideCommander::DeleteCollideBox(CollideBox * pBox)
{

	pBox->pCItem				= nullptr;
	pBox->pPawn					= nullptr;
	pBox->Type					= COLLIDE_RECT_TYPE_NONE;
	pBox->SquareSphereRadius	= 0.0f;
	pBox->Size.Xmin				= 0.0f;
	pBox->Size.Xmax				= 0.0f;
	pBox->Size.Ymin				= 0.0f;
	pBox->Size.Ymax				= 0.0f;
	pBox->Size.Zmin				= 0.0f;
	pBox->Size.Zmax				= 0.0f;
	BoxAllocator.Free(pBox);
}

void CollideCommander::CollideDetect(RayDetect * ray_input_and_output, CollideType detectType)
{
	auto pHead = BoxAllocator.GetHead();
	auto pNode = pHead->m_pNext;

	//是否发生过碰撞。
	bool isCollided		= false;
	//单次检查是否发生碰撞。
	bool ic				= false;
	//碰撞点与射线的最短距离。
	float tmin			= 0.0f;
	//单次碰撞点与射线的距离。
	float tl			= 0.0f;
	//碰撞坐标。
	XMFLOAT3 loc = {0.0f, 0.0f, 0.0f};

	while (pNode != pHead)
	{
		//只有当碰撞体的类型相同，
		//或者指定碰撞所有类型，
		//的时候才进行检查。
		if (pNode->element.Type != detectType || detectType != COLLIDE_TYPE_ALL)
		{
			RayCheckWithSphere(
				ray_input_and_output,
				&pNode->element,
				ic,				//获取是否发生了碰撞。
				loc,					//获取碰撞的坐标。
				tmin);					//碰撞到射线的距离。

			if (ic && isCollided && tl < tmin)
			{
				//本次发生碰撞。
				//曾经发生过一次碰撞。
				//此次的碰撞相比于上一次的距离更近。
				//更新碰撞结果。
				ray_input_and_output->Result.CollideLocation = loc;
				tmin = tl;
				ray_input_and_output->Result.pCollideBox = &pNode->element;
			}
			else if (ic && !isCollided)
			{
				//本次发生碰撞。
				//过去没有发生过碰撞。
				ray_input_and_output->Result.CollideLocation = loc;
				tmin = tl;
				ray_input_and_output->Result.pCollideBox = &pNode->element;
				isCollided = true;
			}
		}

		pNode = pNode->m_pNext;
	}

	//查看是否发生过碰撞。
	if (isCollided)
	{
		ray_input_and_output->Result.CollideHappended = true;
	}
}

void CollideCommander::RayCheckWithSphere(RayDetect * rayDetect, CollideBox * pBox,
	bool & isCollided, XMFLOAT3 & location, float& tl)
{
	//射线的世界矩阵。
	XMMATRIX ray = XMLoadFloat4x4(&rayDetect->controlPD->World);

	//射线起点。
	XMVECTOR rayLoc = ray.r[3];

	//碰撞体包围球中心点坐标。
	XMFLOAT4 sphereLocFloat4 = XMFLOAT4(pBox->pCItem->World.m[3]);
	XMVECTOR sphereLoc = XMLoadFloat4(&sphereLocFloat4);

	//射线起点到计算包围球中心的向量。
	XMVECTOR locVector = sphereLoc - rayLoc;

	//计算射线向量，默认Z轴正向是射线的起始方向
	XMFLOAT4 rayVectorFloat4 = { 0, 0, 1, 0 };
	XMVECTOR rayVector = XMLoadFloat4(&rayVectorFloat4);
	rayVector = XMVector4Transform(rayVector, ray);

	//计算包围球中心到射线线段的垂直距离的平方。
	locVector = XMVector3Cross(locVector, rayVector);
	locVector = XMVector3LengthSq(locVector);
	
	//提取距离平方。
	XMFLOAT4 squareDist;
	XMStoreFloat4(&squareDist, locVector);
	//如果这个距离平方小于包围球的半径平方，就继续检查长方形的碰撞。
	if (squareDist.x < pBox->SquareSphereRadius)
	{
		RayCollideCheck(rayDetect, pBox, isCollided, location, tl);
	}
	else 
	{
		isCollided = false;
		location = { 0.0f, 0.0f, 0.0f };
		tl = 0.0f;
	}
}

void CollideCommander::RayCollideCheck(RayDetect * rayDetect, CollideBox * pBox, 
	bool & isCollided, XMFLOAT3 & location, float& tl)
{
	//射线的世界矩阵。
	XMMATRIX ray = XMLoadFloat4x4(&rayDetect->controlPD->World);

	//碰撞盒的世界矩阵。
	XMFLOAT4X4 collideWorldFloat4x4 = pBox->pCItem->World;
	inv(collideWorldFloat4x4);
	XMMATRIX collideWorld = XMLoadFloat4x4(&collideWorldFloat4x4);
	//简单的对世界矩阵取逆，由于只包含旋转和平移，逆矩阵很容易计算。

	//将射线的方向和坐标转换到碰撞盒的局部坐标中。
	XMMATRIX newRay = ray * collideWorld;

	//射线起点。
	XMVECTOR rPosition = newRay.r[3];

	//计算射线的方向，默认以Z轴正向为射线方向。
	XMVECTOR rDirection = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	rDirection = XMVector4Transform(rDirection, newRay);

	XMFLOAT4 rDirectionFloat4;
	XMStoreFloat4(&rDirectionFloat4, rDirection);

	XMFLOAT4 rPositionFloat4;
	XMStoreFloat4(&rPositionFloat4, rPosition);

	float maxt, mint;
	Caculate_MaxT_MinT(pBox->Size, rDirectionFloat4, rPositionFloat4, maxt, mint);

	if (maxt < mint && maxt >= 0 && maxt < rayDetect->RayLength)
	{
		isCollided = true;

		//计算碰撞点坐标。
		//计算从射线起点到碰撞点的向量。
		rDirection = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);		//射线的世界方向。
		rDirection = XMVector4Transform(rDirection, newRay);
		rDirection *= maxt;

		//获取射线的起点。
		rPosition = ray.r[3];
		
		//相加得到碰撞点坐标。
		rPosition += rDirection;

		//存储碰撞点坐标。
		XMStoreFloat4(&rPositionFloat4, rPosition);
		location.x = rPositionFloat4.x;
		location.y = rPositionFloat4.y;
		location.z = rPositionFloat4.z;
		
		tl = maxt;
	} 
	else
	{
		isCollided = true;
		location.x = location.y = location.z = 0.0f;
		tl = 0.0f;
	}
}

void CollideCommander::inv(XMFLOAT4X4 & matrixWith_R_T)
{
	//对旋转部分求逆，即，将旋转部分的3x3矩阵求转置，
	//同时对平移部分取负数。
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 2; j >= i; --j)
		{
			MySwap(matrixWith_R_T.m[i][j], matrixWith_R_T.m[j][i]);
		}

		//对第四行的前三个平移分量取负数。
		matrixWith_R_T.m[3][i] *= -1;
	}
}

void CollideCommander::Caculate_MaxT_MinT(
	BoxSize&	boxSize,	//碰撞盒的尺寸
	XMFLOAT4&	rDirection,	//射线方向，只有xyz三个分量有用。
	XMFLOAT4&	rPosition,	//射线坐标，只有xyz三个分量有用。
	float&		maxt,		//交叉点1，如果射线和碰撞盒相交，maxt应该小于mint。
	float&		mint)		//交叉点2
{
	//分别对XYZ三个方向的最小和最大碰撞面的计算值。
	float	minTermX, maxTermX,
			minTermY, maxTermY,
			minTermZ, maxTermZ;

	minTermX = My_Minus_And_Divide(boxSize.Xmin, rPosition.x, rDirection.x);
	maxTermX = My_Minus_And_Divide(boxSize.Xmax, rPosition.x, rDirection.x);

	minTermY = My_Minus_And_Divide(boxSize.Ymin, rPosition.y, rDirection.y);
	maxTermY = My_Minus_And_Divide(boxSize.Ymax, rPosition.y, rDirection.y);

	minTermZ = My_Minus_And_Divide(boxSize.Zmin, rPosition.z, rDirection.z);
	maxTermZ = My_Minus_And_Divide(boxSize.Zmax, rPosition.z, rDirection.z);


	//在XYZ三个维度的分量上，让minTerm的值最小，maxTerm的值最大，
	//交换只限于同一个维度上。
	MIN_SWAP(minTermX, maxTermX);
	MIN_SWAP(minTermY, maxTermY);
	MIN_SWAP(minTermZ, maxTermZ);
	
	//在minTerm中找到最大值，没错，是反着来的。
	maxt = MAX(MAX(minTermX, minTermY), minTermZ);

	//在maxTerm中找到最小值，反着来。
	mint = MIN(MIN(maxTermX, maxTermY), maxTermZ);
}

float CollideCommander::My_Minus_And_Divide(float dist, float rp, float rd)
{
	dist -= rp;

	//dist为0，直接返回0。
	if (!dist)
	{
		return 0;
	}

	//如果除数为0。
	if (rd == 0)
	{
		if (dist < 0)
		{
			//返回负无穷。
			return -FLT_MIN;
		}
		else
		{
			//返回正无穷。
			return FLT_MAX;
		}
	}
	return dist / rd;
}

void CollideBox::CaculateRadius()
{
	float x = MAX(this->Size.Xmax, this->Size.Xmin);
	float y = MAX(this->Size.Ymax, this->Size.Ymin);
	float z = MAX(this->Size.Zmax, this->Size.Zmin);
	this->SquareSphereRadius = x * x + y * y + z * z;
}
