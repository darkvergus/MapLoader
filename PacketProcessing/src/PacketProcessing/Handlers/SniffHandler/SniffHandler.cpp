#include "SniffHandler-decl.h"

#include <ParserUtils/PacketParsing.h>

namespace Networking
{
	namespace Packets
	{
		ParserUtils::DataStream& SniffHandler::ResetPacketStream()
		{
			StreamStack.clear();
			StreamStack.push_back({});

			return PacketStream();
		}

		void SniffHandler::PushStream(size_t size, bool isDeflated)
		{
			{
				ParserUtils::DataStream& oldTop = PacketStream();

				if (oldTop.HasRecentlyFailed)
				{
					return;
				}

				if (oldTop.Index + size > oldTop.Data.size())
				{
					oldTop.HasRecentlyFailed = true;

					std::cout << "attempt to read buffer of size " << size << " when only " << (oldTop.Data.size() - oldTop.Index) << " bytes are left" << std::endl;

					return;
				}
			}

			StreamStack.push_back({});

			StackEntry& entry = StreamStack.back();

			
			ParserUtils::DataStream& oldTop = StreamStack[StreamStack.size() - 2].PacketStream;

			if (isDeflated)
			{
				entry.PacketStream = ParserUtils::DataStream{ { oldTop.Data.data() + oldTop.Index, size } };
			}
			else
			{

			}

			ParserUtils::DataStream& newTop = PacketStream();

			newTop.IgnoreUnknownValues = oldTop.IgnoreUnknownValues;
			newTop.SuppressErrors = oldTop.SuppressErrors;
			newTop.PrintOutput = oldTop.PrintOutput;

			oldTop.Index += size;
		}

		void SniffHandler::PopStream()
		{
			if (StreamStack.size() == 1)
			{
				PacketStream().HasRecentlyFailed = true;

				std::cout << "attempt to pop packet stream buffer when no buffers are left to pop" << std::endl;

				return;
			}

			ParserUtils::DataStream& oldTop = PacketStream();
			ParserUtils::DataStream& newTop = StreamStack[StreamStack.size() - 2].PacketStream;

			if (!newTop.HasRecentlyFailed && oldTop.Index < oldTop.Data.size())
			{
				std::cout << "popping buffer when not all bytes have been read from it. bytes remaining: " << (oldTop.Data.size() - oldTop.Index) << std::endl;

				if constexpr (ParserUtils::Packets::PrintOutput || ParserUtils::Packets::PrintErrors)
				{
					if (!(!ParserUtils::Packets::PrintOutput && (PacketStream().SuppressErrors || !PacketStream().PrintOutput)))
					{
						std::ostream& out = ParserUtils::Packets::PrintOutput ? std::cout : FoundValues;
						const ParserUtils::DataStream& stream = PacketStream();

						size_t size = stream.Data.size();
						size_t printAmount = size - stream.Index;
						size_t printCount = 16 * ((printAmount / 16) + (printAmount % 16 ? 1 : 0));

						out << "remaining stream data [" << (printAmount) << "]\n" << std::hex;

						for (size_t i = 0; i <= printCount; ++i)
						{
							size_t index = stream.Index + i;

							if (i && (i % 16) == 0)
							{
								out << " |  " << std::dec;

								for (size_t j = 0; j < 16 && index - 16 + j < size; ++j)
									out << (stream.Data[index - 16 + j] >= 32 && stream.Data[index - 16 + j] <= 126 ? stream.Data[index - 16 + j] : '.');

								out << "\n" << std::hex;
							}

							if (index < size)
							{
								out << std::setw(2) << std::setfill('0') << (int)(unsigned char)stream.Data[index] << " ";
							}
							else if (i < printCount)
							{
								out << "   ";
							}
						}

						out << std::endl;
					}
				}

				newTop.Index -= oldTop.Data.size() - oldTop.Index;
			}

			newTop.HasRecentlyFailed = oldTop.HasRecentlyFailed || oldTop.Index < oldTop.Data.size();
			newTop.FoundUnknownValue = oldTop.FoundUnknownValue;

			StreamStack.pop_back();
		}

		void SniffHandler::CheckStreamStatus()
		{
			if (!StreamStack.size())
			{
				ResetPacketStream();
			}

			if (StreamStack.size() == 1)
			{
				return;
			}

			PacketStream().HasRecentlyFailed = true;

			while (StreamStack.size() > 1)
			{
				PopStream();
			}
		}

		bool SniffHandler::IsNpcBoss(Enum::NpcId npcId) const
		{
			if (Data == nullptr)
			{
				return false;
			}

			const auto npcHandle = Data->Npcs.find(npcId);

			if (npcHandle == Data->Npcs.end())
			{
				return false;
			}

			return npcHandle->second.Class >= 3 && npcHandle->second.NpcType == 0;
		}

		bool SniffHandler::NpcHasHiddenHp(Enum::NpcId npcId) const
		{
			return false;

			if (Data == nullptr)
			{
				return false;
			}

			const auto npcHandle = Data->Npcs.find(npcId);

			if (npcHandle == Data->Npcs.end())
			{
				return false;
			}

			return npcHandle->second.HasHiddenHp;
		}

		unsigned char SniffHandler::GetItemExtraDataType(unsigned int itemId) const
		{
			if (Data == nullptr)
			{
				return 0;
			}

			const auto itemHandle = Data->Items.find((Enum::ItemId)itemId);

			if (itemHandle == Data->Items.end())
			{
				return 0;
			}

			if (itemHandle->second.HasBlueprint || itemHandle->second.HasTemplate)
			{
				return 1;
			}

			if (itemHandle->second.PetId != 0)
			{
				return 2;
			}

			if (itemHandle->second.IsCustomNote)
			{
				return 3;
			}

			if (itemHandle->second.IsBadge)
			{
				return 4;
			}

			return 0;
		}

		unsigned short SniffHandler::GetItemCategory(Enum::ItemId itemId) const
		{
			return (unsigned short)((unsigned int)itemId / 100000);
		}

		unsigned short SniffHandler::GetItemCategory(unsigned int itemId) const
		{
			return (unsigned short)(itemId / 100000);
		}

		bool SniffHandler::StatIntToFloat(float& rate) const
		{
			rate /= 100;

			return true;
		}
	}
}