#include "ShellProperty.h"

PawnType gShellPawnType = PAWN_TYPE_NONE;

AIControlType gShellAIControlType = AI_CONTROL_TYPE_NONE;

float gShellDefaultMoveSpeed = 50.0f;

float gShellDefaultRestDist = 1e5f;

float gShellDefaultDeltaDist = 1.0f;

ShellState gShellDefaultState = ShellState::Move;

XMFLOAT4X4 gShellDefaultStartPos = {	1.0f, 0.0f, 0.0f, 0.0f,
										0.0f, 1.0f, 0.0f, 0.0f,
										0.0f, 0.0f, 1.0f, 0.0f,
										0.0f, 0.0f, 0.0f, 1.0f, };

MyStaticAllocator<ShellProperty> gShellPropertyAllocator(MAX_SHELL_PAWN_NUM);

ShellProperty::ShellProperty()
{
}
