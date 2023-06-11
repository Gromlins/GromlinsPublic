#pragma once
#include "interface.h"
#include "entity.h"

namespace aimbot
{
	bool is_in_crosshair(entity_t);
	void init(entity_t, entity_t, int);
}