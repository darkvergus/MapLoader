#include "./../SniffHandler.h"

#include <GameData/Packets\Client/ReconnectData.h>

namespace Networking
{
	namespace Packets
	{
		template <>
		void SniffHandler::PacketParsed<Client::ReconnectPacket>(const Client::ReconnectPacket& packet)
		{
			CharacterId = packet.CharacterId;
			AccountId = packet.AccountId;
		}
	}
}
