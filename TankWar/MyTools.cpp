#include "MyTools.h"

void SplitString(const std::string& source, std::vector<std::string>& output, const std::string& splitBy)
{
	output.clear();
	std::string::size_type pos1, pos2;
	pos2 = source.find(splitBy);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		output.push_back(source.substr(pos1, pos2 - pos1));

		pos1 = pos2 + splitBy.size();
		pos2 = source.find(splitBy, pos1);
	}
	if (pos1 != source.length())
		output.push_back(source.substr(pos1));
}

void XM_CALLCONV RadianOfRotationMatrix(DirectX::FXMMATRIX RM_in, float& rx_out, float& ry_out)
{
	auto unitVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	unitVector = DirectX::XMVector4Transform(unitVector, RM_in);

	DirectX::XMFLOAT4 unitFloat4;
	DirectX::XMStoreFloat4(&unitFloat4, unitVector);

	rx_out = -asin(unitFloat4.y);

	if (unitFloat4.z == 0.0f)
	{
		//����Ϊ����������
		if (unitFloat4.x > 0.0f)
		{
			ry_out = DirectX::XM_PIDIV2;
		}
		else
		{
			ry_out = -DirectX::XM_PIDIV2;
		}
	}
	else
	{
		ry_out = atan(unitFloat4.x / unitFloat4.z);
	}


	//����ry�ķ�Χ��(-pi, pi)
	if (unitFloat4.x > 0 && unitFloat4.z < 0)
	{
		ry_out += DirectX::XM_PI;
	}
	else if (unitFloat4.x < 0 && unitFloat4.z < 0)
	{
		ry_out = -DirectX::XM_PI + ry_out;
	}
	//
	//if (unitFloat4.z < 0)
	//{
		//���ZС��0������y�����ת�Ƕ���(0, -pi)֮�䣬ֱ�Ӽ�һ��pi�������Ƕȡ�
	//	ry_out += DirectX::XM_PI;
	//}
}

void XM_CALLCONV OffsetInLocal(DirectX::FXMMATRIX worldTransform, DirectX::FXMVECTOR worldTarget, bool needRx, float & rx, float & ry)
{
	//ת�����ֲ������С�
	auto target = ConvertFromWorldToLocal(
		worldTransform, worldTarget);

	DirectX::XMFLOAT3 targetFloat3;
	DirectX::XMStoreFloat3(&targetFloat3, target);

	rx = 0;

	if (targetFloat3.z == 0.0f)
	{
		if (targetFloat3.x > 0.0f)
		{
			ry = DirectX::XM_PIDIV2;
		}
		else
		{
			ry = -DirectX::XM_PIDIV2;
		}
	}
	ry = atan(targetFloat3.x / targetFloat3.z);
	//ע������ry�Ľ����(-pi/2, pi/2)

	//����ry�ķ�Χ��(-pi, pi)
	if (targetFloat3.x > 0 && targetFloat3.z < 0)
	{
		ry += DirectX::XM_PI;
	}
	else if (targetFloat3.x < 0 && targetFloat3.z < 0)
	{
		ry = - DirectX::XM_PI + ry;
	}

	if (needRx && 
		(ry < DirectX::XM_PIDIV2 && ry > -DirectX::XM_PIDIV2))
	{
		//��������С�ںʹ��ڶ�Ӧ�������Ŀ���ڱ���ĳ�����
		//���ʱ����Ҫ��x����ת����Ϊ���ڱ��󣬶�rx�޶���(-pi/2, pi/2)֮�䣬
		//������ͨ����תX����ָ��Ŀ�ֻ꣬���ȿ�Y����ת������
		//���Ե�Ŀ���ڱ����ʱ�򣬲���תX�ᡣ

		//��λ����
		target = DirectX::XMVector3Normalize(target);
		DirectX::XMStoreFloat3(&targetFloat3, target);
		rx = -asin(targetFloat3.y);
	}
	
}

DirectX::XMMATRIX XM_CALLCONV InvOnlyWithRT(DirectX::FXMMATRIX matrix)
{
	auto m = matrix;

	//���ƽ�ơ�
	m.r[3] = DirectX::g_XMZero;
	//ת�ã�����ת�������
	m = DirectX::XMMatrixTranspose(m);

	//��ƽ�Ƶ��档
	auto translationMask = DirectX::XMVectorSet(-1.0f, -1.0f, -1.0f, 1.0f);
	m.r[3] = DirectX::XMVectorMultiply(matrix.r[3], translationMask);

	return m;
}

bool FloatEqual(float target, float compare, float M_EPSINON)
{
	return target >= (compare - M_EPSINON) && target <= (compare + M_EPSINON);
}

DirectX::XMVECTOR XM_CALLCONV ConvertFromWorldToLocal(
	DirectX::FXMMATRIX worldTransformation, DirectX::CXMVECTOR worldPosition)
{
	auto returnV = worldPosition;
	auto invM = worldTransformation;

	//���ƽ�ơ�
	invM.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	//ת�ã�����ת�������
	invM = DirectX::XMMatrixTranspose(invM);

	//��ƽ�Ƶ��档
	auto invT = DirectX::XMVectorSet(-1.0f, -1.0f, -1.0f, 0.0f);
	invT = DirectX::XMVectorMultiply(worldTransformation.r[3], invT);

	returnV = DirectX::XMVectorAdd(worldPosition, invT);

	returnV = DirectX::XMVector4Transform(returnV, invM);

	return returnV;
}
