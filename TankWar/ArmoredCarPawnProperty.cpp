#include "ArmoredCarPawnProperty.h"

//Pawn����
PawnType						gArmoredCarPawnType = PAWN_TYPE_NONE;
//AI������Ԫ����
AIControlType					gArmoredCarAIControlType = AI_CONTROL_TYPE_NONE;

//��ʼ��Ĭ������
const float gCarDefaultMoveSpeed = 2.0f;
const float gCarDefaultRotationSpeed = 2.0f;

MyStaticAllocator<CarProperty>	gArmoredCarPropertyAllocator(MAX_PAWN_CAR_NUM);