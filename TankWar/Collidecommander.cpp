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
	//������ݡ�
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

	//�Ƿ�������ײ��
	bool isCollided		= false;
	//���μ���Ƿ�����ײ��
	bool ic				= false;
	//��ײ�������ߵ���̾��롣
	float tmin			= 0.0f;
	//������ײ�������ߵľ��롣
	float tl			= 0.0f;
	//��ײ���ꡣ
	XMFLOAT3 loc = {0.0f, 0.0f, 0.0f};

	while (pNode != pHead)
	{
		//ֻ�е���ײ���������ͬ��
		//����ָ����ײ�������ͣ�
		//��ʱ��Ž��м�顣
		if (pNode->element.Type != detectType || detectType != COLLIDE_TYPE_ALL)
		{
			RayCheckWithSphere(
				ray_input_and_output,
				&pNode->element,
				ic,				//��ȡ�Ƿ�������ײ��
				loc,					//��ȡ��ײ�����ꡣ
				tmin);					//��ײ�����ߵľ��롣

			if (ic && isCollided && tl < tmin)
			{
				//���η�����ײ��
				//����������һ����ײ��
				//�˴ε���ײ�������һ�εľ��������
				//������ײ�����
				ray_input_and_output->Result.CollideLocation = loc;
				tmin = tl;
				ray_input_and_output->Result.pCollideBox = &pNode->element;
			}
			else if (ic && !isCollided)
			{
				//���η�����ײ��
				//��ȥû�з�������ײ��
				ray_input_and_output->Result.CollideLocation = loc;
				tmin = tl;
				ray_input_and_output->Result.pCollideBox = &pNode->element;
				isCollided = true;
			}
		}

		pNode = pNode->m_pNext;
	}

	//�鿴�Ƿ�������ײ��
	if (isCollided)
	{
		ray_input_and_output->Result.CollideHappended = true;
	}
}

void CollideCommander::RayCheckWithSphere(RayDetect * rayDetect, CollideBox * pBox,
	bool & isCollided, XMFLOAT3 & location, float& tl)
{
	//���ߵ��������
	XMMATRIX ray = XMLoadFloat4x4(&rayDetect->controlPD->World);

	//������㡣
	XMVECTOR rayLoc = ray.r[3];

	//��ײ���Χ�����ĵ����ꡣ
	XMFLOAT4 sphereLocFloat4 = XMFLOAT4(pBox->pCItem->World.m[3]);
	XMVECTOR sphereLoc = XMLoadFloat4(&sphereLocFloat4);

	//������㵽�����Χ�����ĵ�������
	XMVECTOR locVector = sphereLoc - rayLoc;

	//��������������Ĭ��Z�����������ߵ���ʼ����
	XMFLOAT4 rayVectorFloat4 = { 0, 0, 1, 0 };
	XMVECTOR rayVector = XMLoadFloat4(&rayVectorFloat4);
	rayVector = XMVector4Transform(rayVector, ray);

	//�����Χ�����ĵ������߶εĴ�ֱ�����ƽ����
	locVector = XMVector3Cross(locVector, rayVector);
	locVector = XMVector3LengthSq(locVector);
	
	//��ȡ����ƽ����
	XMFLOAT4 squareDist;
	XMStoreFloat4(&squareDist, locVector);
	//����������ƽ��С�ڰ�Χ��İ뾶ƽ�����ͼ�����鳤���ε���ײ��
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
	//���ߵ��������
	XMMATRIX ray = XMLoadFloat4x4(&rayDetect->controlPD->World);

	//��ײ�е��������
	XMFLOAT4X4 collideWorldFloat4x4 = pBox->pCItem->World;
	inv(collideWorldFloat4x4);
	XMMATRIX collideWorld = XMLoadFloat4x4(&collideWorldFloat4x4);
	//�򵥵Ķ��������ȡ�棬����ֻ������ת��ƽ�ƣ����������׼��㡣

	//�����ߵķ��������ת������ײ�еľֲ������С�
	XMMATRIX newRay = ray * collideWorld;

	//������㡣
	XMVECTOR rPosition = newRay.r[3];

	//�������ߵķ���Ĭ����Z������Ϊ���߷���
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

		//������ײ�����ꡣ
		//�����������㵽��ײ���������
		rDirection = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);		//���ߵ����緽��
		rDirection = XMVector4Transform(rDirection, newRay);
		rDirection *= maxt;

		//��ȡ���ߵ���㡣
		rPosition = ray.r[3];
		
		//��ӵõ���ײ�����ꡣ
		rPosition += rDirection;

		//�洢��ײ�����ꡣ
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
	//����ת�������棬��������ת���ֵ�3x3������ת�ã�
	//ͬʱ��ƽ�Ʋ���ȡ������
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 2; j >= i; --j)
		{
			MySwap(matrixWith_R_T.m[i][j], matrixWith_R_T.m[j][i]);
		}

		//�Ե����е�ǰ����ƽ�Ʒ���ȡ������
		matrixWith_R_T.m[3][i] *= -1;
	}
}

void CollideCommander::Caculate_MaxT_MinT(
	BoxSize&	boxSize,	//��ײ�еĳߴ�
	XMFLOAT4&	rDirection,	//���߷���ֻ��xyz�����������á�
	XMFLOAT4&	rPosition,	//�������ֻ꣬��xyz�����������á�
	float&		maxt,		//�����1��������ߺ���ײ���ཻ��maxtӦ��С��mint��
	float&		mint)		//�����2
{
	//�ֱ��XYZ�����������С�������ײ��ļ���ֵ��
	float	minTermX, maxTermX,
			minTermY, maxTermY,
			minTermZ, maxTermZ;

	minTermX = My_Minus_And_Divide(boxSize.Xmin, rPosition.x, rDirection.x);
	maxTermX = My_Minus_And_Divide(boxSize.Xmax, rPosition.x, rDirection.x);

	minTermY = My_Minus_And_Divide(boxSize.Ymin, rPosition.y, rDirection.y);
	maxTermY = My_Minus_And_Divide(boxSize.Ymax, rPosition.y, rDirection.y);

	minTermZ = My_Minus_And_Divide(boxSize.Zmin, rPosition.z, rDirection.z);
	maxTermZ = My_Minus_And_Divide(boxSize.Zmax, rPosition.z, rDirection.z);


	//��XYZ����ά�ȵķ����ϣ���minTerm��ֵ��С��maxTerm��ֵ���
	//����ֻ����ͬһ��ά���ϡ�
	MIN_SWAP(minTermX, maxTermX);
	MIN_SWAP(minTermY, maxTermY);
	MIN_SWAP(minTermZ, maxTermZ);
	
	//��minTerm���ҵ����ֵ��û���Ƿ������ġ�
	maxt = MAX(MAX(minTermX, minTermY), minTermZ);

	//��maxTerm���ҵ���Сֵ����������
	mint = MIN(MIN(maxTermX, maxTermY), maxTermZ);
}

float CollideCommander::My_Minus_And_Divide(float dist, float rp, float rd)
{
	dist -= rp;

	//distΪ0��ֱ�ӷ���0��
	if (!dist)
	{
		return 0;
	}

	//�������Ϊ0��
	if (rd == 0)
	{
		if (dist < 0)
		{
			//���ظ����
			return -FLT_MIN;
		}
		else
		{
			//���������
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
