#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "MyAssert.h"

//分割字符串。
void SplitString(const std::string& source, std::vector<std::string>& output, const std::string& splitBy);

//删除类的复制构造函数和等于号重载函数。
#define DELETE_COPY_CONSTRUCTOR(CLASS) \
	CLASS(const CLASS&) = delete;\
	CLASS& operator = (const CLASS&) = delete;

//计算RM矩阵中代表旋转的俯仰角和偏航角，
// 0 < rx < (pi / 2)。
// 0 < ry < pi。
// RM 4x4的矩阵，不能包含缩放因子，计算时只考虑RM的旋转部分。
void XM_CALLCONV RadianOfRotationMatrix(DirectX::FXMMATRIX RM_in, float& rx_out, float& ry_out);

//这个方程用来计算指定物体到目标点的旋转弧度。
//worldTransform: 指定物体的世界变换矩阵。
//worldTarget :处于世界坐标中的目标平移坐标。
//needRx : 如果这个参数为true，表明需要计算rx，rx的计算涉及开根号，如果非必要，将这个参数设为false，rx将默认为0。
//-(pi / 2) < rx < (pi / 2)。
//-pi < ry < pi。
void XM_CALLCONV OffsetInLocal(
	DirectX::FXMMATRIX	_In_ worldTransform,
	DirectX::FXMVECTOR	_In_ worldTarget,
	bool					 needRx,
	float&				_Out_ rx,
	float&				_Out_ ry);

DirectX::XMMATRIX XM_CALLCONV InvOnlyWithRT(
	DirectX::FXMMATRIX _In_ matrix);

//比较两个float是否相等，M_EPSINON为精度。
bool FloatEqual(float target, float compare, float M_EPSINON);

DirectX::XMVECTOR XM_CALLCONV ConvertFromWorldToLocal(
	DirectX::FXMMATRIX worldTransformation, DirectX::CXMVECTOR worldPosition);
