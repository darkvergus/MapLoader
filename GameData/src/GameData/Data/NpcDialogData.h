#pragma once

#include <GameData/Enums/Handles.h>

namespace Maple
{
	namespace Game
	{
		struct NpcDialogData
		{
			Enum::ScriptId ScriptId = (Enum::ScriptId)0;
			unsigned int ScriptIndex = 0;
			unsigned int Options = 0;
		};
	}
}