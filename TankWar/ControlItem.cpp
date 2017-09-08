#include "ControlItem.h"

extern const int gNumFrameResources;

//增加偏航角
void ControlItem::RotateYaw(float d)
{
	Rotation.y += d;
	NumFramesDirty = gNumFrameResources;
}

//增加俯仰角
void ControlItem::RotatePitch(float d)
{
	Rotation.x += d;
	NumFramesDirty = gNumFrameResources;
}

//增加滚动角
void ControlItem::RotateRoll(float d)
{
	this->Rotation.z += d;
	NumFramesDirty = gNumFrameResources;
}

//向X轴正向移动。
void ControlItem::MoveX(float d)
{
	this->Translation.x += d;
	NumFramesDirty = gNumFrameResources;
}

//向Y轴正向移动。
void ControlItem::MoveY(float d)
{
	this->Translation.y += d;
	NumFramesDirty = gNumFrameResources;
}

//向Z轴正向移动。
void ControlItem::MoveZ(float d)
{
	this->Translation.z += d;
	NumFramesDirty = gNumFrameResources;
}

void ControlItem::MoveXYZ(float dx, float dy, float dz)
{
	this->Translation.x += dx;
	this->Translation.y += dy;
	this->Translation.z += dz;
	NumFramesDirty = gNumFrameResources;
}

void ControlItem::RotateXYZ(float dx, float dy, float dz)
{
	Rotation.x += dx;
	Rotation.y += dy;
	Rotation.z += dz;
	NumFramesDirty = gNumFrameResources;
}

void ControlItem::ClearRotation()
{
	Rotation = { 0.0f, 0.0f, 0.0f };
	NumFramesDirty = gNumFrameResources;
}

void ControlItem::ClearTranslation()
{
	Translation = { 0.0f, 0.0f, 0.0f };
	NumFramesDirty = gNumFrameResources;
}

//将ControlItemd的显示属性设为隐藏。
void ControlItem::Hide()
{
	//0xfe = b 1111 1110，
	//最后一个字设为0。
	this->Property &= 0xfe;
}

//将ControlItemd的显示属性设为显示。
void ControlItem::Show()
{
	//0x01 = b 0000 0001，
	//最后一个字设为1。
	this->Property |= 0x01;
}