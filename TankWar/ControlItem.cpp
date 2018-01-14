#include "ControlItem.h"

extern const int gNumFrameResources;

//����ƫ����
void ControlItem::RotateYaw(float d)
{
	Rotation.y += d;
	NumFramesDirty = gNumFrameResources;
}

//���Ӹ�����
void ControlItem::RotatePitch(float d)
{
	Rotation.x += d;
	NumFramesDirty = gNumFrameResources;
}

//���ӹ�����
void ControlItem::RotateRoll(float d)
{
	this->Rotation.z += d;
	NumFramesDirty = gNumFrameResources;
}

//��X�������ƶ���
void ControlItem::MoveX(float d)
{
	this->Translation.x += d;
	NumFramesDirty = gNumFrameResources;
}

//��Y�������ƶ���
void ControlItem::MoveY(float d)
{
	this->Translation.y += d;
	NumFramesDirty = gNumFrameResources;
}

//��Z�������ƶ���
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

//��ControlItemd����ʾ������Ϊ���ء�
void ControlItem::Hide()
{
	//0xfe = b 1111 1110��
	//���һ������Ϊ0��
	this->Property &= 0xfe;
}

//��ControlItemd����ʾ������Ϊ��ʾ��
void ControlItem::Show()
{
	//0x01 = b 0000 0001��
	//���һ������Ϊ1��
	this->Property |= 0x01;
}