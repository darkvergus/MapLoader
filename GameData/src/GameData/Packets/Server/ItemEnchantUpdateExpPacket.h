#pragma once

#include <GameData/Enums/Handles.h>

namespace Networking
{
	namespace Packets
	{
		namespace Server
		{
			struct ItemEnchantUpdateExpPacket
			{
				Enum::ItemInstanceId ItemInstanceId = (Enum::ItemInstanceId)0;
				unsigned int EnchantExp = 0;
			};
		}
	}
}