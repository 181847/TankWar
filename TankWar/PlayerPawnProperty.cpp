#include "PlayerPawnProperty.h"

PawnType							gPlayerPawnType				= PAWN_TYPE_NONE;

PlayerControlType					gPlayerControlType			= PLAYER_CONTROL_TYPE_NONE;

AIControlType						gPlayerAIControlType		= AI_CONTROL_TYPE_NONE;

MyStaticAllocator<PlayerProperty>	gPlayerPropertyAllocator	(MAX_PLAYER_PAWN_NUM);

PlayerProperty::PlayerProperty()
{
}
