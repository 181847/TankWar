#include "StaticPawnProperty.h"



PawnType gStaticPawnType = PAWN_TYPE_NONE;

MyStaticAllocator<StaticPawnProperty> gStaticPawnPropertyAllocator(MAX_STATIC_PAWN_NUM);
