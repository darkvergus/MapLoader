#pragma once

#include "SniffHandler-decl.h"

#include "./../../PacketParserBase.h"

#include <GameData/Packets\Client/RequestQuitPacket.h>
#include <GameData/Packets\Server/UserChatPacket.h>
#include <GameData/Packets\Client/LogInPacket.h>
#include <GameData/Packets\Client/StateSkillPacket.h>
#include <GameData/Packets\Client/ResponseKeyPacket.h>
#include <GameData/Packets\Client/ResponseVersionPacket.h>
#include <GameData/Packets\Server/FieldRemovePlayerPacket.h>
#include <GameData/Packets\Client/HandshakePacket.h>
#include <GameData/Packets\Client/ResponseHeartbeatPacket.h>
#include <GameData/Packets\Client/LogInAuthTokenPacket.h>
#include <GameData/Packets\Client/SkillCastPacket.h>
#include <GameData/Packets\Client/SkillSyncDamagePacket.h>
#include <GameData/Packets\Client/SkillDamagePacket.h>
#include <GameData/Packets\Client/SkillRegionSkillPacket.h>
#include <GameData/Packets\Client/SkillSyncPacket.h>
#include <GameData/Packets\Client/SkillSyncTickPacket.h>
#include <GameData/Packets\Client/SkillCancelPacket.h>
#include <GameData/Packets\Server/Reconnect7Packet.h>
#include <GameData/Packets\Client/NpcTalkClosePacket.h>
#include <GameData/Packets\Client/NpcTalkBeginPacket.h>
#include <GameData/Packets\Client/NpcTalkContinuePacket.h>
#include <GameData/Packets\Client/NpcTalkEnchantUnknownPacket.h>
#include <GameData/Packets\Client/NpcTalkEnchantPacket.h>
#include <GameData/Packets\Client/NpcTalkNextQuestPacket.h>
#include <GameData/Packets\Client/NpcTalkAllianceQuestAcceptPacket.h>
#include <GameData/Packets\Client/NpcTalkAllianceQuestTalkPacket.h>
#include <GameData/Packets\Client/NpcTalkCinematicPacket.h>
#include <GameData/Packets\Server/RequestFieldEnterPacket.h>
#include <GameData/Packets\Client/SystemInfoPacket.h>
#include <GameData/Packets\Server/SkillDamageSyncPacket.h>
#include <GameData/Packets\Server/SkillDamagePacket.h>
#include <GameData/Packets\Server/SkillDamageDotPacket.h>
#include <GameData/Packets\Server/SkillDamageHealPacket.h>
#include <GameData/Packets\Server/SkillDamageRegionPacket.h>
#include <GameData/Packets\Server/SkillDamageTilePacket.h>
#include <GameData/Packets\Server/FieldAddNpc.h>
#include <GameData/Packets\Server/BuffPacket.h>
#include <GameData/Packets\Server/FieldAddItemPacket.h>
#include <GameData/Packets\Server/FieldRemoveItemPacket.h>
#include <GameData/Packets\Server/FieldRemoveNpc.h>
#include <GameData/Packets\Server/TrophyStartPacket.h>
#include <GameData/Packets\Server/TrophyContentPacket.h>
#include <GameData/Packets\Server/TrophyUpdatePacket.h>

namespace Networking
{
	namespace Packets
	{
		template <>
		void SniffHandler::PacketParsed<Client::RequestQuitPacket>(const Client::RequestQuitPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::UserChatPacket>(const Server::UserChatPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::LogInPacket>(const Client::LogInPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::StateSkillPacket>(const Client::StateSkillPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::ResponseKeyPacket>(const Client::ResponseKeyPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::ResponseVersionPacket>(const Client::ResponseVersionPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::FieldRemovePlayerPacket>(const Server::FieldRemovePlayerPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::HandshakePacket>(const Client::HandshakePacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::ResponseHeartbeatPacket>(const Client::ResponseHeartbeatPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::LogInAuthTokenPacket>(const Client::LogInAuthTokenPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::SkillCastPacket>(const Client::SkillCastPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::SkillSyncDamagePacket>(const Client::SkillSyncDamagePacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::SkillDamagePacket>(const Client::SkillDamagePacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::SkillRegionSkillPacket>(const Client::SkillRegionSkillPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::SkillSyncPacket>(const Client::SkillSyncPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::SkillSyncTickPacket>(const Client::SkillSyncTickPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::SkillCancelPacket>(const Client::SkillCancelPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::Reconnect7Packet>(const Server::Reconnect7Packet& packet);

		template <>
		void SniffHandler::PacketParsed<Client::NpcTalkClosePacket>(const Client::NpcTalkClosePacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::NpcTalkBeginPacket>(const Client::NpcTalkBeginPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::NpcTalkContinuePacket>(const Client::NpcTalkContinuePacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::NpcTalkEnchantUnknownPacket>(const Client::NpcTalkEnchantUnknownPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::NpcTalkEnchantPacket>(const Client::NpcTalkEnchantPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::NpcTalkNextQuestPacket>(const Client::NpcTalkNextQuestPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::NpcTalkAllianceQuestAcceptPacket>(const Client::NpcTalkAllianceQuestAcceptPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::NpcTalkAllianceQuestTalkPacket>(const Client::NpcTalkAllianceQuestTalkPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::NpcTalkCinematicPacket>(const Client::NpcTalkCinematicPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::RequestFieldEnterPacket>(const Server::RequestFieldEnterPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Client::SystemInfoPacket>(const Client::SystemInfoPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::SkillDamageSyncPacket>(const Server::SkillDamageSyncPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::SkillDamagePacket>(const Server::SkillDamagePacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::SkillDamageDotPacket>(const Server::SkillDamageDotPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::SkillDamageHealPacket>(const Server::SkillDamageHealPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::SkillDamageRegionPacket>(const Server::SkillDamageRegionPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::SkillDamageTilePacket>(const Server::SkillDamageTilePacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::FieldAddNpc>(const Server::FieldAddNpc& packet);

		template <>
		void SniffHandler::PacketParsed<Server::BuffPacket>(const Server::BuffPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::FieldAddItemPacket>(const Server::FieldAddItemPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::FieldRemoveItemPacket>(const Server::FieldRemoveItemPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::FieldRemoveNpc>(const Server::FieldRemoveNpc& packet);

		template <>
		void SniffHandler::PacketParsed<Server::TrophyStartPacket>(const Server::TrophyStartPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::TrophyContentPacket>(const Server::TrophyContentPacket& packet);

		template <>
		void SniffHandler::PacketParsed<Server::TrophyUpdatePacket>(const Server::TrophyUpdatePacket& packet);

	}
}
