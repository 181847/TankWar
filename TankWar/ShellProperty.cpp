#include "ShellProperty.h"

PawnType ShellType = PAWN_TYPE_NONE;

MyStaticAllocator<ShellProperty> gShellPropertyAllocator(MAX_SHELL_PAWN_NUM);