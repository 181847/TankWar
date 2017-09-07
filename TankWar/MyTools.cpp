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

	rx_out = asin(unitFloat4.y);
	ry_out = DirectX::XM_PI + atan(unitFloat4.x / unitFloat4.z);

	if (unitFloat4.z < 0)
	{
		//如果Z小于0，表明y轴的旋转角度在(0, -pi)之间，直接加一个pi，修正角度。
		ry_out += DirectX::XM_PI;
	}
}

void XM_CALLCONV OffsetInLocal(DirectX::FXMMATRIX worldTransform, DirectX::FXMVECTOR worldTarget, bool needRx, float & rx, float & ry)
{
	//转换到局部坐标中。
	auto target = DirectX::XMVector2Transform(worldTarget, InvOnlyWithRT(worldTransform));

	DirectX::XMFLOAT3 targetFloat3;
	DirectX::XMStoreFloat3(&targetFloat3, target);

	rx = 0;
	ry = atan(targetFloat3.x / targetFloat3.z);
	//注：这里ry的结果在(-pi/2, pi/2)

	//修正ry的范围到(-pi, pi)
	if (targetFloat3.x > 0 && targetFloat3.z < 0)
	{
		ry -= DirectX::XM_PI;
	}
	else if (targetFloat3.x < 0 && targetFloat3.z < 0)
	{
		ry += DirectX::XM_PI;
	}

	if (needRx)
	{
		//单位化。
		target = DirectX::XMVector3Normalize(target);
		DirectX::XMStoreFloat3(&targetFloat3, target);
		rx = -asin(targetFloat3.y);
	}
	
}

DirectX::XMMATRIX XM_CALLCONV InvOnlyWithRT(DirectX::FXMMATRIX matrix)
{
	auto m = matrix;

	//清空平移。
	m.r[3] = DirectX::g_XMZero;
	//转置，求旋转的逆矩阵。
	m = DirectX::XMMatrixTranspose(m);

	//求平移的逆。
	auto translationMask = DirectX::XMVectorSet(-1.0f, -1.0f, -1.0f, 1.0f);
	m.r[3] = DirectX::XMVectorMultiply(matrix.r[3], translationMask);

	return m;
}

bool FloatEqual(float target, float compare, float M_EPSINON)
{
	return target >= (compare - M_EPSINON) && target <= (compare + M_EPSINON);
}
