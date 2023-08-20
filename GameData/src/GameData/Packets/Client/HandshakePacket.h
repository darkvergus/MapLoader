#pragma once

#include <string>
#include <vector>
#include <GameData/Packets\Client/HandshakePacket.h>

namespace Networking
{
	namespace Packets
	{
		namespace Client
		{
			struct HandshakePacket
			{
				struct DataEntry
				{
					std::wstring String;
				};
				
				unsigned char Function = 0;
				std::vector<DataEntry> Data;
			};
		}
	}
}
