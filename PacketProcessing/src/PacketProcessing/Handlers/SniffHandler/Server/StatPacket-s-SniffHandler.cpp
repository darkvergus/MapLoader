#include "./../SniffHandler.h"

#include <ParserUtils/PacketParsing.h>

namespace Networking
{
	namespace Packets
	{
		template <>
		void SniffHandler::PacketParsed<Server::StatPacket>(const Server::StatPacket& packet)
		{
			if (!Field.CurrentMap)
			{
				return;
			}

			const auto entry = Field.Actors.find(packet.ActorId);

			if (entry == Field.Actors.end())
			{
				if constexpr (ParserUtils::Packets::PrintUnknownValues)
				{
					if (!Field.PrintedMap)
					{
						Field.PrintedMap = true;

						std::cout << "entered map [" << (unsigned int)Field.MapId << "] '" << Field.CurrentMap->Name << "'" << std::endl;
					}

					PacketStream().FoundUnknownValue = true;

					std::cout << "changing stats of unknown actor: " << (unsigned int)packet.ActorId << std::endl;
				}

				return;
			}

			const Actor& actor = entry->second;

			if (!Field.Players.contains(packet.ActorId))
			{
				return;
			}

			const Player& player = Field.Players[packet.ActorId];

			CheckBaseStats(player.JobCode, actor.Level, packet.Stats, Version != 12);
		}
	}
}
