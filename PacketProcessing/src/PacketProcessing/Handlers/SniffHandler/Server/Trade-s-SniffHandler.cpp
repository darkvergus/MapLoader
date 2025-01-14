#include "./../SniffHandler.h"

#include <GameData/Packets\Server/TradeData.h>
#include <ParserUtils/PacketParsing.h>

namespace Networking
{
	namespace Packets
	{
		template <>
		void SniffHandler::PacketParsed<Server::TradeReceiveRequestPacket>(const Server::TradeReceiveRequestPacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::TradeErrorPacket>(const Server::TradeErrorPacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::TradeConfirmRequestPacket>(const Server::TradeConfirmRequestPacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::TradeDeclinedPacket>(const Server::TradeDeclinedPacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::TradeBeginPacket>(const Server::TradeBeginPacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::TradeEndPacket>(const Server::TradeEndPacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::TradeAddItemPacket>(const Server::TradeAddItemPacket& packet)
		{
			Item* item = RegisterItem(packet.ItemInstanceId, packet.ItemId);

			item->Amount = packet.Amount;
			item->Rarity = packet.Rarity;

			if constexpr (ParserUtils::Packets::PrintPacketOutput)
			{
				std::cout << TimeStamp << "adding item to trade window " << PrintItem{ Field, packet.ItemInstanceId } << std::endl;
				std::cout << PrintItemStats{ Field, packet.ItemInstanceId };
			}
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::TradeRemoveItemPacket>(const Server::TradeRemoveItemPacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::TradeSetMoneyPacket>(const Server::TradeSetMoneyPacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::TradeFinalizePacket>(const Server::TradeFinalizePacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::TradeChangeOfferPacket>(const Server::TradeChangeOfferPacket& packet)
		{
			
		}
	

		template <>
		void SniffHandler::PacketParsed<Server::TradeFinalizeConfirmPacket>(const Server::TradeFinalizeConfirmPacket& packet)
		{
			
		}
	}
}
