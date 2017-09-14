#include "ArmoredCarPawnProperty.h"

//Pawn类型
PawnType						gArmoredCarPawnType = PAWN_TYPE_NONE;
//AI操作单元类型
AIControlType					gArmoredCarAIControlType = AI_CONTROL_TYPE_NONE;

//初始化默认属性
const float gCarDefaultMoveSpeed = 2.0f;
const float gCarDefaultRotationSpeed = 2.0f;

MyStaticAllocator<CarProperty>	gArmoredCarPropertyAllocator(MAX_PAWN_CAR_NUM);