#include "./../SniffHandler.h"

namespace Networking
{
	namespace Packets
	{
		template <>
		void SniffHandler::PacketParsed<Server::CharacterListLoadPacket>(const Server::CharacterListLoadPacket& packet)
		{
			
		}
	}
}