#include "./../SniffHandler.h"

#include <GameData/Packets\Server/EnchantScrollEnchantUiPacket.h>
#include <GameData/Packets\Server/EnchantScrollStatUiPacket.h>
#include <GameData/Packets\Server/EnchantScrollSuccessPacket.h>
#include <GameData/Packets\Server/EnchantScrollResultPacket.h>
#include <GameData/Packets\Server/EnchantScrollRewardPacket.h>

namespace Networking
{
	namespace Packets
	{
		template <>
		void SniffHandler::PacketParsed<Server::EnchantScrollEnchantUiPacket>(const Server::EnchantScrollEnchantUiPacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::EnchantScrollStatUiPacket>(const Server::EnchantScrollStatUiPacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::EnchantScrollSuccessPacket>(const Server::EnchantScrollSuccessPacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::EnchantScrollResultPacket>(const Server::EnchantScrollResultPacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::EnchantScrollRewardPacket>(const Server::EnchantScrollRewardPacket& packet)
		{
			
		}
	}
}