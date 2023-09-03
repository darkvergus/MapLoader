#include "PacketParser.h"

#include <vector>

#include <ParserUtils/PacketParsing.h>

namespace Networking
{
	namespace Packets
	{
		void ParseBasicStat_v12(PacketHandler& handler, Maple::Game::BasicStat& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<short>("type", handler, blockOutput.Type, "\t\t");
			Read<int>("value", handler, blockOutput.Value, "\t\t");
			Read<float>("rate", handler, blockOutput.Rate, "\t\t");
		}

		void ParseSpecialStat_v12(PacketHandler& handler, Maple::Game::SpecialStat& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<short>("type", handler, blockOutput.Type, "\t\t");
			Read<int>("value", handler, blockOutput.Value, "\t\t");
			Read<float>("rate", handler, blockOutput.Rate, "\t\t");
		}

		void ParseBasicAndSpecialStats_v12(PacketHandler& handler, Maple::Game::BonusStats& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			short statCount_var0 = 0;
			Read<short>("statCount", handler, statCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Basic, statCount_var0);

			for (short i = 0; i < statCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				ParseBasicStat_v12(handler, blockOutput.Basic[i]);
			}

			Read<short>("statCount", handler, statCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Special, statCount_var0);

			for (short i = 0; i < statCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				ParseSpecialStat_v12(handler, blockOutput.Special[i]);
			}

			Read<int>("empowermentItemId", handler, blockOutput.EmpowermentItemId, "\t\t");
		}

		void ParseItemBinding_v12(PacketHandler& handler, Maple::Game::ItemBinding& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<long long>("characterId", handler, blockOutput.CharacterId, "\t\t");
			Read<std::wstring>("name", handler, blockOutput.Name, "\t\t");
		}

		void ParseItemCustomization_v12(PacketHandler& handler, Maple::Game::ItemCustomization& blockOutput, unsigned int itemId_param0)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();


			{
				Color4I_BGRA dyeColorPrimary_var0;
				Read<Color4I_BGRA>("dyeColorPrimary", handler, dyeColorPrimary_var0, "\t\t\t");

				blockOutput.DyeColor.Primary = (Color4I)dyeColorPrimary_var0;

				Color4I_BGRA dyeColorSecondary_var1;
				Read<Color4I_BGRA>("dyeColorSecondary", handler, dyeColorSecondary_var1, "\t\t\t");

				blockOutput.DyeColor.Secondary = (Color4I)dyeColorSecondary_var1;

				Color4I_BGRA dyeColorTertiary_var2;
				Read<Color4I_BGRA>("dyeColorTertiary", handler, dyeColorTertiary_var2, "\t\t\t");

				blockOutput.DyeColor.Tertiary = (Color4I)dyeColorTertiary_var2;

				Read<int>("dyeColorIndex", handler, blockOutput.DyeColor.Index, "\t\t\t");

				int unknown_var4 = 0;
				Read<int>("unknown", handler, unknown_var4, "\t\t\t");
			}

			short itemCategory_var5 = 0;

			if (!handler.PacketStream().HasRecentlyFailed)
			{
				itemCategory_var5 = handler.GetItemCategory(itemId_param0);
			}

			if (itemCategory_var5 == 113)
			{

				{
					Read<Vector3S>("position1", handler, blockOutput.Cap.Position1, "\t\t\t\t");
					Read<Vector3S>("position2", handler, blockOutput.Cap.Position2, "\t\t\t\t");
					Read<Vector3S>("position3", handler, blockOutput.Cap.Position3, "\t\t\t\t");
					Read<Vector3S>("position4", handler, blockOutput.Cap.Position4, "\t\t\t\t");

					float unknown_var10 = 0;
					Read<float>("unknown", handler, unknown_var10, "\t\t\t\t");
				}
			}

			if (itemCategory_var5 == 102)
			{

				{
					Read<float>("backLength", handler, blockOutput.Hair.BackLength, "\t\t\t\t");
					Read<Vector3S>("backPosition1", handler, blockOutput.Hair.BackPosition1, "\t\t\t\t");
					Read<Vector3S>("backPosition2", handler, blockOutput.Hair.BackPosition2, "\t\t\t\t");
					Read<float>("frontLength", handler, blockOutput.Hair.FrontLength, "\t\t\t\t");
					Read<Vector3S>("frontPosition1", handler, blockOutput.Hair.FrontPosition1, "\t\t\t\t");
					Read<Vector3S>("frontPosition2", handler, blockOutput.Hair.FrontPosition2, "\t\t\t\t");
				}
			}

			if (itemCategory_var5 == 104)
			{

				{
					float position1_var17 = 0;
					Read<float>("position1", handler, position1_var17, "\t\t\t\t");

					float position2_var18 = 0;
					Read<float>("position2", handler, position2_var18, "\t\t\t\t");

					float position3_var19 = 0;
					Read<float>("position3", handler, position3_var19, "\t\t\t\t");

					float position4_var20 = 0;
					Read<float>("position4", handler, position4_var20, "\t\t\t\t");
				}
			}
		}

		void ParseItemEnchantmentHeader_v12(PacketHandler& handler, Maple::Game::ItemEnchantment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("enchantLevel", handler, blockOutput.Level, "\t\t");
			Read<int>("enchantExp", handler, blockOutput.EnchantExp, "\t\t");
			Read<unsigned char>("enchantCharges", handler, blockOutput.EnchantCharges, "\t\t");

			long long unknown_var3 = 0;
			Read<long long>("unknown", handler, unknown_var3, "\t\t");

			int unknown_var4 = 0;
			Read<int>("unknown", handler, unknown_var4, "\t\t");

			int unknown_var5 = 0;
			Read<int>("unknown", handler, unknown_var5, "\t\t");
			Read<bool>("canRepackage", handler, blockOutput.CanRepackage, "\t\t");

			ValidateValues(stream, "canRepackage", "\t\t", blockOutput.CanRepackage, (bool)0, (bool)1);

			Read<int>("charges", handler, blockOutput.Charges, "\t\t");
		}

		void ParseItemEnchantment_v12(PacketHandler& handler, Maple::Game::ItemEnchantment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			ParseItemEnchantmentHeader_v12(handler, blockOutput);

			unsigned char basicStatCount_var0 = 0;
			Read<unsigned char>("basicStatCount", handler, basicStatCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Basic, basicStatCount_var0);

			for (unsigned char i = 0; i < basicStatCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<int>("type", handler, blockOutput.Basic[i].Type, "\t\t\t");
				Read<int>("value", handler, blockOutput.Basic[i].Value, "\t\t\t");
				Read<float>("rate", handler, blockOutput.Basic[i].Rate, "\t\t\t");
			}
		}

		void ParseItemTemplate_v12(PacketHandler& handler, Maple::Game::ItemTemplate& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			long long unknown_var0 = 0;
			Read<long long>("unknown", handler, unknown_var0, "\t\t");
			Read<std::wstring>("fileName", handler, blockOutput.FileName, "\t\t");
			Read<std::wstring>("name", handler, blockOutput.Name, "\t\t");

			unsigned char unknown_var3 = 0;
			Read<unsigned char>("unknown", handler, unknown_var3, "\t\t");

			int unknown_var4 = 0;
			Read<int>("unknown", handler, unknown_var4, "\t\t");
			Read<long long>("accountId", handler, blockOutput.AccountId, "\t\t");
			Read<long long>("characterId", handler, blockOutput.CharacterId, "\t\t");
			Read<std::wstring>("author", handler, blockOutput.Author, "\t\t");
			Read<long long>("creationTime", handler, blockOutput.CreationTime, "\t\t");
			Read<std::wstring>("url", handler, blockOutput.Url, "\t\t");

			unsigned char unknown_var10 = 0;
			Read<unsigned char>("unknown", handler, unknown_var10, "\t\t");
		}

		void ParseItemBlueprint_v12(PacketHandler& handler, Maple::Game::ItemBlueprint& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			long long unknown_var0 = 0;
			Read<long long>("unknown", handler, unknown_var0, "\t\t");

			int unknown_var1 = 0;
			Read<int>("unknown", handler, unknown_var1, "\t\t");

			int unknown_var2 = 0;
			Read<int>("unknown", handler, unknown_var2, "\t\t");

			int unknown_var3 = 0;
			Read<int>("unknown", handler, unknown_var3, "\t\t");

			long long unknown_var4 = 0;
			Read<long long>("unknown", handler, unknown_var4, "\t\t");

			int unknown_var5 = 0;
			Read<int>("unknown", handler, unknown_var5, "\t\t");

			long long unknown_var6 = 0;
			Read<long long>("unknown", handler, unknown_var6, "\t\t");

			long long unknown_var7 = 0;
			Read<long long>("unknown", handler, unknown_var7, "\t\t");

			std::wstring unknown_var8;
			Read<std::wstring>("unknown", handler, unknown_var8, "\t\t");
		}

		void ParseItemPet_v12(PacketHandler& handler, Maple::Game::ItemPet& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<std::wstring>("name", handler, blockOutput.Name, "\t\t");
			Read<long long>("exp", handler, blockOutput.Exp, "\t\t");
			Read<int>("evolvePoints", handler, blockOutput.EvolvePoints, "\t\t");
			Read<int>("level", handler, blockOutput.Level, "\t\t");
			Read<bool>("hasItems", handler, blockOutput.HasItems, "\t\t");

			ValidateValues(stream, "hasItems", "\t\t", blockOutput.HasItems, (bool)0, (bool)1);
		}

		void ParseItemMusic_v12(PacketHandler& handler, Maple::Game::ItemMusic& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("length", handler, blockOutput.Length, "\t\t");
			Read<int>("instrument", handler, blockOutput.Instrument, "\t\t");
			Read<std::wstring>("title", handler, blockOutput.Title, "\t\t");
			Read<std::wstring>("author", handler, blockOutput.Author, "\t\t");

			int unknown_var4 = 0;
			Read<int>("unknown", handler, unknown_var4, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var4, (int)1);

			Read<long long>("authorId", handler, blockOutput.AuthorId, "\t\t");
			Read<bool>("isLocked", handler, blockOutput.IsLocked, "\t\t");

			ValidateValues(stream, "isLocked", "\t\t", blockOutput.IsLocked, (bool)0, (bool)1);

			long long unknown_var7 = 0;
			Read<long long>("unknown", handler, unknown_var7, "\t\t");

			long long unknown_var8 = 0;
			Read<long long>("unknown", handler, unknown_var8, "\t\t");
		}

		void ParseItemBadgeTransparency_v12(PacketHandler& handler, Maple::Game::ItemBadge& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<bool>("transparency", handler, blockOutput.Transparency[0], "\t\t");

			ValidateValues(stream, "transparency", "\t\t", blockOutput.Transparency[0], (bool)0, (bool)1);

			Read<bool>("transparency", handler, blockOutput.Transparency[1], "\t\t");

			ValidateValues(stream, "transparency", "\t\t", blockOutput.Transparency[1], (bool)0, (bool)1);

			Read<bool>("transparency", handler, blockOutput.Transparency[2], "\t\t");

			ValidateValues(stream, "transparency", "\t\t", blockOutput.Transparency[2], (bool)0, (bool)1);

			Read<bool>("transparency", handler, blockOutput.Transparency[3], "\t\t");

			ValidateValues(stream, "transparency", "\t\t", blockOutput.Transparency[3], (bool)0, (bool)1);

			Read<bool>("transparency", handler, blockOutput.Transparency[4], "\t\t");

			ValidateValues(stream, "transparency", "\t\t", blockOutput.Transparency[4], (bool)0, (bool)1);

			Read<bool>("transparency", handler, blockOutput.Transparency[5], "\t\t");

			ValidateValues(stream, "transparency", "\t\t", blockOutput.Transparency[5], (bool)0, (bool)1);

			Read<bool>("transparency", handler, blockOutput.Transparency[6], "\t\t");

			ValidateValues(stream, "transparency", "\t\t", blockOutput.Transparency[6], (bool)0, (bool)1);

			Read<bool>("transparency", handler, blockOutput.Transparency[7], "\t\t");

			ValidateValues(stream, "transparency", "\t\t", blockOutput.Transparency[7], (bool)0, (bool)1);

			Read<bool>("transparency", handler, blockOutput.Transparency[8], "\t\t");

			ValidateValues(stream, "transparency", "\t\t", blockOutput.Transparency[8], (bool)0, (bool)1);

			Read<bool>("transparency", handler, blockOutput.Transparency[9], "\t\t");

			ValidateValues(stream, "transparency", "\t\t", blockOutput.Transparency[9], (bool)0, (bool)1);
		}

		void ParseItemBadge_v12(PacketHandler& handler, Maple::Game::ItemBadge& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char unknown_var0 = 0;
			Read<unsigned char>("unknown", handler, unknown_var0, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var0, (unsigned char)1);

			Read<unsigned char>("type", handler, blockOutput.Type, "\t\t");

			ValidateValues(stream, "type", "\t\t", blockOutput.Type, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4, (unsigned char)5, (unsigned char)6, (unsigned char)7, (unsigned char)8, (unsigned char)9, (unsigned char)10, (unsigned char)11, (unsigned char)12);

			Read<std::wstring>("id", handler, blockOutput.Id, "\t\t");

			if (blockOutput.Type == 1)
			{
				ParseItemBadgeTransparency_v12(handler, blockOutput);
			}

			if (blockOutput.Type == 11)
			{
				Read<int>("petSkinId", handler, blockOutput.PetSkinId, "\t\t\t");
			}
		}

		void ParseItemTransfer_v12(PacketHandler& handler, Maple::Game::ItemTransfer& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("transferFlags", handler, blockOutput.TransferFlags, "\t\t");

			bool unknown_var1 = false;
			Read<bool>("unknown", handler, unknown_var1, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var1, (bool)0, (bool)1);

			Read<int>("remainingTrades", handler, blockOutput.RemainingTrades, "\t\t");
			Read<int>("remainingPackages", handler, blockOutput.RemainingPackages, "\t\t");

			unsigned char unknown_var4 = 0;
			Read<unsigned char>("unknown", handler, unknown_var4, "\t\t");

			bool unknown_var5 = false;
			Read<bool>("unknown", handler, unknown_var5, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var5, (bool)0, (bool)1);

			Read<bool>("isBound", handler, blockOutput.IsBound, "\t\t");

			ValidateValues(stream, "isBound", "\t\t", blockOutput.IsBound, (bool)0, (bool)1);

			if (blockOutput.IsBound)
			{
				ParseItemBinding_v12(handler, blockOutput.Binding);
			}
		}

		void ParseItemSocket_v12(PacketHandler& handler, Maple::Game::ItemSocket& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<unsigned char>("maxSlots", handler, blockOutput.MaxSlots, "\t\t");
			Read<unsigned char>("unlockedSlots", handler, blockOutput.UnlockedSlots, "\t\t");

			ResizeVector(handler, blockOutput.Sockets, blockOutput.UnlockedSlots);

			for (unsigned char i = 0; i < blockOutput.UnlockedSlots && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<bool>("hasGem", handler, blockOutput.Sockets[i].HasGem, "\t\t\t");

				ValidateValues(stream, "hasGem", "\t\t\t", blockOutput.Sockets[i].HasGem, (bool)0, (bool)1);

				if (blockOutput.Sockets[i].HasGem)
				{

					{
						Read<int>("itemId", handler, blockOutput.Sockets[i].Gem.ItemId, "\t\t\t\t\t");
						Read<bool>("isBound", handler, blockOutput.Sockets[i].Gem.IsBound, "\t\t\t\t\t");

						ValidateValues(stream, "isBound", "\t\t\t\t\t", blockOutput.Sockets[i].Gem.IsBound, (bool)0, (bool)1);

						if (blockOutput.Sockets[i].Gem.IsBound)
						{
							ParseItemBinding_v12(handler, blockOutput.Sockets[i].Gem.Binding);
						}

						Read<bool>("isLocked", handler, blockOutput.Sockets[i].Gem.IsLocked, "\t\t\t\t\t");

						ValidateValues(stream, "isLocked", "\t\t\t\t\t", blockOutput.Sockets[i].Gem.IsLocked, (bool)0, (bool)1);

						if (blockOutput.Sockets[i].Gem.IsLocked)
						{
							unsigned char unknown_var6 = 0;
							Read<unsigned char>("unknown", handler, unknown_var6, "\t\t\t\t\t\t");
							Read<long long>("unlockTime", handler, blockOutput.Sockets[i].Gem.UnlockTime, "\t\t\t\t\t\t");
						}
					}
				}
			}
		}

		void ParseItemCouple_v12(PacketHandler& handler, Maple::Game::ItemCouple& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<long long>("characterId", handler, blockOutput.CharacterId, "\t\t");

			if (blockOutput.CharacterId != 0)
			{
				Read<std::wstring>("name", handler, blockOutput.Name, "\t\t\t");
				Read<bool>("isCreator", handler, blockOutput.IsCreator, "\t\t\t");

				ValidateValues(stream, "isCreator", "\t\t\t", blockOutput.IsCreator, (bool)0, (bool)1);
			}
		}

		void ParseItemHeader_v12(PacketHandler& handler, Maple::Game::ItemData& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<long long>("creationTime", handler, blockOutput.CreationTime, "\t\t");
			Read<long long>("expirationTime", handler, blockOutput.ExpirationTime, "\t\t");

			long long unknown_var2 = 0;
			Read<long long>("unknown", handler, unknown_var2, "\t\t");
			Read<int>("timesChanged", handler, blockOutput.TimesChanged, "\t\t");
			Read<int>("remainingUses", handler, blockOutput.RemainingUses, "\t\t");
			Read<bool>("isLocked", handler, blockOutput.IsLocked, "\t\t");

			ValidateValues(stream, "isLocked", "\t\t", blockOutput.IsLocked, (bool)0, (bool)1);

			Read<long long>("unlockTime", handler, blockOutput.UnlockTime, "\t\t");
			Read<short>("glamorForges", handler, blockOutput.GlamorForges, "\t\t");

			unsigned char unknownBool_var8 = 0;
			Read<unsigned char>("unknownBool", handler, unknownBool_var8, "\t\t");

			ValidateValues(stream, "unknownBool", "\t\t", unknownBool_var8, (unsigned char)0);
		}

		void ParseItemData_v12(PacketHandler& handler, Maple::Game::ItemData& blockOutput, unsigned int itemId_param0)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("amount", handler, blockOutput.Amount, "\t\t");

			int unknown_var1 = 0;
			Read<int>("unknown", handler, unknown_var1, "\t\t");

			int unknown_var2 = 0;
			Read<int>("unknown", handler, unknown_var2, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var2, (int)18446744073709551615);
			ParseItemHeader_v12(handler, blockOutput);

			int unknown_var3 = 0;
			Read<int>("unknown", handler, unknown_var3, "\t\t");
			ParseItemCustomization_v12(handler, blockOutput.Customization, itemId_param0);

			{

				unsigned char unknown_var4 = 0;
				Read<unsigned char>("unknown", handler, unknown_var4, "\t\t\t");
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Constant);
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Static);
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Random);
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Title);
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Empowerment1);
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Empowerment2);
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Empowerment3);
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Empowerment4);
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Empowerment5);
			}
			ParseItemEnchantment_v12(handler, blockOutput.Enchantment);

			{

				Read<int>("limitBreakLevel", handler, blockOutput.LimitBreak.Level, "\t\t\t");

				{

					int statCount_var6 = 0;
					Read<int>("statCount", handler, statCount_var6, "\t\t\t\t");

					ResizeVector(handler, blockOutput.LimitBreak.Stats.Basic, statCount_var6);

					for (int i = 0; i < statCount_var6 && !handler.PacketStream().HasRecentlyFailed; ++i)
					{
						ParseBasicStat_v12(handler, blockOutput.LimitBreak.Stats.Basic[i]);
					}

					Read<int>("statCount", handler, statCount_var6, "\t\t\t\t");

					ResizeVector(handler, blockOutput.LimitBreak.Stats.Special, statCount_var6);

					for (int i = 0; i < statCount_var6 && !handler.PacketStream().HasRecentlyFailed; ++i)
					{
						ParseSpecialStat_v12(handler, blockOutput.LimitBreak.Stats.Special[i]);
					}
				}
			}

			unsigned char extraDataType_var7 = 0;

			if (!handler.PacketStream().HasRecentlyFailed)
			{
				extraDataType_var7 = handler.GetItemExtraDataType(itemId_param0);
			}

			ValidateValues(stream, "extraDataType", "\t\t", extraDataType_var7, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4);

			if (extraDataType_var7 == 1)
			{
				ParseItemTemplate_v12(handler, blockOutput.Template);
				ParseItemBlueprint_v12(handler, blockOutput.Blueprint);
			}

			if (extraDataType_var7 == 2)
			{
				ParseItemPet_v12(handler, blockOutput.Pet);
			}

			if (extraDataType_var7 == 3)
			{
				ParseItemMusic_v12(handler, blockOutput.Music);
			}

			if (extraDataType_var7 == 4)
			{
				ParseItemBadge_v12(handler, blockOutput.Badge);
			}
			ParseItemTransfer_v12(handler, blockOutput.ItemTransfer);
			ParseItemSocket_v12(handler, blockOutput.ItemSocket);
			ParseItemCouple_v12(handler, blockOutput.ItemCouple);
			ParseItemBinding_v12(handler, blockOutput.Binding);
		}

		void ParseEffectStats_v12(PacketHandler& handler, Maple::Game::EffectStats& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("startTime", handler, blockOutput.StartTime, "\t\t");
			Read<int>("endTime", handler, blockOutput.EndTime, "\t\t");
			Read<int>("effectId", handler, blockOutput.EffectId, "\t\t");
			Read<short>("effectLevel", handler, blockOutput.EffectLevel, "\t\t");
			Read<int>("stacks", handler, blockOutput.Stacks, "\t\t");
		}

		void ParseCharacterData_v12(PacketHandler& handler, Maple::Game::CharacterData& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<long long>("accountId", handler, blockOutput.AccountId, "\t\t");
			Read<long long>("characterId", handler, blockOutput.CharacterId, "\t\t");
			Read<std::wstring>("name", handler, blockOutput.Name, "\t\t");
			Read<unsigned char>("gender", handler, blockOutput.Gender, "\t\t");

			ValidateValues(stream, "gender", "\t\t", blockOutput.Gender, (unsigned char)0, (unsigned char)1);

			unsigned char unknown_var4 = 0;
			Read<unsigned char>("unknown", handler, unknown_var4, "\t\t");

			long long unknown_var5 = 0;
			Read<long long>("unknown", handler, unknown_var5, "\t\t");

			int unknown_var6 = 0;
			Read<int>("unknown", handler, unknown_var6, "\t\t");
			Read<int>("mapId", handler, blockOutput.MapId, "\t\t");
			Read<int>("mapInstanceId", handler, blockOutput.MapInstanceId, "\t\t");

			int unknown_var9 = 0;
			Read<int>("unknown", handler, unknown_var9, "\t\t");
			Read<unsigned short>("level", handler, blockOutput.Level, "\t\t");
			Read<unsigned short>("channel", handler, blockOutput.Channel, "\t\t");
			Read<int>("jobCode", handler, blockOutput.JobCode, "\t\t");
			Read<int>("job", handler, blockOutput.Job, "\t\t");
			Read<int>("currentHp", handler, blockOutput.Hp.Current, "\t\t");
			Read<int>("maxHp", handler, blockOutput.Hp.Max, "\t\t");

			short unknown_var16 = 0;
			Read<short>("unknown", handler, unknown_var16, "\t\t");

			long long unknown_var17 = 0;
			Read<long long>("unknown", handler, unknown_var17, "\t\t");

			long long storageCooldown_var18 = 0;
			Read<long long>("storageCooldown", handler, storageCooldown_var18, "\t\t");

			blockOutput.StorageCooldown = (long)storageCooldown_var18;

			long long doctorCooldown_var19 = 0;
			Read<long long>("doctorCooldown", handler, doctorCooldown_var19, "\t\t");

			blockOutput.DoctorCooldown = (long)doctorCooldown_var19;

			Read<int>("returnMapId", handler, blockOutput.ReturnMapId, "\t\t");
			Read<Vector3S>("returnPosition", handler, blockOutput.ReturnPosition, "\t\t");
			Read<int>("gearScore", handler, blockOutput.GearScore, "\t\t");

			Color4I_BGRA skinColorPrimary_var23;
			Read<Color4I_BGRA>("skinColorPrimary", handler, skinColorPrimary_var23, "\t\t");

			blockOutput.SkinColor.Primary = (Color4I)skinColorPrimary_var23;

			Color4I_BGRA skinColorSecondary_var24;
			Read<Color4I_BGRA>("skinColorSecondary", handler, skinColorSecondary_var24, "\t\t");

			blockOutput.SkinColor.Secondary = (Color4I)skinColorSecondary_var24;

			long long creationTime_var25 = 0;
			Read<long long>("creationTime", handler, creationTime_var25, "\t\t");

			blockOutput.CreationTime = (long)creationTime_var25;

			Read<int>("combatTrophyCount", handler, blockOutput.CombatTrophyCount, "\t\t");
			Read<int>("adventureTrophyCount", handler, blockOutput.AdventureTrophyCount, "\t\t");
			Read<int>("lifestyleTrophyCount", handler, blockOutput.LifestyleTrophyCount, "\t\t");
			Read<long long>("guildId", handler, blockOutput.GuildId, "\t\t");
			Read<std::wstring>("guildName", handler, blockOutput.GuildName, "\t\t");
			Read<std::wstring>("guildMotto", handler, blockOutput.GuildMotto, "\t\t");
			Read<std::wstring>("profileUrl", handler, blockOutput.ProfileUrl, "\t\t");

			unsigned char clubCount_var33 = 0;
			Read<unsigned char>("clubCount", handler, clubCount_var33, "\t\t");

			ResizeVector(handler, blockOutput.Clubs, clubCount_var33);

			for (unsigned char i = 0; i < clubCount_var33 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<bool>("hasClub", handler, blockOutput.Clubs[i].HasClub, "\t\t\t");

				ValidateValues(stream, "hasClub", "\t\t\t", blockOutput.Clubs[i].HasClub, (bool)0, (bool)1);

				if (blockOutput.Clubs[i].HasClub)
				{
					Read<long long>("clubId", handler, blockOutput.Clubs[i].ClubId, "\t\t\t\t");
					Read<std::wstring>("clubName", handler, blockOutput.Clubs[i].ClubName, "\t\t\t\t");
				}
			}

			unsigned char unknown_var37 = 0;
			Read<unsigned char>("unknown", handler, unknown_var37, "\t\t");

			{

				int placeholderMastery_var38 = 0;
				Read<int>("placeholderMastery", handler, placeholderMastery_var38, "\t\t\t");

				ValidateValues(stream, "placeholderMastery", "\t\t\t", placeholderMastery_var38, (int)0);

				Read<int>("fishingMastery", handler, blockOutput.Mastery.Fishing, "\t\t\t");
				Read<int>("performanceMastery", handler, blockOutput.Mastery.Performance, "\t\t\t");
				Read<int>("miningMastery", handler, blockOutput.Mastery.Mining, "\t\t\t");
				Read<int>("foragingMastery", handler, blockOutput.Mastery.Foraging, "\t\t\t");
				Read<int>("ranchingMastery", handler, blockOutput.Mastery.Ranching, "\t\t\t");
				Read<int>("farmingMastery", handler, blockOutput.Mastery.Farming, "\t\t\t");
				Read<int>("smithingMastery", handler, blockOutput.Mastery.Smithing, "\t\t\t");
				Read<int>("handicraftMastery", handler, blockOutput.Mastery.Handicraft, "\t\t\t");
				Read<int>("alchemyMastery", handler, blockOutput.Mastery.Alchemy, "\t\t\t");
				Read<int>("cookingMastery", handler, blockOutput.Mastery.Cooking, "\t\t\t");
				Read<int>("petTamingMastery", handler, blockOutput.Mastery.PetTaming, "\t\t\t");
			}
			Read<std::wstring>("username", handler, blockOutput.Username, "\t\t");
			Read<long long>("sessionId", handler, blockOutput.SessionId, "\t\t");

			long long unknown_var52 = 0;
			Read<long long>("unknown", handler, unknown_var52, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var52, (long long)0, (long long)2000);

			long long unknown_var53 = 0;
			Read<long long>("unknown", handler, unknown_var53, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var53, (long long)0, (long long)3000);

			int unknownCount_var54 = 0;
			Read<int>("unknownCount", handler, unknownCount_var54, "\t\t");

			ResizeVector(handler, blockOutput.UnknownItems, unknownCount_var54);

			for (int i = 0; i < unknownCount_var54 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				long long unknown_var55 = 0;
				Read<long long>("unknown", handler, unknown_var55, "\t\t\t");
			}

			unsigned char unknown_var56 = 0;
			Read<unsigned char>("unknown", handler, unknown_var56, "\t\t");

			bool unknown_var57 = false;
			Read<bool>("unknown", handler, unknown_var57, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var57, (bool)0, (bool)1);

			long long birthday_var58 = 0;
			Read<long long>("birthday", handler, birthday_var58, "\t\t");

			blockOutput.Birthday = (long)birthday_var58;

			int unknown_var59 = 0;
			Read<int>("unknown", handler, unknown_var59, "\t\t");

			int unknown_var60 = 0;
			Read<int>("unknown", handler, unknown_var60, "\t\t");

			long long unknownTime_var61 = 0;
			Read<long long>("unknownTime", handler, unknownTime_var61, "\t\t");
			Read<int>("prestigeLevel", handler, blockOutput.PrestigeLevel, "\t\t");

			long long unknownTime_var63 = 0;
			Read<long long>("unknownTime", handler, unknownTime_var63, "\t\t");

			int unknownCount2_var64 = 0;
			Read<int>("unknownCount2", handler, unknownCount2_var64, "\t\t");

			ResizeVector(handler, blockOutput.UnknownItems2, unknownCount2_var64);

			for (int i = 0; i < unknownCount2_var64 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				long long unknown_var65 = 0;
				Read<long long>("unknown", handler, unknown_var65, "\t\t\t");
			}

			int unknownCount3_var66 = 0;
			Read<int>("unknownCount3", handler, unknownCount3_var66, "\t\t");

			ResizeVector(handler, blockOutput.UnknownItems3, unknownCount3_var66);

			for (int i = 0; i < unknownCount3_var66 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				long long unknown_var67 = 0;
				Read<long long>("unknown", handler, unknown_var67, "\t\t\t");
			}

			short unknown_var68 = 0;
			Read<short>("unknown", handler, unknown_var68, "\t\t");

			long long unknown_var69 = 0;
			Read<long long>("unknown", handler, unknown_var69, "\t\t");
		}

		void ParseSkillDamagePacket_v12(PacketHandler& handler, Server::SkillDamagePacket& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<long long>("skillSn", handler, blockOutput.SkillSn, "\t\t");
			Read<long long>("skillAttack", handler, blockOutput.SkillAttack, "\t\t");
			Read<int>("casterId", handler, blockOutput.CasterId, "\t\t");
			Read<unsigned int>("skillId", handler, blockOutput.SkillId, "\t\t");
			Read<unsigned short>("skillLevel", handler, blockOutput.SkillLevel, "\t\t");
			Read<unsigned char>("motionPoint", handler, blockOutput.MotionPoint, "\t\t");
			Read<unsigned char>("attackPoint", handler, blockOutput.AttackPoint, "\t\t");
			Read<Vector3Short>("position", handler, blockOutput.Position, "\t\t");
			Read<Vector3Short>("direction", handler, blockOutput.Direction, "\t\t");

			unsigned char count_var9 = 0;
			Read<unsigned char>("count", handler, count_var9, "\t\t");

			ResizeVector(handler, blockOutput.DamagedTargets, count_var9);

			for (unsigned char i = 0; i < count_var9 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<int>("targetId", handler, blockOutput.DamagedTargets[i].TargetId, "\t\t\t");

				unsigned char damageCount_var11 = 0;
				Read<unsigned char>("damageCount", handler, damageCount_var11, "\t\t\t");

				ResizeVector(handler, blockOutput.DamagedTargets[i].Damages, damageCount_var11);

				for (unsigned char j = 0; j < damageCount_var11 && !handler.PacketStream().HasRecentlyFailed; ++j)
				{
					Read<unsigned char>("hitType", handler, blockOutput.DamagedTargets[i].Damages[j].HitType, "\t\t\t\t");

					ValidateValues(stream, "hitType", "\t\t\t\t", blockOutput.DamagedTargets[i].Damages[j].HitType, (unsigned char)0, (unsigned char)8, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4);

					Read<long long>("damage", handler, blockOutput.DamagedTargets[i].Damages[j].Damage, "\t\t\t\t");
				}
			}
		}

		void ParseSkillDamageDotPacket_v12(PacketHandler& handler, Server::SkillDamageDotPacket& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("casterId", handler, blockOutput.CasterId, "\t\t");
			Read<int>("targetId", handler, blockOutput.TargetId, "\t\t");
			Read<int>("serverTick", handler, blockOutput.ServerTick, "\t\t");
			Read<unsigned char>("hitType", handler, blockOutput.HitType, "\t\t");

			ValidateValues(stream, "hitType", "\t\t", blockOutput.HitType, (unsigned char)8, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4);
		}

		void ParseSkillDamageHealPacket_v12(PacketHandler& handler, Server::SkillDamageHealPacket& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("casterId", handler, blockOutput.CasterId, "\t\t");
			Read<int>("targetId", handler, blockOutput.TargetId, "\t\t");
			Read<int>("serverTick", handler, blockOutput.ServerTick, "\t\t");
			Read<int>("hpAmount", handler, blockOutput.HpAmount, "\t\t");
			Read<int>("spAmount", handler, blockOutput.SpAmount, "\t\t");
			Read<int>("epAmount", handler, blockOutput.EpAmount, "\t\t");
			Read<bool>("isVisible", handler, blockOutput.IsVisible, "\t\t");

			ValidateValues(stream, "isVisible", "\t\t", blockOutput.IsVisible, (bool)0, (bool)1);
		}

		void ParseSkillDamageRegionPacket_v12(PacketHandler& handler, Server::SkillDamageRegionPacket& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<long long>("skillSn", handler, blockOutput.SkillSn, "\t\t");
			Read<int>("casterId", handler, blockOutput.CasterId, "\t\t");
			Read<int>("regionSkillObjectId", handler, blockOutput.RegionSkillObjectId, "\t\t");
			Read<unsigned char>("attackPoint", handler, blockOutput.AttackPoint, "\t\t");

			unsigned char count_var4 = 0;
			Read<unsigned char>("count", handler, count_var4, "\t\t");

			ResizeVector(handler, blockOutput.DamagedTargets, count_var4);

			for (unsigned char i = 0; i < count_var4 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<int>("targetId", handler, blockOutput.DamagedTargets[i].TargetId, "\t\t\t");

				unsigned char damageCount_var6 = 0;
				Read<unsigned char>("damageCount", handler, damageCount_var6, "\t\t\t");
				Read<Vector3Short>("position", handler, blockOutput.DamagedTargets[i].Position, "\t\t\t");
				Read<Vector3S>("direction", handler, blockOutput.DamagedTargets[i].Direction, "\t\t\t");

				ResizeVector(handler, blockOutput.DamagedTargets[i].Damages, damageCount_var6);

				for (unsigned char j = 0; j < damageCount_var6 && !handler.PacketStream().HasRecentlyFailed; ++j)
				{
					Read<unsigned char>("hitType", handler, blockOutput.DamagedTargets[i].Damages[j].HitType, "\t\t\t\t");

					ValidateValues(stream, "hitType", "\t\t\t\t", blockOutput.DamagedTargets[i].Damages[j].HitType, (unsigned char)8, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4);

					Read<long long>("damage", handler, blockOutput.DamagedTargets[i].Damages[j].Damage, "\t\t\t\t");
				}
			}
		}

		void ParseSkillDamageTilePacket_v12(PacketHandler& handler, Server::SkillDamageTilePacket& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<long long>("skillSn", handler, blockOutput.SkillSn, "\t\t");
			Read<unsigned int>("skillId", handler, blockOutput.SkillId, "\t\t");
			Read<unsigned short>("skillLevel", handler, blockOutput.SkillLevel, "\t\t");

			unsigned char count_var3 = 0;
			Read<unsigned char>("count", handler, count_var3, "\t\t");

			ResizeVector(handler, blockOutput.DamagedTargets, count_var3);

			for (unsigned char i = 0; i < count_var3 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<int>("targetId", handler, blockOutput.DamagedTargets[i].TargetId, "\t\t\t");

				unsigned char damageCount_var5 = 0;
				Read<unsigned char>("damageCount", handler, damageCount_var5, "\t\t\t");
				Read<Vector3Short>("position", handler, blockOutput.DamagedTargets[i].Position, "\t\t\t");
				Read<Vector3S>("direction", handler, blockOutput.DamagedTargets[i].Direction, "\t\t\t");

				ResizeVector(handler, blockOutput.DamagedTargets[i].Damages, damageCount_var5);

				for (unsigned char j = 0; j < damageCount_var5 && !handler.PacketStream().HasRecentlyFailed; ++j)
				{
					Read<unsigned char>("hitType", handler, blockOutput.DamagedTargets[i].Damages[j].HitType, "\t\t\t\t");

					ValidateValues(stream, "hitType", "\t\t\t\t", blockOutput.DamagedTargets[i].Damages[j].HitType, (unsigned char)0, (unsigned char)8, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4);

					Read<long long>("damage", handler, blockOutput.DamagedTargets[i].Damages[j].Damage, "\t\t\t\t");
				}
			}
		}

		void ParseEquipmentData_v12(PacketHandler& handler, Maple::Game::EquipmentData& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("itemId", handler, blockOutput.ItemId, "\t\t");
			Read<long long>("itemInstanceId", handler, blockOutput.ItemInstanceId, "\t\t");
			Read<std::wstring>("slot", handler, blockOutput.SlotName, "\t\t");
			Read<int>("rarity", handler, blockOutput.Rarity, "\t\t");

			ValidateValues(stream, "rarity", "\t\t", blockOutput.Rarity, (int)0, (int)1, (int)2, (int)3, (int)4, (int)5, (int)6);
			ParseItemData_v12(handler, blockOutput.Data, (unsigned int)blockOutput.ItemId);
		}

		void ParseCharacterEquipmentItems_v12(PacketHandler& handler, Maple::Game::CharacterEquipment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char equipmentCount_var0 = 0;
			Read<unsigned char>("equipmentCount", handler, equipmentCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Equipment, equipmentCount_var0);

			for (unsigned char i = 0; i < equipmentCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				ParseEquipmentData_v12(handler, blockOutput.Equipment[i]);
			}
		}

		void ParseCharacterEquipmentBadges_v12(PacketHandler& handler, Maple::Game::CharacterEquipment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char badgeCount_var0 = 0;
			Read<unsigned char>("badgeCount", handler, badgeCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Badges, badgeCount_var0);

			for (unsigned char i = 0; i < badgeCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<unsigned char>("slot", handler, blockOutput.Badges[i].Slot, "\t\t\t");
				Read<int>("itemId", handler, blockOutput.Badges[i].ItemId, "\t\t\t");
				Read<long long>("itemInstanceId", handler, blockOutput.Badges[i].ItemInstanceId, "\t\t\t");
				Read<int>("rarity", handler, blockOutput.Badges[i].Rarity, "\t\t\t");

				ValidateValues(stream, "rarity", "\t\t\t", blockOutput.Badges[i].Rarity, (int)0, (int)1, (int)2, (int)3, (int)4, (int)5, (int)6);
				ParseItemData_v12(handler, blockOutput.Badges[i].Data, (unsigned int)blockOutput.Badges[i].ItemId);
			}
		}

		void ParseCharacterEquipmentSkins_v12(PacketHandler& handler, Maple::Game::CharacterEquipment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<bool>("hasSkin", handler, blockOutput.HasSkin, "\t\t");

			ValidateValues(stream, "hasSkin", "\t\t", blockOutput.HasSkin, (bool)0, (bool)1);

			if (blockOutput.HasSkin)
			{
				long long unknown_var1 = 0;
				Read<long long>("unknown", handler, unknown_var1, "\t\t\t");

				long long unknown_var2 = 0;
				Read<long long>("unknown", handler, unknown_var2, "\t\t\t");

				unsigned char skinCount_var3 = 0;
				Read<unsigned char>("skinCount", handler, skinCount_var3, "\t\t\t");

				ResizeVector(handler, blockOutput.Skins, skinCount_var3);

				for (unsigned char i = 0; i < skinCount_var3 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					ParseEquipmentData_v12(handler, blockOutput.Skins[i]);
				}
			}
		}

		void ParseCharacterListEntry_v12(PacketHandler& handler, Maple::Game::CharacterListEntry& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			ParseCharacterData_v12(handler, blockOutput.Character);

			Read<std::wstring>("profileUrl", handler, blockOutput.ProfileUrl, "\t\t");
			Read<long long>("deleteTime", handler, blockOutput.DeleteTime, "\t\t");
			ParseCharacterEquipmentItems_v12(handler, blockOutput.CharacterEquipment);
			ParseCharacterEquipmentBadges_v12(handler, blockOutput.CharacterEquipment);
			ParseCharacterEquipmentSkins_v12(handler, blockOutput.CharacterEquipment);
		}

		void ParseSkillTreePageData_v12(PacketHandler& handler, Maple::Game::SkillTreePageData& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char count_var0 = 0;
			Read<unsigned char>("count", handler, count_var0, "\t\t");

			ResizeVector(handler, blockOutput.Skills, count_var0);

			for (unsigned char i = 0; i < count_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<bool>("notify", handler, blockOutput.Skills[i].Notify, "\t\t\t");

				ValidateValues(stream, "notify", "\t\t\t", blockOutput.Skills[i].Notify, (bool)0, (bool)1);

				Read<bool>("enabled", handler, blockOutput.Skills[i].Enabled, "\t\t\t");

				ValidateValues(stream, "enabled", "\t\t\t", blockOutput.Skills[i].Enabled, (bool)0, (bool)1);

				Read<int>("skillId", handler, blockOutput.Skills[i].SkillId, "\t\t\t");
				Read<int>("skillLevel", handler, blockOutput.Skills[i].SkillLevel, "\t\t\t");

				bool unknown_var5 = false;
				Read<bool>("unknown", handler, unknown_var5, "\t\t\t");

				ValidateValues(stream, "unknown", "\t\t\t", unknown_var5, (bool)0, (bool)1);
			}
		}

		void ParseSkillTreeData_v12(PacketHandler& handler, Maple::Game::SkillTreeData& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("job", handler, blockOutput.Job, "\t\t");

			unsigned char unknownCount_var1 = 0;
			Read<unsigned char>("unknownCount", handler, unknownCount_var1, "\t\t");

			ValidateValues(stream, "unknownCount", "\t\t", unknownCount_var1, (unsigned char)1);

			Read<int>("jobCode", handler, blockOutput.JobCode, "\t\t");
			ParseSkillTreePageData_v12(handler, blockOutput.Active);
			ParseSkillTreePageData_v12(handler, blockOutput.Passive);
			ParseSkillTreePageData_v12(handler, blockOutput.Special);
			ParseSkillTreePageData_v12(handler, blockOutput.Consumable);
		}

		void ParseCharacterDetailsHeader_v12(PacketHandler& handler, Server::CharacterInfoPacket::CharacterDetails& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<long long>("accountId", handler, blockOutput.AccountId, "\t\t");
			Read<long long>("characterId", handler, blockOutput.CharacterId, "\t\t");
			Read<std::wstring>("characterName", handler, blockOutput.Name, "\t\t");
			Read<short>("level", handler, blockOutput.Level, "\t\t");
			Read<int>("jobCode", handler, blockOutput.JobCode, "\t\t");
			Read<int>("job", handler, blockOutput.Job, "\t\t");
			Read<int>("gender", handler, blockOutput.Gender, "\t\t");

			ValidateValues(stream, "gender", "\t\t", blockOutput.Gender, (int)0, (int)1);

			Read<int>("prestigeLevel", handler, blockOutput.PrestigeLevel, "\t\t");

			unsigned char unknown_var8 = 0;
			Read<unsigned char>("unknown", handler, unknown_var8, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var8, (unsigned char)0);
		}

		void ParseCharacterDetailsBody_v12(PacketHandler& handler, Server::CharacterInfoPacket::CharacterDetails& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<std::wstring>("profileUrl", handler, blockOutput.ProfileUrl, "\t\t");
			Read<std::wstring>("motto", handler, blockOutput.Motto, "\t\t");
			Read<std::wstring>("guildName", handler, blockOutput.GuildName, "\t\t");
			Read<std::wstring>("guildRank", handler, blockOutput.GuildRank, "\t\t");
			Read<std::wstring>("houseName", handler, blockOutput.HouseName, "\t\t");
			Read<int>("plotMapId", handler, blockOutput.PlotMapId, "\t\t");
			Read<int>("plotNumber", handler, blockOutput.PlotNumber, "\t\t");
			Read<int>("apartmentNumber", handler, blockOutput.ApartmentNumber, "\t\t");
			Read<int>("characterTitle", handler, blockOutput.CharacterTitle, "\t\t");

			int unlockedTitles_var9 = 0;
			Read<int>("unlockedTitles", handler, unlockedTitles_var9, "\t\t");

			ResizeVector(handler, blockOutput.UnlockedTitles, unlockedTitles_var9);

			for (int i = 0; i < unlockedTitles_var9 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<int>("titleId", handler, blockOutput.UnlockedTitles[i].TitleId, "\t\t\t");
			}
			Read<int>("trophyCount", handler, blockOutput.TrophyCount, "\t\t");
			Read<int>("gearScore", handler, blockOutput.GearScore, "\t\t");
			Read<long long>("lastLoggedIn", handler, blockOutput.LastLoggedIn, "\t\t");

			long long unknown_var14 = 0;
			Read<long long>("unknown", handler, unknown_var14, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var14, (long long)0, (long long)1);

			Color4I_BGRA skinColorPrimary_var15;
			Read<Color4I_BGRA>("skinColorPrimary", handler, skinColorPrimary_var15, "\t\t");

			blockOutput.SkinColor.Primary = (Color4I)skinColorPrimary_var15;

			Color4I_BGRA skinColorSecondary_var16;
			Read<Color4I_BGRA>("skinColorSecondary", handler, skinColorSecondary_var16, "\t\t");

			blockOutput.SkinColor.Secondary = (Color4I)skinColorSecondary_var16;

			Read<short>("maritalStatus", handler, blockOutput.MaritalStatus, "\t\t");
			Read<std::wstring>("spouse1Name", handler, blockOutput.Spouse1Name, "\t\t");
			Read<std::wstring>("spouse2Name", handler, blockOutput.Spouse2Name, "\t\t");
			Read<long long>("proposalTimestamp", handler, blockOutput.ProposalTimestamp, "\t\t");
		}

		void ParseCharacterDetailsBasicStats_v12(PacketHandler& handler, Server::CharacterInfoPacket::CharacterStats& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char basicStatCount_var0 = 35;

			ResizeVector(handler, blockOutput.Basic, basicStatCount_var0);

			for (unsigned char i = 0; i < basicStatCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<long long>("max", handler, blockOutput.Basic[i].Max, "\t\t\t");
			}

			ResizeVector(handler, blockOutput.Basic, basicStatCount_var0);

			for (unsigned char i = 0; i < basicStatCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<long long>("base", handler, blockOutput.Basic[i].Base, "\t\t\t");
			}

			ResizeVector(handler, blockOutput.Basic, basicStatCount_var0);

			for (unsigned char i = 0; i < basicStatCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<long long>("current", handler, blockOutput.Basic[i].Current, "\t\t\t");
			}

			ResizeVector(handler, blockOutput.Basic, basicStatCount_var0);

			for (unsigned char i = 0; i < basicStatCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<float>("rate", handler, blockOutput.Basic[i].Rate, "\t\t\t");
			}
		}

		void ParseCharacterDetailsSpecialStats_v12(PacketHandler& handler, Server::CharacterInfoPacket::CharacterStats& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char specialStatCount_var0 = 180;

			ResizeVector(handler, blockOutput.Special, specialStatCount_var0);

			for (unsigned char i = 0; i < specialStatCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<float>("rate", handler, blockOutput.Special[i].Rate, "\t\t\t");
			}

			ResizeVector(handler, blockOutput.Special, specialStatCount_var0);

			for (unsigned char i = 0; i < specialStatCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<float>("value", handler, blockOutput.Special[i].Value, "\t\t\t");
			}
		}

		void ParseItemBadge_v2486(PacketHandler& handler, Maple::Game::ItemBadge& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char unknown_var0 = 0;
			Read<unsigned char>("unknown", handler, unknown_var0, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var0, (unsigned char)1);

			Read<unsigned char>("type", handler, blockOutput.Type, "\t\t");

			ValidateValues(stream, "type", "\t\t", blockOutput.Type, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4, (unsigned char)5, (unsigned char)6, (unsigned char)7, (unsigned char)8, (unsigned char)9, (unsigned char)10, (unsigned char)11, (unsigned char)12);

			Read<int>("id", handler, blockOutput.BadgeId, "\t\t");

			if (blockOutput.Type == 1)
			{
				ParseItemBadgeTransparency_v12(handler, blockOutput);
			}

			if (blockOutput.Type == 11)
			{
				Read<int>("petSkinId", handler, blockOutput.PetSkinId, "\t\t\t");
			}
		}

		void ParseItemData_v2486(PacketHandler& handler, Maple::Game::ItemData& blockOutput, unsigned int itemId_param0)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("amount", handler, blockOutput.Amount, "\t\t");

			int unknown_var1 = 0;
			Read<int>("unknown", handler, unknown_var1, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var1, (int)18446744073709551615);

			int unknown_var2 = 0;
			Read<int>("unknown", handler, unknown_var2, "\t\t");
			ParseItemHeader_v12(handler, blockOutput);
			ParseItemCustomization_v12(handler, blockOutput.Customization, itemId_param0);

			{
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Constant);
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Static);
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Random);
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Title);
				ParseBasicAndSpecialStats_v12(handler, blockOutput.Stats.Empowerment1);
			}
			ParseItemEnchantment_v12(handler, blockOutput.Enchantment);

			{

				Read<int>("limitBreakLevel", handler, blockOutput.LimitBreak.Level, "\t\t\t");

				{

					int statCount_var4 = 0;
					Read<int>("statCount", handler, statCount_var4, "\t\t\t\t");

					ResizeVector(handler, blockOutput.LimitBreak.Stats.Basic, statCount_var4);

					for (int i = 0; i < statCount_var4 && !handler.PacketStream().HasRecentlyFailed; ++i)
					{
						ParseBasicStat_v12(handler, blockOutput.LimitBreak.Stats.Basic[i]);
					}

					Read<int>("statCount", handler, statCount_var4, "\t\t\t\t");

					ResizeVector(handler, blockOutput.LimitBreak.Stats.Special, statCount_var4);

					for (int i = 0; i < statCount_var4 && !handler.PacketStream().HasRecentlyFailed; ++i)
					{
						ParseSpecialStat_v12(handler, blockOutput.LimitBreak.Stats.Special[i]);
					}
				}
			}

			unsigned char extraDataType_var5 = 0;

			if (!handler.PacketStream().HasRecentlyFailed)
			{
				extraDataType_var5 = handler.GetItemExtraDataType(itemId_param0);
			}

			ValidateValues(stream, "extraDataType", "\t\t", extraDataType_var5, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4);

			if (extraDataType_var5 == 1)
			{
				ParseItemTemplate_v12(handler, blockOutput.Template);
				ParseItemBlueprint_v12(handler, blockOutput.Blueprint);
			}

			if (extraDataType_var5 == 2)
			{
				ParseItemPet_v12(handler, blockOutput.Pet);
			}

			if (extraDataType_var5 == 3)
			{
				ParseItemMusic_v12(handler, blockOutput.Music);
			}

			if (extraDataType_var5 == 4)
			{
				ParseItemBadge_v2486(handler, blockOutput.Badge);
			}
			ParseItemTransfer_v12(handler, blockOutput.ItemTransfer);
			ParseItemSocket_v12(handler, blockOutput.ItemSocket);
			ParseItemCouple_v12(handler, blockOutput.ItemCouple);
			ParseItemBinding_v12(handler, blockOutput.Binding);
		}

		void ParseEquipmentData_v2486(PacketHandler& handler, Maple::Game::EquipmentData& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("itemId", handler, blockOutput.ItemId, "\t\t");
			Read<long long>("itemInstanceId", handler, blockOutput.ItemInstanceId, "\t\t");
			Read<unsigned char>("slot", handler, blockOutput.Slot, "\t\t");
			Read<int>("rarity", handler, blockOutput.Rarity, "\t\t");

			ValidateValues(stream, "rarity", "\t\t", blockOutput.Rarity, (int)0, (int)1, (int)2, (int)3, (int)4, (int)5, (int)6);
			ParseItemData_v2486(handler, blockOutput.Data, (unsigned int)blockOutput.ItemId);
		}

		void ParseCharacterEquipmentItems_v2486(PacketHandler& handler, Maple::Game::CharacterEquipment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char equipmentCount_var0 = 0;
			Read<unsigned char>("equipmentCount", handler, equipmentCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Equipment, equipmentCount_var0);

			for (unsigned char i = 0; i < equipmentCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				ParseEquipmentData_v2486(handler, blockOutput.Equipment[i]);
			}
		}

		void ParseCharacterEquipmentBadges_v2486(PacketHandler& handler, Maple::Game::CharacterEquipment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char badgeCount_var0 = 0;
			Read<unsigned char>("badgeCount", handler, badgeCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Badges, badgeCount_var0);

			for (unsigned char i = 0; i < badgeCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<unsigned char>("slot", handler, blockOutput.Badges[i].Slot, "\t\t\t");
				Read<int>("itemId", handler, blockOutput.Badges[i].ItemId, "\t\t\t");
				Read<long long>("itemInstanceId", handler, blockOutput.Badges[i].ItemInstanceId, "\t\t\t");
				Read<int>("rarity", handler, blockOutput.Badges[i].Rarity, "\t\t\t");

				ValidateValues(stream, "rarity", "\t\t\t", blockOutput.Badges[i].Rarity, (int)0, (int)1, (int)2, (int)3, (int)4, (int)5, (int)6);
				ParseItemData_v2486(handler, blockOutput.Badges[i].Data, (unsigned int)blockOutput.Badges[i].ItemId);
			}
		}

		void ParseCharacterEquipmentSkins_v2486(PacketHandler& handler, Maple::Game::CharacterEquipment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<bool>("hasSkin", handler, blockOutput.HasSkin, "\t\t");

			ValidateValues(stream, "hasSkin", "\t\t", blockOutput.HasSkin, (bool)0, (bool)1);

			if (blockOutput.HasSkin)
			{
				long long unknown_var1 = 0;
				Read<long long>("unknown", handler, unknown_var1, "\t\t\t");

				long long unknown_var2 = 0;
				Read<long long>("unknown", handler, unknown_var2, "\t\t\t");

				unsigned char skinCount_var3 = 0;
				Read<unsigned char>("skinCount", handler, skinCount_var3, "\t\t\t");

				ResizeVector(handler, blockOutput.Skins, skinCount_var3);

				for (unsigned char i = 0; i < skinCount_var3 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					ParseEquipmentData_v2486(handler, blockOutput.Skins[i]);
				}
			}
		}

		void ParseCharacterListEntry_v2486(PacketHandler& handler, Maple::Game::CharacterListEntry& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			ParseCharacterData_v12(handler, blockOutput.Character);

			Read<std::wstring>("profileUrl", handler, blockOutput.ProfileUrl, "\t\t");
			Read<long long>("deleteTime", handler, blockOutput.DeleteTime, "\t\t");
			ParseCharacterEquipmentItems_v2486(handler, blockOutput.CharacterEquipment);
			ParseCharacterEquipmentBadges_v2486(handler, blockOutput.CharacterEquipment);
			ParseCharacterEquipmentSkins_v2486(handler, blockOutput.CharacterEquipment);
		}

		void ParseBasicStat_v2497(PacketHandler& handler, Maple::Game::BasicStat& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<short>("type", handler, blockOutput.Type, "\t\t");
			Read<int>("value", handler, blockOutput.Value, "\t\t");
		}

		void ParseSpecialStat_v2497(PacketHandler& handler, Maple::Game::SpecialStat& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<short>("type", handler, blockOutput.Type, "\t\t");
			Read<int>("value", handler, blockOutput.Value, "\t\t");

			bool dummy_var2 = false;

			if (!handler.PacketStream().HasRecentlyFailed)
			{
				dummy_var2 = handler.StatIntToFloat(blockOutput.Value);
			}

			ValidateValues(stream, "dummy", "\t\t", dummy_var2, (bool)0, (bool)1);
		}

		void ParseBasicAndSpecialStats_v2497(PacketHandler& handler, Maple::Game::BonusStats& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			short statCount_var0 = 0;
			Read<short>("statCount", handler, statCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Basic, statCount_var0);

			for (short i = 0; i < statCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				ParseBasicStat_v2497(handler, blockOutput.Basic[i]);
			}

			Read<short>("statCount", handler, statCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Special, statCount_var0);

			for (short i = 0; i < statCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				ParseSpecialStat_v2497(handler, blockOutput.Special[i]);
			}

			Read<int>("empowermentItemId", handler, blockOutput.EmpowermentItemId, "\t\t");
		}

		void ParseItemData_v2497(PacketHandler& handler, Maple::Game::ItemData& blockOutput, unsigned int itemId_param0)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("amount", handler, blockOutput.Amount, "\t\t");

			int unknown_var1 = 0;
			Read<int>("unknown", handler, unknown_var1, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var1, (int)18446744073709551615);

			int unknown_var2 = 0;
			Read<int>("unknown", handler, unknown_var2, "\t\t");
			ParseItemHeader_v12(handler, blockOutput);
			ParseItemCustomization_v12(handler, blockOutput.Customization, itemId_param0);

			{
				ParseBasicAndSpecialStats_v2497(handler, blockOutput.Stats.Static);
				ParseBasicAndSpecialStats_v2497(handler, blockOutput.Stats.Random);
				ParseBasicAndSpecialStats_v2497(handler, blockOutput.Stats.Empowerment1);
			}

			{
				ParseItemEnchantmentHeader_v12(handler, blockOutput.Enchantment);

				unsigned char basicStatCount_var3 = 0;
				Read<unsigned char>("basicStatCount", handler, basicStatCount_var3, "\t\t\t");

				ResizeVector(handler, blockOutput.Enchantment.Basic, basicStatCount_var3);

				for (unsigned char i = 0; i < basicStatCount_var3 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					Read<int>("type", handler, blockOutput.Enchantment.Basic[i].Type, "\t\t\t\t");
					Read<int>("value", handler, blockOutput.Enchantment.Basic[i].Value, "\t\t\t\t");
				}
			}

			{
				Read<int>("limitBreakLevel", handler, blockOutput.LimitBreak.Level, "\t\t\t");

				{

					int statCount_var7 = 0;
					Read<int>("statCount", handler, statCount_var7, "\t\t\t\t");

					ResizeVector(handler, blockOutput.LimitBreak.Stats.Basic, statCount_var7);

					for (int i = 0; i < statCount_var7 && !handler.PacketStream().HasRecentlyFailed; ++i)
					{
						ParseBasicStat_v2497(handler, blockOutput.LimitBreak.Stats.Basic[i]);
					}

					Read<int>("statCount", handler, statCount_var7, "\t\t\t\t");

					ResizeVector(handler, blockOutput.LimitBreak.Stats.Special, statCount_var7);

					for (int i = 0; i < statCount_var7 && !handler.PacketStream().HasRecentlyFailed; ++i)
					{
						ParseSpecialStat_v2497(handler, blockOutput.LimitBreak.Stats.Special[i]);
					}
				}
			}

			unsigned char extraDataType_var8 = 0;

			if (!handler.PacketStream().HasRecentlyFailed)
			{
				extraDataType_var8 = handler.GetItemExtraDataType(itemId_param0);
			}

			ValidateValues(stream, "extraDataType", "\t\t", extraDataType_var8, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4);

			if (extraDataType_var8 == 1)
			{
				ParseItemTemplate_v12(handler, blockOutput.Template);
				ParseItemBlueprint_v12(handler, blockOutput.Blueprint);
			}

			if (extraDataType_var8 == 2)
			{
				ParseItemPet_v12(handler, blockOutput.Pet);
			}

			if (extraDataType_var8 == 3)
			{
				ParseItemMusic_v12(handler, blockOutput.Music);
			}

			if (extraDataType_var8 == 4)
			{
				ParseItemBadge_v2486(handler, blockOutput.Badge);
			}
			ParseItemTransfer_v12(handler, blockOutput.ItemTransfer);
			ParseItemSocket_v12(handler, blockOutput.ItemSocket);
			ParseItemCouple_v12(handler, blockOutput.ItemCouple);
			ParseItemBinding_v12(handler, blockOutput.Binding);
		}

		void ParseEquipmentData_v2497(PacketHandler& handler, Maple::Game::EquipmentData& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("itemId", handler, blockOutput.ItemId, "\t\t");
			Read<long long>("itemInstanceId", handler, blockOutput.ItemInstanceId, "\t\t");
			Read<unsigned char>("slot", handler, blockOutput.Slot, "\t\t");
			Read<int>("rarity", handler, blockOutput.Rarity, "\t\t");

			ValidateValues(stream, "rarity", "\t\t", blockOutput.Rarity, (int)0, (int)1, (int)2, (int)3, (int)4, (int)5, (int)6);
			ParseItemData_v2497(handler, blockOutput.Data, (unsigned int)blockOutput.ItemId);
		}

		void ParseCharacterEquipmentItems_v2497(PacketHandler& handler, Maple::Game::CharacterEquipment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char equipmentCount_var0 = 0;
			Read<unsigned char>("equipmentCount", handler, equipmentCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Equipment, equipmentCount_var0);

			for (unsigned char i = 0; i < equipmentCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				ParseEquipmentData_v2497(handler, blockOutput.Equipment[i]);
			}
		}

		void ParseCharacterEquipmentBadges_v2497(PacketHandler& handler, Maple::Game::CharacterEquipment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char badgeCount_var0 = 0;
			Read<unsigned char>("badgeCount", handler, badgeCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Badges, badgeCount_var0);

			for (unsigned char i = 0; i < badgeCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<unsigned char>("slot", handler, blockOutput.Badges[i].Slot, "\t\t\t");
				Read<int>("itemId", handler, blockOutput.Badges[i].ItemId, "\t\t\t");
				Read<long long>("itemInstanceId", handler, blockOutput.Badges[i].ItemInstanceId, "\t\t\t");
				Read<int>("rarity", handler, blockOutput.Badges[i].Rarity, "\t\t\t");

				ValidateValues(stream, "rarity", "\t\t\t", blockOutput.Badges[i].Rarity, (int)0, (int)1, (int)2, (int)3, (int)4, (int)5, (int)6);
				ParseItemData_v2497(handler, blockOutput.Badges[i].Data, (unsigned int)blockOutput.Badges[i].ItemId);
			}
		}

		void ParseCharacterEquipmentSkins_v2497(PacketHandler& handler, Maple::Game::CharacterEquipment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<bool>("hasSkin", handler, blockOutput.HasSkin, "\t\t");

			ValidateValues(stream, "hasSkin", "\t\t", blockOutput.HasSkin, (bool)0, (bool)1);

			if (blockOutput.HasSkin)
			{
				long long unknown_var1 = 0;
				Read<long long>("unknown", handler, unknown_var1, "\t\t\t");

				long long unknown_var2 = 0;
				Read<long long>("unknown", handler, unknown_var2, "\t\t\t");

				unsigned char skinCount_var3 = 0;
				Read<unsigned char>("skinCount", handler, skinCount_var3, "\t\t\t");

				ResizeVector(handler, blockOutput.Skins, skinCount_var3);

				for (unsigned char i = 0; i < skinCount_var3 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					ParseEquipmentData_v2497(handler, blockOutput.Skins[i]);
				}
			}
		}

		void ParseCharacterListEntry_v2497(PacketHandler& handler, Maple::Game::CharacterListEntry& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			ParseCharacterData_v12(handler, blockOutput.Character);

			Read<std::wstring>("profileUrl", handler, blockOutput.ProfileUrl, "\t\t");
			Read<long long>("deleteTime", handler, blockOutput.DeleteTime, "\t\t");
			ParseCharacterEquipmentItems_v2497(handler, blockOutput.CharacterEquipment);
			ParseCharacterEquipmentBadges_v2497(handler, blockOutput.CharacterEquipment);
			ParseCharacterEquipmentSkins_v2497(handler, blockOutput.CharacterEquipment);
		}

		void ParseCharacterDetailsSpecialStats_v2497(PacketHandler& handler, Server::CharacterInfoPacket::CharacterStats& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			int specialStatCount_var0 = 0;
			Read<int>("specialStatCount", handler, specialStatCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Special, specialStatCount_var0);

			for (int i = 0; i < specialStatCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<short>("type", handler, blockOutput.Special[i].Type, "\t\t\t");
				Read<int>("value", handler, blockOutput.Special[i].Value, "\t\t\t");

				bool dummy_var3 = false;

				if (!handler.PacketStream().HasRecentlyFailed)
				{
					dummy_var3 = handler.StatIntToFloat(blockOutput.Special[i].Value);
				}

				ValidateValues(stream, "dummy", "\t\t\t", dummy_var3, (bool)0, (bool)1);
			}
		}

		void ParseSkillTreeData_v2502(PacketHandler& handler, Maple::Game::SkillTreeData& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("job", handler, blockOutput.Job, "\t\t");

			unsigned char unknownCount_var1 = 0;
			Read<unsigned char>("unknownCount", handler, unknownCount_var1, "\t\t");

			ValidateValues(stream, "unknownCount", "\t\t", unknownCount_var1, (unsigned char)1);

			Read<int>("jobCode", handler, blockOutput.JobCode, "\t\t");
			ParseSkillTreePageData_v12(handler, blockOutput.Active);
		}

		void ParseCharacterData_v2509(PacketHandler& handler, Maple::Game::CharacterData& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<long long>("accountId", handler, blockOutput.AccountId, "\t\t");
			Read<long long>("characterId", handler, blockOutput.CharacterId, "\t\t");
			Read<std::wstring>("name", handler, blockOutput.Name, "\t\t");
			Read<unsigned char>("gender", handler, blockOutput.Gender, "\t\t");

			ValidateValues(stream, "gender", "\t\t", blockOutput.Gender, (unsigned char)0, (unsigned char)1);

			unsigned char unknown_var4 = 0;
			Read<unsigned char>("unknown", handler, unknown_var4, "\t\t");

			long long unknown_var5 = 0;
			Read<long long>("unknown", handler, unknown_var5, "\t\t");

			int unknown_var6 = 0;
			Read<int>("unknown", handler, unknown_var6, "\t\t");
			Read<int>("mapId", handler, blockOutput.MapId, "\t\t");
			Read<int>("mapInstanceId", handler, blockOutput.MapInstanceId, "\t\t");

			int unknown_var9 = 0;
			Read<int>("unknown", handler, unknown_var9, "\t\t");
			Read<unsigned short>("level", handler, blockOutput.Level, "\t\t");
			Read<unsigned short>("channel", handler, blockOutput.Channel, "\t\t");
			Read<int>("jobCode", handler, blockOutput.JobCode, "\t\t");
			Read<int>("job", handler, blockOutput.Job, "\t\t");
			Read<int>("currentHp", handler, blockOutput.Hp.Current, "\t\t");
			Read<int>("maxHp", handler, blockOutput.Hp.Max, "\t\t");

			short unknown_var16 = 0;
			Read<short>("unknown", handler, unknown_var16, "\t\t");

			long long unknown_var17 = 0;
			Read<long long>("unknown", handler, unknown_var17, "\t\t");

			long long storageCooldown_var18 = 0;
			Read<long long>("storageCooldown", handler, storageCooldown_var18, "\t\t");

			blockOutput.StorageCooldown = (long)storageCooldown_var18;

			long long doctorCooldown_var19 = 0;
			Read<long long>("doctorCooldown", handler, doctorCooldown_var19, "\t\t");

			blockOutput.DoctorCooldown = (long)doctorCooldown_var19;

			Read<int>("returnMapId", handler, blockOutput.ReturnMapId, "\t\t");
			Read<Vector3S>("returnPosition", handler, blockOutput.ReturnPosition, "\t\t");
			Read<int>("gearScore", handler, blockOutput.GearScore, "\t\t");

			Color4I_BGRA skinColorPrimary_var23;
			Read<Color4I_BGRA>("skinColorPrimary", handler, skinColorPrimary_var23, "\t\t");

			blockOutput.SkinColor.Primary = (Color4I)skinColorPrimary_var23;

			Color4I_BGRA skinColorSecondary_var24;
			Read<Color4I_BGRA>("skinColorSecondary", handler, skinColorSecondary_var24, "\t\t");

			blockOutput.SkinColor.Secondary = (Color4I)skinColorSecondary_var24;

			long long creationTime_var25 = 0;
			Read<long long>("creationTime", handler, creationTime_var25, "\t\t");

			blockOutput.CreationTime = (long)creationTime_var25;

			Read<int>("combatTrophyCount", handler, blockOutput.CombatTrophyCount, "\t\t");
			Read<int>("adventureTrophyCount", handler, blockOutput.AdventureTrophyCount, "\t\t");
			Read<int>("lifestyleTrophyCount", handler, blockOutput.LifestyleTrophyCount, "\t\t");
			Read<long long>("guildId", handler, blockOutput.GuildId, "\t\t");
			Read<std::wstring>("guildName", handler, blockOutput.GuildName, "\t\t");
			Read<std::wstring>("guildMotto", handler, blockOutput.GuildMotto, "\t\t");
			Read<std::wstring>("profileUrl", handler, blockOutput.ProfileUrl, "\t\t");

			unsigned char clubCount_var33 = 0;
			Read<unsigned char>("clubCount", handler, clubCount_var33, "\t\t");

			ResizeVector(handler, blockOutput.Clubs, clubCount_var33);

			for (unsigned char i = 0; i < clubCount_var33 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<bool>("hasClub", handler, blockOutput.Clubs[i].HasClub, "\t\t\t");

				ValidateValues(stream, "hasClub", "\t\t\t", blockOutput.Clubs[i].HasClub, (bool)0, (bool)1);

				if (blockOutput.Clubs[i].HasClub)
				{
					Read<long long>("clubId", handler, blockOutput.Clubs[i].ClubId, "\t\t\t\t");
					Read<std::wstring>("clubName", handler, blockOutput.Clubs[i].ClubName, "\t\t\t\t");
				}
			}

			unsigned char unknown_var37 = 0;
			Read<unsigned char>("unknown", handler, unknown_var37, "\t\t");

			{

				int placeholderMastery_var38 = 0;
				Read<int>("placeholderMastery", handler, placeholderMastery_var38, "\t\t\t");

				ValidateValues(stream, "placeholderMastery", "\t\t\t", placeholderMastery_var38, (int)0);

				Read<int>("fishingMastery", handler, blockOutput.Mastery.Fishing, "\t\t\t");
				Read<int>("performanceMastery", handler, blockOutput.Mastery.Performance, "\t\t\t");
				Read<int>("miningMastery", handler, blockOutput.Mastery.Mining, "\t\t\t");
				Read<int>("foragingMastery", handler, blockOutput.Mastery.Foraging, "\t\t\t");
				Read<int>("ranchingMastery", handler, blockOutput.Mastery.Ranching, "\t\t\t");
				Read<int>("farmingMastery", handler, blockOutput.Mastery.Farming, "\t\t\t");
				Read<int>("smithingMastery", handler, blockOutput.Mastery.Smithing, "\t\t\t");
				Read<int>("handicraftMastery", handler, blockOutput.Mastery.Handicraft, "\t\t\t");
				Read<int>("alchemyMastery", handler, blockOutput.Mastery.Alchemy, "\t\t\t");
				Read<int>("cookingMastery", handler, blockOutput.Mastery.Cooking, "\t\t\t");
				Read<int>("petTamingMastery", handler, blockOutput.Mastery.PetTaming, "\t\t\t");
			}
			Read<std::wstring>("username", handler, blockOutput.Username, "\t\t");
			Read<long long>("sessionId", handler, blockOutput.SessionId, "\t\t");

			int unknownCount_var52 = 0;
			Read<int>("unknownCount", handler, unknownCount_var52, "\t\t");

			ResizeVector(handler, blockOutput.UnknownItems, unknownCount_var52);

			for (int i = 0; i < unknownCount_var52 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				long long unknown_var53 = 0;
				Read<long long>("unknown", handler, unknown_var53, "\t\t\t");
			}

			unsigned char unknown_var54 = 0;
			Read<unsigned char>("unknown", handler, unknown_var54, "\t\t");

			bool unknown_var55 = false;
			Read<bool>("unknown", handler, unknown_var55, "\t\t");

			ValidateValues(stream, "unknown", "\t\t", unknown_var55, (bool)0, (bool)1);

			long long birthday_var56 = 0;
			Read<long long>("birthday", handler, birthday_var56, "\t\t");

			blockOutput.Birthday = (long)birthday_var56;

			int unknown_var57 = 0;
			Read<int>("unknown", handler, unknown_var57, "\t\t");

			int unknown_var58 = 0;
			Read<int>("unknown", handler, unknown_var58, "\t\t");

			long long unknownTime_var59 = 0;
			Read<long long>("unknownTime", handler, unknownTime_var59, "\t\t");
			Read<int>("prestigeLevel", handler, blockOutput.PrestigeLevel, "\t\t");

			long long unknownTime_var61 = 0;
			Read<long long>("unknownTime", handler, unknownTime_var61, "\t\t");

			int unknownCount2_var62 = 0;
			Read<int>("unknownCount2", handler, unknownCount2_var62, "\t\t");

			ResizeVector(handler, blockOutput.UnknownItems2, unknownCount2_var62);

			for (int i = 0; i < unknownCount2_var62 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				long long unknown_var63 = 0;
				Read<long long>("unknown", handler, unknown_var63, "\t\t\t");
			}

			int unknownCount3_var64 = 0;
			Read<int>("unknownCount3", handler, unknownCount3_var64, "\t\t");

			ResizeVector(handler, blockOutput.UnknownItems3, unknownCount3_var64);

			for (int i = 0; i < unknownCount3_var64 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				long long unknown_var65 = 0;
				Read<long long>("unknown", handler, unknown_var65, "\t\t\t");
			}

			short unknown_var66 = 0;
			Read<short>("unknown", handler, unknown_var66, "\t\t");

			long long unknown_var67 = 0;
			Read<long long>("unknown", handler, unknown_var67, "\t\t");
		}

		void ParseEquipmentData_v2509(PacketHandler& handler, Maple::Game::EquipmentData& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("itemId", handler, blockOutput.ItemId, "\t\t");
			Read<long long>("itemInstanceId", handler, blockOutput.ItemInstanceId, "\t\t");
			Read<unsigned char>("slot", handler, blockOutput.Slot, "\t\t");
			Read<int>("rarity", handler, blockOutput.Rarity, "\t\t");

			ValidateValues(stream, "rarity", "\t\t", blockOutput.Rarity, (int)0, (int)1, (int)2, (int)3, (int)4, (int)5, (int)6);
			ParseItemData_v2497(handler, blockOutput.Data, (unsigned int)blockOutput.ItemId);
		}

		void ParseCharacterEquipmentItems_v2509(PacketHandler& handler, Maple::Game::CharacterEquipment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char equipmentCount_var0 = 0;
			Read<unsigned char>("equipmentCount", handler, equipmentCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Equipment, equipmentCount_var0);

			for (unsigned char i = 0; i < equipmentCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				ParseEquipmentData_v2509(handler, blockOutput.Equipment[i]);
			}
		}

		void ParseCharacterEquipmentBadges_v2509(PacketHandler& handler, Maple::Game::CharacterEquipment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char badgeCount_var0 = 0;
			Read<unsigned char>("badgeCount", handler, badgeCount_var0, "\t\t");

			ResizeVector(handler, blockOutput.Badges, badgeCount_var0);

			for (unsigned char i = 0; i < badgeCount_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<unsigned char>("slot", handler, blockOutput.Badges[i].Slot, "\t\t\t");
				Read<int>("itemId", handler, blockOutput.Badges[i].ItemId, "\t\t\t");
				Read<long long>("itemInstanceId", handler, blockOutput.Badges[i].ItemInstanceId, "\t\t\t");
				Read<int>("rarity", handler, blockOutput.Badges[i].Rarity, "\t\t\t");

				ValidateValues(stream, "rarity", "\t\t\t", blockOutput.Badges[i].Rarity, (int)0, (int)1, (int)2, (int)3, (int)4, (int)5, (int)6);
				ParseItemData_v2497(handler, blockOutput.Badges[i].Data, (unsigned int)blockOutput.Badges[i].ItemId);
			}
		}

		void ParseCharacterEquipmentSkins_v2509(PacketHandler& handler, Maple::Game::CharacterEquipment& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<bool>("hasSkin", handler, blockOutput.HasSkin, "\t\t");

			ValidateValues(stream, "hasSkin", "\t\t", blockOutput.HasSkin, (bool)0, (bool)1);

			if (blockOutput.HasSkin)
			{
				long long unknown_var1 = 0;
				Read<long long>("unknown", handler, unknown_var1, "\t\t\t");

				long long unknown_var2 = 0;
				Read<long long>("unknown", handler, unknown_var2, "\t\t\t");

				unsigned char skinCount_var3 = 0;
				Read<unsigned char>("skinCount", handler, skinCount_var3, "\t\t\t");

				ResizeVector(handler, blockOutput.Skins, skinCount_var3);

				for (unsigned char i = 0; i < skinCount_var3 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					ParseEquipmentData_v2509(handler, blockOutput.Skins[i]);
				}
			}
		}

		void ParseCharacterListEntry_v2509(PacketHandler& handler, Maple::Game::CharacterListEntry& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			ParseCharacterData_v2509(handler, blockOutput.Character);

			Read<std::wstring>("profileUrl", handler, blockOutput.ProfileUrl, "\t\t");
			Read<long long>("deleteTime", handler, blockOutput.DeleteTime, "\t\t");
			ParseCharacterEquipmentItems_v2509(handler, blockOutput.CharacterEquipment);
			ParseCharacterEquipmentBadges_v2509(handler, blockOutput.CharacterEquipment);
			ParseCharacterEquipmentSkins_v2509(handler, blockOutput.CharacterEquipment);
		}

		void ParseSkillTreePageData_v2512(PacketHandler& handler, Maple::Game::SkillTreePageData& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char count_var0 = 0;
			Read<unsigned char>("count", handler, count_var0, "\t\t");

			ResizeVector(handler, blockOutput.Skills, count_var0);

			for (unsigned char i = 0; i < count_var0 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<int>("skillId", handler, blockOutput.Skills[i].SkillId, "\t\t\t");
				Read<int>("skillLevel", handler, blockOutput.Skills[i].SkillLevel, "\t\t\t");
				Read<bool>("notify", handler, blockOutput.Skills[i].Notify, "\t\t\t");

				ValidateValues(stream, "notify", "\t\t\t", blockOutput.Skills[i].Notify, (bool)0, (bool)1);

				Read<bool>("enabled", handler, blockOutput.Skills[i].Enabled, "\t\t\t");

				ValidateValues(stream, "enabled", "\t\t\t", blockOutput.Skills[i].Enabled, (bool)0, (bool)1);

				Read<bool>("coreSkill", handler, blockOutput.Skills[i].CoreSkill, "\t\t\t");

				ValidateValues(stream, "coreSkill", "\t\t\t", blockOutput.Skills[i].CoreSkill, (bool)0, (bool)1);
			}
		}

		void ParseSkillTreeData_v2512(PacketHandler& handler, Maple::Game::SkillTreeData& blockOutput)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Read<int>("job", handler, blockOutput.Job, "\t\t");
			Read<int>("jobCode", handler, blockOutput.JobCode, "\t\t");
			ParseSkillTreePageData_v2512(handler, blockOutput.Active);
		}

		template <>
		void ParsePacket<12, ClientPacket, 0x21>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)0);

			if (mode_var0 == 0)
			{
				Client::StateSkillPacket output0;

				Read<long long>("skillSn", handler, output0.SkillSn, "\t\t");
				Read<unsigned int>("serverTick", handler, output0.ServerTick, "\t\t");
				Read<unsigned int>("skillId", handler, output0.SkillId, "\t\t");
				Read<unsigned short>("skillLevel", handler, output0.SkillLevel, "\t\t");

				ValidateValues(stream, "skillLevel", "\t\t", output0.SkillLevel, (unsigned short)1);

				Read<int>("animation", handler, output0.Animation, "\t\t");
				Read<int>("clientTick", handler, output0.ClientTick, "\t\t");

				long long unknown_var7 = 0;
				Read<long long>("unknown", handler, unknown_var7, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::StateSkillPacket>(output0);

					return;
				}
			}
		}

		template <>
		void ParsePacket<12, ClientPacket, 0x1>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Client::ResponseVersionPacket output0;

			Read<unsigned int>("version", handler, output0.Version, "\t");

			unsigned short feature_var1 = 0;
			Read<unsigned short>("feature", handler, feature_var1, "\t");

			unsigned short locale_var2 = 0;
			Read<unsigned short>("locale", handler, locale_var2, "\t");

			if (stream.Succeeded())
			{
				handler.PacketParsed<Client::ResponseVersionPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ClientPacket, 0xa>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Client::ResponseHeartbeatPacket output0;

			Read<int>("serverTick", handler, output0.ServerTick, "\t");
			Read<int>("clientTick", handler, output0.ClientTick, "\t");

			if (stream.Succeeded())
			{
				handler.PacketParsed<Client::ResponseHeartbeatPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ClientPacket, 0x2>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Client::HandshakePacket output0;

			Read<unsigned char>("function", handler, output0.Function, "\t");

			int count_var1 = 0;
			Read<int>("count", handler, count_var1, "\t");

			ResizeVector(handler, output0.Data, count_var1);

			for (int i = 0; i < count_var1 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<std::wstring>("unknown", handler, output0.Data[i].String, "\t\t");

				long long unknown_var3 = 0;
				Read<long long>("unknown", handler, unknown_var3, "\t\t");
			}

			if (stream.Succeeded())
			{
				handler.PacketParsed<Client::HandshakePacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ClientPacket, 0x3>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)0, (unsigned char)1, (unsigned char)2);

			if (mode_var0 == 0)
			{
				Client::LogInPacket output0;

				Read<std::wstring>("username", handler, output0.Username, "\t\t");
				Read<std::wstring>("password", handler, output0.Password, "\t\t");

				short unknown_var3 = 0;
				Read<short>("unknown", handler, unknown_var3, "\t\t");
				Read<unsigned long long>("machineLow", handler, output0.MachineLow, "\t\t");
				Read<unsigned long long>("machineHigh", handler, output0.MachineHigh, "\t\t");

				int unknownMachineIdPtr_var6 = 0;
				Read<int>("unknownMachineIdPtr", handler, unknownMachineIdPtr_var6, "\t\t");
				Read<int>("sessionKey", handler, output0.SessionKey, "\t\t");
				Read<std::string>("locale", handler, output0.Locale, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::LogInPacket>(output0);

					return;
				}
			}

			if (mode_var0 != 0)
			{
				Client::LogInAuthTokenPacket output1;

				long long unknown_var9 = 0;
				Read<long long>("unknown", handler, unknown_var9, "\t\t");

				std::string authToken_var10;
				Read<std::string>("authToken", handler, authToken_var10, "\t\t");

				short unknown_var11 = 0;
				Read<short>("unknown", handler, unknown_var11, "\t\t");

				int unknown_var12 = 0;
				Read<int>("unknown", handler, unknown_var12, "\t\t");
				Read<unsigned long long>("machineLow", handler, output1.MachineLow, "\t\t");
				Read<unsigned long long>("machineHigh", handler, output1.MachineHigh, "\t\t");

				int unknown_var15 = 0;
				Read<int>("unknown", handler, unknown_var15, "\t\t");

				int unknown_var16 = 0;
				Read<int>("unknown", handler, unknown_var16, "\t\t");
				Read<std::string>("locale", handler, output1.Locale, "\t\t");

				unsigned char unknown_var18 = 0;
				Read<unsigned char>("unknown", handler, unknown_var18, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::LogInAuthTokenPacket>(output1);

					return;
				}
			}
		}

		template <>
		void ParsePacket<12, ClientPacket, 0x4>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Client::ResponseKeyPacket output0;

			Read<long long>("accountId", handler, output0.AccountId, "\t");

			if (output0.AccountId != 0)
			{
				Read<int>("tokenA", handler, output0.TokenA, "\t\t");
				Read<int>("tokenB", handler, output0.TokenB, "\t\t");
				Read<unsigned long long>("machineHigh", handler, output0.MachineHigh, "\t\t");
				Read<unsigned long long>("machineLow", handler, output0.MachineLow, "\t\t");
			}

			if (stream.Succeeded())
			{
				handler.PacketParsed<Client::ResponseKeyPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ClientPacket, 0xd>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Client::RequestQuitPacket output0;

			Read<unsigned char>("mode", handler, output0.Mode, "\t");

			ValidateValues(stream, "mode", "\t", output0.Mode, (unsigned char)0, (unsigned char)1);

			if (stream.Succeeded())
			{
				handler.PacketParsed<Client::RequestQuitPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ClientPacket, 0x20>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4);

			if (mode_var0 == 0)
			{
				Client::SkillCastPacket output0;

				Read<long long>("skillSn", handler, output0.SkillSn, "\t\t");
				Read<int>("serverTick", handler, output0.ServerTick, "\t\t");
				Read<unsigned int>("skillId", handler, output0.SkillId, "\t\t");
				Read<unsigned short>("skillLevel", handler, output0.SkillLevel, "\t\t");
				Read<unsigned char>("attackPoint", handler, output0.AttackPoint, "\t\t");
				Read<Vector3S>("position", handler, output0.Position, "\t\t");
				Read<Vector3S>("direction", handler, output0.Direction, "\t\t");
				Read<Vector3S>("rotation", handler, output0.Rotation, "\t\t");

				float unknown_var9 = 0;
				Read<float>("unknown", handler, unknown_var9, "\t\t");

				ValidateValues(stream, "unknown", "\t\t", unknown_var9, (float)0);

				Read<int>("clientTick", handler, output0.ClientTick, "\t\t");

				bool unknown_var11 = false;
				Read<bool>("unknown", handler, unknown_var11, "\t\t");

				ValidateValues(stream, "unknown", "\t\t", unknown_var11, (bool)0, (bool)1);

				long long unknown_var12 = 0;
				Read<long long>("unknown", handler, unknown_var12, "\t\t");

				ValidateValues(stream, "unknown", "\t\t", unknown_var12, (long long)0);

				Read<bool>("flag", handler, output0.Flag, "\t\t");

				ValidateValues(stream, "flag", "\t\t", output0.Flag, (bool)0, (bool)1);

				if (output0.Flag)
				{
					int unknown_var14 = 0;
					Read<int>("unknown", handler, unknown_var14, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var14, (int)0);

					std::string unknown_var15;
					Read<std::string>("unknown", handler, unknown_var15, "\t\t\t");
				}

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::SkillCastPacket>(output0);

					return;
				}
			}

			if (mode_var0 == 1)
			{
				unsigned char damageMode_var16 = 0;
				Read<unsigned char>("damageMode", handler, damageMode_var16, "\t\t");

				ValidateValues(stream, "damageMode", "\t\t", damageMode_var16, (unsigned char)0, (unsigned char)1, (unsigned char)2);

				if (damageMode_var16 == 0)
				{
					Client::SkillSyncDamagePacket output1;

					Read<long long>("skillSn", handler, output1.SkillSn, "\t\t\t");
					Read<unsigned char>("attackPoint", handler, output1.AttackPoint, "\t\t\t");
					Read<Vector3S>("position", handler, output1.Position, "\t\t\t");
					Read<Vector3S>("rotation", handler, output1.Rotation, "\t\t\t");

					unsigned char count_var21 = 0;
					Read<unsigned char>("count", handler, count_var21, "\t\t\t");

					int unknown_var22 = 0;
					Read<int>("unknown", handler, unknown_var22, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var22, (int)0);

					ResizeVector(handler, output1.Projectiles, count_var21);

					for (unsigned char i = 0; i < count_var21 && !handler.PacketStream().HasRecentlyFailed; ++i)
					{
						auto& projectile_array0 = output1.Projectiles[i];

						Read<long long>("skillAttack", handler, projectile_array0.SkillAttack, "\t\t\t\t");
						Read<int>("targetId", handler, projectile_array0.TargetId, "\t\t\t\t");
						Read<unsigned char>("index", handler, projectile_array0.Index, "\t\t\t\t");

						bool moreTargets_var26 = false;
						Read<bool>("moreTargets", handler, moreTargets_var26, "\t\t\t\t");

						ValidateValues(stream, "moreTargets", "\t\t\t\t", moreTargets_var26, (bool)0, (bool)1);

						size_t j = 0;

						while (moreTargets_var26 && !handler.PacketStream().HasRecentlyFailed)
						{
							projectile_array0.ChainTargets.push_back({});

							auto& nextTarget_array1 = projectile_array0.ChainTargets[j];

							Read<long long>("skillAttack", handler, nextTarget_array1.SkillAttack, "\t\t\t\t\t");
							Read<int>("targetId", handler, nextTarget_array1.TargetId, "\t\t\t\t\t");
							Read<unsigned char>("index", handler, nextTarget_array1.Index, "\t\t\t\t\t");

							unsigned char unknown_var30 = 0;
							Read<unsigned char>("unknown", handler, unknown_var30, "\t\t\t\t\t");

							Read<bool>("moreTargets", handler, moreTargets_var26, "\t\t\t\t\t");

							ValidateValues(stream, "moreTargets", "\t\t\t\t\t", moreTargets_var26, (bool)0, (bool)1);

							++j;
						}
					}

					if (stream.Succeeded())
					{
						handler.PacketParsed<Client::SkillSyncDamagePacket>(output1);

						return;
					}
				}

				if (damageMode_var16 == 1)
				{
					Client::SkillDamagePacket output2;

					Read<long long>("skillSn", handler, output2.SkillSn, "\t\t\t");
					Read<long long>("skillAttack", handler, output2.SkillAttack, "\t\t\t");
					Read<Vector3S>("position", handler, output2.Position, "\t\t\t");
					Read<Vector3S>("hitPoint", handler, output2.HitPoint, "\t\t\t");
					Read<Vector3S>("rotation", handler, output2.Rotation, "\t\t\t");
					Read<unsigned char>("attackPoint", handler, output2.AttackPoint, "\t\t\t");

					unsigned char count_var37 = 0;
					Read<unsigned char>("count", handler, count_var37, "\t\t\t");

					int unknown_var38 = 0;
					Read<int>("unknown", handler, unknown_var38, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var38, (int)0);

					ResizeVector(handler, output2.Hits, count_var37);

					for (unsigned char i = 0; i < count_var37 && !handler.PacketStream().HasRecentlyFailed; ++i)
					{
						auto& hitActor_array2 = output2.Hits[i];

						Read<int>("entityId", handler, hitActor_array2.EntityId, "\t\t\t\t");

						unsigned char unknown_var40 = 0;
						Read<unsigned char>("unknown", handler, unknown_var40, "\t\t\t\t");

						ValidateValues(stream, "unknown", "\t\t\t\t", unknown_var40, (unsigned char)0);
					}

					if (stream.Succeeded())
					{
						handler.PacketParsed<Client::SkillDamagePacket>(output2);

						return;
					}
				}

				if (damageMode_var16 == 2)
				{
					Client::SkillRegionSkillPacket output3;

					Read<long long>("skillSn", handler, output3.SkillSn, "\t\t\t");
					Read<unsigned char>("attackPoint", handler, output3.AttackPoint, "\t\t\t");

					int unknown_var43 = 0;
					Read<int>("unknown", handler, unknown_var43, "\t\t\t");
					Read<int>("attackCounter", handler, output3.AttackCounter, "\t\t\t");
					Read<Vector3S>("position", handler, output3.Position, "\t\t\t");
					Read<Vector3S>("rotation", handler, output3.Rotation, "\t\t\t");

					if (stream.Succeeded())
					{
						handler.PacketParsed<Client::SkillRegionSkillPacket>(output3);

						return;
					}
				}
			}

			if (mode_var0 == 2)
			{
				Client::SkillSyncPacket output4;

				Read<long long>("skillSn", handler, output4.SkillSn, "\t\t");
				Read<unsigned int>("skillId", handler, output4.SkillId, "\t\t");
				Read<unsigned short>("skillLevel", handler, output4.SkillLevel, "\t\t");
				Read<unsigned char>("motionPoint", handler, output4.MotionPoint, "\t\t");
				Read<Vector3S>("position", handler, output4.Position, "\t\t");

				Vector3S unknown_var52;
				Read<Vector3S>("unknown", handler, unknown_var52, "\t\t");
				Read<Vector3S>("rotation", handler, output4.Rotation, "\t\t");

				Vector3S unknown_var54;
				Read<Vector3S>("unknown", handler, unknown_var54, "\t\t");

				bool unknown_var55 = false;
				Read<bool>("unknown", handler, unknown_var55, "\t\t");

				ValidateValues(stream, "unknown", "\t\t", unknown_var55, (bool)0, (bool)1);

				int unknown_var56 = 0;
				Read<int>("unknown", handler, unknown_var56, "\t\t");

				ValidateValues(stream, "unknown", "\t\t", unknown_var56, (int)0);

				unsigned char unknown_var57 = 0;
				Read<unsigned char>("unknown", handler, unknown_var57, "\t\t");

				ValidateValues(stream, "unknown", "\t\t", unknown_var57, (unsigned char)0);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::SkillSyncPacket>(output4);

					return;
				}
			}

			if (mode_var0 == 3)
			{
				Client::SkillSyncTickPacket output5;

				Read<long long>("skillSn", handler, output5.SkillSn, "\t\t");
				Read<int>("serverTick", handler, output5.ServerTick, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::SkillSyncTickPacket>(output5);

					return;
				}
			}

			if (mode_var0 == 4)
			{
				Client::SkillCancelPacket output6;

				Read<long long>("skillSn", handler, output6.SkillSn, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::SkillCancelPacket>(output6);

					return;
				}
			}
		}

		template <>
		void ParsePacket<12, ClientPacket, 0x22>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)8, (unsigned char)0, (unsigned char)9, (unsigned char)1, (unsigned char)2, (unsigned char)4, (unsigned char)6, (unsigned char)7, (unsigned char)11);

			if (mode_var0 == 0)
			{
				Client::NpcTalkClosePacket output0;

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::NpcTalkClosePacket>(output0);

					return;
				}
			}

			if (mode_var0 == 1)
			{
				Client::NpcTalkBeginPacket output1;

				Read<int>("actorId", handler, output1.ActorId, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::NpcTalkBeginPacket>(output1);

					return;
				}
			}

			if (mode_var0 == 2)
			{
				Client::NpcTalkContinuePacket output2;

				Read<int>("index", handler, output2.Index, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::NpcTalkContinuePacket>(output2);

					return;
				}
			}

			if (mode_var0 == 4)
			{
				Client::NpcTalkEnchantUnknownPacket output3;

				short unknown_var3 = 0;
				Read<short>("unknown", handler, unknown_var3, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::NpcTalkEnchantUnknownPacket>(output3);

					return;
				}
			}

			if (mode_var0 == 6)
			{
				Client::NpcTalkEnchantPacket output4;

				Read<int>("npcId", handler, output4.NpcId, "\t\t");
				Read<int>("scriptId", handler, output4.ScriptId, "\t\t");
				Read<short>("eventType", handler, output4.EventType, "\t\t");

				ValidateValues(stream, "eventType", "\t\t", output4.EventType, (short)1, (short)202, (short)2, (short)3);

				if (output4.EventType == 1)
				{
					Read<long long>("itemInstanceId", handler, output4.ItemInstanceId, "\t\t\t");
				}

				if (output4.EventType == 202)
				{
					short unknown_var8 = 0;
					Read<short>("unknown", handler, unknown_var8, "\t\t\t");
				}

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::NpcTalkEnchantPacket>(output4);

					return;
				}
			}

			if (mode_var0 == 7)
			{
				Client::NpcTalkNextQuestPacket output5;

				Read<int>("questId", handler, output5.QuestId, "\t\t");
				Read<short>("eventType", handler, output5.EventType, "\t\t");

				ValidateValues(stream, "eventType", "\t\t", output5.EventType, (short)0, (short)2);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::NpcTalkNextQuestPacket>(output5);

					return;
				}
			}

			if (mode_var0 == 8)
			{
				Client::NpcTalkAllianceQuestAcceptPacket output6;

				Read<int>("index", handler, output6.Index, "\t\t");
				Read<short>("eventType", handler, output6.EventType, "\t\t");

				ValidateValues(stream, "eventType", "\t\t", output6.EventType, (short)0, (short)2);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::NpcTalkAllianceQuestAcceptPacket>(output6);

					return;
				}
			}

			if (mode_var0 == 9)
			{
				Client::NpcTalkAllianceQuestTalkPacket output7;

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::NpcTalkAllianceQuestTalkPacket>(output7);

					return;
				}
			}

			if (mode_var0 == 11)
			{
				Client::NpcTalkCinematicPacket output8;

				Read<int>("actorId", handler, output8.ActorId, "\t\t");
				Read<int>("index", handler, output8.Index, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::NpcTalkCinematicPacket>(output8);

					return;
				}
			}
		}

		template <>
		void ParsePacket<12, ClientPacket, 0xb7>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Client::SystemInfoPacket output0;

			Read<std::wstring>("info", handler, output0.Info, "\t");

			if (stream.Succeeded())
			{
				handler.PacketParsed<Client::SystemInfoPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0x7>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::Reconnect7Packet output0;

			int unknown_var0 = 0;
			Read<int>("unknown", handler, unknown_var0, "\t");

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::Reconnect7Packet>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0x3e>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)8, (unsigned char)0, (unsigned char)1, (unsigned char)3, (unsigned char)4, (unsigned char)5, (unsigned char)6, (unsigned char)7);

			if (mode_var0 == 0)
			{
				Server::SkillDamageSyncPacket output0;

				Read<long long>("skillSn", handler, output0.SkillSn, "\t\t");
				Read<int>("casterId", handler, output0.CasterId, "\t\t");
				Read<unsigned int>("skillId", handler, output0.SkillId, "\t\t");

				if (output0.SkillId == 0)
				{
					unsigned char unknown_var4 = 0;
					Read<unsigned char>("unknown", handler, unknown_var4, "\t\t\t");
				}

				if (output0.SkillId != 0)
				{
					Read<unsigned short>("skillLevel", handler, output0.SkillLevel, "\t\t\t");
					Read<unsigned char>("motionPoint", handler, output0.MotionPoint, "\t\t\t");
					Read<unsigned char>("attackPoint", handler, output0.AttackPoint, "\t\t\t");
					Read<Vector3Short>("position", handler, output0.Position, "\t\t\t");
					Read<Vector3S>("direction", handler, output0.Direction, "\t\t\t");
					Read<bool>("isChaining", handler, output0.IsChaining, "\t\t\t");

					ValidateValues(stream, "isChaining", "\t\t\t", output0.IsChaining, (bool)0, (bool)1);

					Read<int>("serverTick", handler, output0.ServerTick, "\t\t\t");

					unsigned char count_var12 = 0;
					Read<unsigned char>("count", handler, count_var12, "\t\t\t");

					ResizeVector(handler, output0.Hits, count_var12);

					for (unsigned char i = 0; i < count_var12 && !handler.PacketStream().HasRecentlyFailed; ++i)
					{
						long long unknown_var13 = 0;
						Read<long long>("unknown", handler, unknown_var13, "\t\t\t\t");
						Read<long long>("skillAttack", handler, output0.Hits[i].SkillAttack, "\t\t\t\t");
						Read<int>("targetId", handler, output0.Hits[i].TargetId, "\t\t\t\t");
						Read<short>("animation", handler, output0.Hits[i].Animation, "\t\t\t\t");
					}
				}

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageSyncPacket>(output0);

					return;
				}
			}

			if (mode_var0 == 1)
			{
				Server::SkillDamagePacket output1;
				ParseSkillDamagePacket_v12(handler, output1);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamagePacket>(output1);

					return;
				}
			}

			if (mode_var0 == 3)
			{
				Server::SkillDamageDotPacket output2;
				ParseSkillDamageDotPacket_v12(handler, output2);

				Read<int>("damage", handler, output2.Damage, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageDotPacket>(output2);

					return;
				}
			}

			if (mode_var0 == 4)
			{
				Server::SkillDamageHealPacket output3;
				ParseSkillDamageHealPacket_v12(handler, output3);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageHealPacket>(output3);

					return;
				}
			}

			if (mode_var0 == 5)
			{
				Server::SkillDamageRegionPacket output4;
				ParseSkillDamageRegionPacket_v12(handler, output4);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageRegionPacket>(output4);

					return;
				}
			}

			if (mode_var0 == 6)
			{
				Server::SkillDamageTilePacket output5;
				ParseSkillDamageTilePacket_v12(handler, output5);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageTilePacket>(output5);

					return;
				}
			}

			if (mode_var0 == 7)
			{
				int unknown_var18 = 0;
				Read<int>("unknown", handler, unknown_var18, "\t\t");

				int count_var19 = 0;
				Read<int>("count", handler, count_var19, "\t\t");

				for (int i = 0; i < count_var19 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					int unknown_var20 = 0;
					Read<int>("unknown", handler, unknown_var20, "\t\t\t");
				}
			}

			if (mode_var0 == 8)
			{
				long long skillSn_var21 = 0;
				Read<long long>("skillSn", handler, skillSn_var21, "\t\t");

				bool hasTarget_var22 = false;
				Read<bool>("hasTarget", handler, hasTarget_var22, "\t\t");

				ValidateValues(stream, "hasTarget", "\t\t", hasTarget_var22, (bool)0, (bool)1);

				if (hasTarget_var22)
				{
					unsigned int skillId_var23 = 0;
					Read<unsigned int>("skillId", handler, skillId_var23, "\t\t\t");

					unsigned short skillLevel_var24 = 0;
					Read<unsigned short>("skillLevel", handler, skillLevel_var24, "\t\t\t");

					int unknown_var25 = 0;
					Read<int>("unknown", handler, unknown_var25, "\t\t\t");

					int npcId_var26 = 0;
					Read<int>("npcId", handler, npcId_var26, "\t\t\t");
				}
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0x56>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::FieldAddNpcPacket output0;

			Read<int>("actorId", handler, output0.ActorId, "\t");
			Read<int>("npcId", handler, output0.NpcId, "\t");
			Read<Vector3S>("position", handler, output0.Position, "\t");
			Read<Vector3S>("rotation", handler, output0.Rotation, "\t");

			bool isBoss_var4 = false;

			if (!handler.PacketStream().HasRecentlyFailed)
			{
				isBoss_var4 = handler.IsNpcBoss(output0.NpcId);
			}

			ValidateValues(stream, "isBoss", "\t", isBoss_var4, (bool)0, (bool)1);

			if (isBoss_var4)
			{
				Read<std::string>("kfmName", handler, output0.KfmName, "\t\t");
			}

			unsigned char defaultStatsMode_var6 = 0;
			Read<unsigned char>("defaultStatsMode", handler, defaultStatsMode_var6, "\t");

			ValidateValues(stream, "defaultStatsMode", "\t", defaultStatsMode_var6, (unsigned char)35);

			if (defaultStatsMode_var6 == 35)
			{
				Read<unsigned long long>("hpMax", handler, output0.Hp.Max, "\t\t");
				Read<unsigned int>("moveSpeedMax", handler, output0.MoveSpeed.Max, "\t\t");
				Read<unsigned long long>("hpBase", handler, output0.Hp.Base, "\t\t");
				Read<unsigned int>("moveSpeedBase", handler, output0.MoveSpeed.Base, "\t\t");
				Read<unsigned long long>("hpCurrent", handler, output0.Hp.Current, "\t\t");
				Read<unsigned int>("moveSpeedCurrent", handler, output0.MoveSpeed.Current, "\t\t");
			}
			Read<bool>("isDead", handler, output0.IsDead, "\t");

			ValidateValues(stream, "isDead", "\t", output0.IsDead, (bool)0, (bool)1);

			unsigned short effectCount_var14 = 0;
			Read<unsigned short>("effectCount", handler, effectCount_var14, "\t");

			ResizeVector(handler, output0.AdditionalEffects, effectCount_var14);

			for (unsigned short i = 0; i < effectCount_var14 && !handler.PacketStream().HasRecentlyFailed; ++i)
			{
				Read<int>("ownerId", handler, output0.AdditionalEffects[i].OwnerId, "\t\t");
				Read<int>("effectInstanceId", handler, output0.AdditionalEffects[i].InstanceId, "\t\t");
				Read<int>("casterId", handler, output0.AdditionalEffects[i].CasterId, "\t\t");
				ParseEffectStats_v12(handler, output0.AdditionalEffects[i].Stats);

				Read<bool>("enabled", handler, output0.AdditionalEffects[i].Enabled, "\t\t");

				ValidateValues(stream, "enabled", "\t\t", output0.AdditionalEffects[i].Enabled, (bool)0, (bool)1);

				Read<long long>("shieldHealth", handler, output0.AdditionalEffects[i].ShieldHealth, "\t\t");
			}
			Read<long long>("petItemInstanceId", handler, output0.PetItemInstanceId, "\t");

			unsigned char unknown_var21 = 0;
			Read<unsigned char>("unknown", handler, unknown_var21, "\t");
			Read<short>("level", handler, output0.Level, "\t");

			short unknownLevelPad_var23 = 0;
			Read<short>("unknownLevelPad", handler, unknownLevelPad_var23, "\t");

			ValidateValues(stream, "unknownLevelPad", "\t", unknownLevelPad_var23, (short)0);

			int unknown_var24 = 0;
			Read<int>("unknown", handler, unknown_var24, "\t");

			if (isBoss_var4)
			{
				Read<std::wstring>("spawnEffect", handler, output0.SpawnEffect, "\t\t");

				int skillCount_var26 = 0;
				Read<int>("skillCount", handler, skillCount_var26, "\t\t");

				ResizeVector(handler, output0.Skills, skillCount_var26);

				for (int i = 0; i < skillCount_var26 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					Read<int>("skillId", handler, output0.Skills[i].SkillId, "\t\t\t");
					Read<short>("skillLevel", handler, output0.Skills[i].SkillLevel, "\t\t\t");
				}

				int unknown_var29 = 0;
				Read<int>("unknown", handler, unknown_var29, "\t\t");
			}

			bool hasHiddenHp_var30 = false;

			if (!handler.PacketStream().HasRecentlyFailed)
			{
				hasHiddenHp_var30 = handler.NpcHasHiddenHp(output0.NpcId);
			}

			ValidateValues(stream, "hasHiddenHp", "\t", hasHiddenHp_var30, (bool)0, (bool)1);

			if (hasHiddenHp_var30)
			{
				Read<long long>("hiddenHpAdd", handler, output0.HiddenHpAdd, "\t\t");
			}

			bool unknownBool_var32 = false;
			Read<bool>("unknownBool", handler, unknownBool_var32, "\t");

			ValidateValues(stream, "unknownBool", "\t", unknownBool_var32, (bool)0, (bool)1);

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::FieldAddNpcPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0x16>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char message_var0 = 0;
			Read<unsigned char>("message", handler, message_var0, "\t");

			ValidateValues(stream, "message", "\t", message_var0, (unsigned char)0);

			if (message_var0 == 0)
			{
				Server::RequestFieldEnterPacket output0;

				Read<int>("mapId", handler, output0.MapId, "\t\t");

				unsigned char unknown_var2 = 0;
				Read<unsigned char>("unknown", handler, unknown_var2, "\t\t");

				unsigned char unknown_var3 = 0;
				Read<unsigned char>("unknown", handler, unknown_var3, "\t\t");

				int unknown_var4 = 0;
				Read<int>("unknown", handler, unknown_var4, "\t\t");

				int unknown_var5 = 0;
				Read<int>("unknown", handler, unknown_var5, "\t\t");
				Read<Vector3S>("position", handler, output0.Position, "\t\t");
				Read<Vector3S>("rotation", handler, output0.Rotation, "\t\t");

				int unknown_var8 = 0;
				Read<int>("unknown", handler, unknown_var8, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::RequestFieldEnterPacket>(output0);

					return;
				}
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0x2c>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::FieldRemoveItemPacket output0;

			Read<int>("objectId", handler, output0.ObjectId, "\t");

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::FieldRemoveItemPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0xc>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4, (unsigned char)5, (unsigned char)6, (unsigned char)7);

			if (mode_var0 == 0)
			{
				Server::CharacterListLoadPacket output0;

				unsigned char count_var1 = 0;
				Read<unsigned char>("count", handler, count_var1, "\t\t");

				ResizeVector(handler, output0.Characters, count_var1);

				for (unsigned char i = 0; i < count_var1 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					ParseCharacterListEntry_v12(handler, output0.Characters[i]);
				}

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListLoadPacket>(output0);

					return;
				}
			}

			if (mode_var0 == 1)
			{
				Server::CharacterListAddPacket output1;
				ParseCharacterListEntry_v12(handler, output1.Character);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListAddPacket>(output1);

					return;
				}
			}

			if (mode_var0 == 2)
			{
				Server::CharacterListDeletePacket output2;

				int unknown_var2 = 0;
				Read<int>("unknown", handler, unknown_var2, "\t\t");
				Read<long long>("characterId", handler, output2.CharacterId, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListDeletePacket>(output2);

					return;
				}
			}

			if (mode_var0 == 3)
			{
			}
			if (mode_var0 == 4)
			{
				unsigned char unknown_var4 = 0;
				Read<unsigned char>("unknown", handler, unknown_var4, "\t\t");
			}

			if (mode_var0 == 5)
			{
				Server::CharacterListDeletePendingPacket output3;

				Read<long long>("characterId", handler, output3.CharacterId, "\t\t");
				Read<int>("messageId", handler, output3.MessageId, "\t\t");
				Read<long long>("deleteTime", handler, output3.DeleteTime, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListDeletePendingPacket>(output3);

					return;
				}
			}

			if (mode_var0 == 6)
			{
				Server::CharacterListCancelDeletePacket output4;

				Read<long long>("characterId", handler, output4.CharacterId, "\t\t");
				Read<int>("messageId", handler, output4.MessageId, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListCancelDeletePacket>(output4);

					return;
				}
			}

			if (mode_var0 == 7)
			{
				Server::CharacterListChangeNamePacket output5;

				Read<int>("confirm", handler, output5.Confirm, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListChangeNamePacket>(output5);

					return;
				}
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0x18>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::FieldRemovePlayerPacket output0;

			Read<int>("characterId", handler, output0.CharacterId, "\t");

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::FieldRemovePlayerPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0x1d>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::UserChatPacket output0;

			Read<long long>("accountId", handler, output0.AccountId, "\t");
			Read<long long>("characterId", handler, output0.CharacterId, "\t");
			Read<std::wstring>("characterName", handler, output0.CharacterName, "\t");

			unsigned char unknown_var3 = 0;
			Read<unsigned char>("unknown", handler, unknown_var3, "\t");
			Read<std::wstring>("message", handler, output0.Message, "\t");
			Read<int>("type", handler, output0.Type, "\t");

			ValidateValues(stream, "type", "\t", output0.Type, (int)8, (int)0, (int)3, (int)4, (int)5, (int)6, (int)7, (int)9, (int)11, (int)12, (int)13, (int)14, (int)15, (int)16, (int)18, (int)19, (int)20, (int)23);

			unsigned char unknown_var6 = 0;
			Read<unsigned char>("unknown", handler, unknown_var6, "\t");
			Read<int>("channelId", handler, output0.ChannelId, "\t");

			if (output0.Type == 3)
			{
				std::wstring unknown_var8;
				Read<std::wstring>("unknown", handler, unknown_var8, "\t\t");
			}

			if (output0.Type == 16)
			{
				Read<int>("superChatId", handler, output0.SuperChatId, "\t\t");
			}

			if (output0.Type == 20)
			{
				Read<long long>("clubId", handler, output0.ClubId, "\t\t");
			}

			unsigned char unknown_var11 = 0;
			Read<unsigned char>("unknown", handler, unknown_var11, "\t");

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::UserChatPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0x2b>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::FieldAddItemPacket output0;

			Read<int>("objectId", handler, output0.ObjectId, "\t");
			Read<int>("itemId", handler, output0.ItemId, "\t");
			Read<int>("amount", handler, output0.Amount, "\t");

			bool hasUid_var3 = false;
			Read<bool>("hasUid", handler, hasUid_var3, "\t");

			ValidateValues(stream, "hasUid", "\t", hasUid_var3, (bool)0, (bool)1);

			if (hasUid_var3)
			{
				Read<long long>("uid", handler, output0.ItemInstanceId, "\t\t");
			}
			Read<Vector3S>("position", handler, output0.Position, "\t");
			Read<int>("ownerId", handler, output0.OwnerId, "\t");

			int unknown_var7 = 0;
			Read<int>("unknown", handler, unknown_var7, "\t");

			unsigned char unknown_var8 = 0;
			Read<unsigned char>("unknown", handler, unknown_var8, "\t");
			Read<int>("rarity", handler, output0.Rarity, "\t");

			ValidateValues(stream, "rarity", "\t", output0.Rarity, (int)0, (int)1, (int)2, (int)3, (int)4, (int)5, (int)6);

			short unknown_var10 = 0;
			Read<short>("unknown", handler, unknown_var10, "\t");

			bool unknown_var11 = false;
			Read<bool>("unknown", handler, unknown_var11, "\t");

			ValidateValues(stream, "unknown", "\t", unknown_var11, (bool)0, (bool)1);

			bool unknown_var12 = false;
			Read<bool>("unknown", handler, unknown_var12, "\t");

			ValidateValues(stream, "unknown", "\t", unknown_var12, (bool)0, (bool)1);

			if (output0.ItemId < 90000001 || output0.ItemId > 90000003)
			{
				ParseItemData_v12(handler, output0.ItemData, (unsigned int)output0.ItemId);
			}

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::FieldAddItemPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0x48>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::BuffPacket output0;

			Read<unsigned char>("mode", handler, output0.Mode, "\t");

			ValidateValues(stream, "mode", "\t", output0.Mode, (unsigned char)0, (unsigned char)1, (unsigned char)2);

			Read<int>("targetId", handler, output0.TargetId, "\t");
			Read<int>("instanceId", handler, output0.InstanceId, "\t");
			Read<int>("sourceId", handler, output0.SourceId, "\t");

			if (output0.InstanceId != 0)
			{
				if (output0.Mode == 0)
				{
					ParseEffectStats_v12(handler, output0.Stats);

					Read<bool>("hasShield", handler, output0.UpdateShield, "\t\t\t");

					ValidateValues(stream, "hasShield", "\t\t\t", output0.UpdateShield, (bool)0, (bool)1);

					Read<long long>("shieldHealth", handler, output0.ShieldHealth, "\t\t\t");
				}

				if (output0.Mode == 2)
				{
					int flags_var6 = 0;
					Read<int>("flags", handler, flags_var6, "\t\t\t");

					output0.UpdateEffect = GetBit(flags_var6, 0);
					output0.UpdateShield = GetBit(flags_var6, 1);

					if (GetBit(flags_var6, 0) == 1)
					{
						ParseEffectStats_v12(handler, output0.Stats);

						Read<bool>("enabled", handler, output0.Enabled, "\t\t\t\t");

						ValidateValues(stream, "enabled", "\t\t\t\t", output0.Enabled, (bool)0, (bool)1);
					}

					if (GetBit(flags_var6, 1) == 1)
					{
						Read<long long>("shieldHealth", handler, output0.ShieldHealth, "\t\t\t\t");
					}
				}
			}

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::BuffPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0x4a>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::JobPacket output0;

			Read<int>("actorId", handler, output0.ActorId, "\t");
			Read<unsigned char>("mode", handler, output0.Mode, "\t");

			ValidateValues(stream, "mode", "\t", output0.Mode, (unsigned char)8, (unsigned char)0, (unsigned char)9, (unsigned char)1, (unsigned char)10, (unsigned char)2, (unsigned char)3, (unsigned char)7, (unsigned char)11);
			ParseSkillTreeData_v12(handler, output0.SkillTree);

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::JobPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0x57>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::FieldRemoveNpcPacket output0;

			Read<int>("actorId", handler, output0.ActorId, "\t");

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::FieldRemoveNpcPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0x5f>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)4);

			if (mode_var0 == 0)
			{
				Server::TrophyStartPacket output0;

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::TrophyStartPacket>(output0);

					return;
				}
			}

			if (mode_var0 == 1)
			{
				Server::TrophyContentPacket output1;

				int count_var1 = 0;
				Read<int>("count", handler, count_var1, "\t\t");

				ResizeVector(handler, output1.Trophies, count_var1);

				for (int i = 0; i < count_var1 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					Read<int>("trophyId", handler, output1.Trophies[i].TrophyId, "\t\t\t");

					int unknown_var3 = 0;
					Read<int>("unknown", handler, unknown_var3, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var3, (int)1, (int)2, (int)3, (int)4, (int)5, (int)6, (int)7, (int)8, (int)9);

					Read<unsigned char>("status", handler, output1.Trophies[i].Status, "\t\t\t");

					ValidateValues(stream, "status", "\t\t\t", output1.Trophies[i].Status, (unsigned char)2, (unsigned char)3);

					Read<int>("isDone", handler, output1.Trophies[i].IsDone, "\t\t\t");
					Read<int>("nextGrade", handler, output1.Trophies[i].NextGrade, "\t\t\t");
					Read<int>("lastReward", handler, output1.Trophies[i].LastReward, "\t\t\t");
					Read<bool>("isFavorited", handler, output1.Trophies[i].IsFavorited, "\t\t\t");

					ValidateValues(stream, "isFavorited", "\t\t\t", output1.Trophies[i].IsFavorited, (bool)0, (bool)1);

					Read<long long>("counter", handler, output1.Trophies[i].Counter, "\t\t\t");

					int timestampsCount_var10 = 0;
					Read<int>("timestampsCount", handler, timestampsCount_var10, "\t\t\t");

					ResizeVector(handler, output1.Trophies[i].Timestamps, timestampsCount_var10);

					for (int j = 0; j < timestampsCount_var10 && !handler.PacketStream().HasRecentlyFailed; ++j)
					{
						Read<int>("index", handler, output1.Trophies[i].Timestamps[j].Index, "\t\t\t\t");
						Read<long long>("timestamp", handler, output1.Trophies[i].Timestamps[j].Time, "\t\t\t\t");
					}
				}

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::TrophyContentPacket>(output1);

					return;
				}
			}

			if (mode_var0 == 2)
			{
				Server::TrophyUpdatePacket output2;

				Read<int>("trophyId", handler, output2.TrophyId, "\t\t");
				Read<unsigned char>("status", handler, output2.Status, "\t\t");

				ValidateValues(stream, "status", "\t\t", output2.Status, (unsigned char)2, (unsigned char)3);

				Read<int>("isDone", handler, output2.IsDone, "\t\t");
				Read<int>("nextGrade", handler, output2.NextGrade, "\t\t");
				Read<int>("lastReward", handler, output2.LastReward, "\t\t");
				Read<bool>("isFavorited", handler, output2.IsFavorited, "\t\t");

				ValidateValues(stream, "isFavorited", "\t\t", output2.IsFavorited, (bool)0, (bool)1);

				Read<long long>("counter", handler, output2.Counter, "\t\t");

				int timestampsCount_var20 = 0;
				Read<int>("timestampsCount", handler, timestampsCount_var20, "\t\t");

				ResizeVector(handler, output2.Timestamps, timestampsCount_var20);

				for (int i = 0; i < timestampsCount_var20 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					Read<int>("index", handler, output2.Timestamps[i].Index, "\t\t\t");
					Read<long long>("timestamp", handler, output2.Timestamps[i].Time, "\t\t\t");
				}

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::TrophyUpdatePacket>(output2);

					return;
				}
			}
		}

		template <>
		void ParsePacket<12, ServerPacket, 0x7a>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::CharacterInfoPacket output0;

			Read<long long>("characterId", handler, output0.CharacterId, "\t");
			Read<bool>("found", handler, output0.Found, "\t");

			ValidateValues(stream, "found", "\t", output0.Found, (bool)0, (bool)1);

			if (output0.Found)
			{
				long long notAccountId_var2 = 0;
				Read<long long>("notAccountId", handler, notAccountId_var2, "\t\t");

				long long characterId2_var3 = 0;
				Read<long long>("characterId2", handler, characterId2_var3, "\t\t");
				Read<long long>("currentTime", handler, output0.CurrentTime, "\t\t");

				int bufferSize_var5 = 0;
				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer0(handler, (size_t)bufferSize_var5, true);
					ParseCharacterDetailsHeader_v12(handler, output0.Details);

					{
						ParseCharacterDetailsBasicStats_v12(handler, output0.Details.Stats);
						ParseCharacterDetailsSpecialStats_v12(handler, output0.Details.Stats);
					}
					ParseCharacterDetailsBody_v12(handler, output0.Details);
				}

				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer1(handler, (size_t)bufferSize_var5, true);
					ParseCharacterEquipmentItems_v12(handler, output0.CharacterEquipment);

					unsigned char unknown_var6 = 0;
					Read<unsigned char>("unknown", handler, unknown_var6, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var6, (unsigned char)0);

					long long unknown_var7 = 0;
					Read<long long>("unknown", handler, unknown_var7, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var7, (long long)0);

					long long unknown_var8 = 0;
					Read<long long>("unknown", handler, unknown_var8, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var8, (long long)0);

					unsigned char unknown_var9 = 0;
					Read<unsigned char>("unknown", handler, unknown_var9, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var9, (unsigned char)0);
				}

				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer2(handler, (size_t)bufferSize_var5, true);
					ParseCharacterEquipmentBadges_v12(handler, output0.CharacterEquipment);
				}
			}

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::CharacterInfoPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<2502, ServerPacket, 0x49>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::JobPacket output0;

			Read<int>("actorId", handler, output0.ActorId, "\t");
			Read<unsigned char>("mode", handler, output0.Mode, "\t");

			ValidateValues(stream, "mode", "\t", output0.Mode, (unsigned char)8, (unsigned char)0, (unsigned char)9, (unsigned char)1, (unsigned char)10, (unsigned char)2, (unsigned char)3, (unsigned char)7, (unsigned char)11);
			ParseSkillTreeData_v2502(handler, output0.SkillTree);

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::JobPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<2486, ServerPacket, 0xc>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4, (unsigned char)5, (unsigned char)6, (unsigned char)7);

			if (mode_var0 == 0)
			{
				Server::CharacterListLoadPacket output0;

				unsigned char count_var1 = 0;
				Read<unsigned char>("count", handler, count_var1, "\t\t");

				ResizeVector(handler, output0.Characters, count_var1);

				for (unsigned char i = 0; i < count_var1 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					ParseCharacterListEntry_v2486(handler, output0.Characters[i]);
				}

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListLoadPacket>(output0);

					return;
				}
			}

			if (mode_var0 == 1)
			{
				Server::CharacterListAddPacket output1;
				ParseCharacterListEntry_v2486(handler, output1.Character);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListAddPacket>(output1);

					return;
				}
			}

			if (mode_var0 == 2)
			{
				Server::CharacterListDeletePacket output2;

				int unknown_var2 = 0;
				Read<int>("unknown", handler, unknown_var2, "\t\t");
				Read<long long>("characterId", handler, output2.CharacterId, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListDeletePacket>(output2);

					return;
				}
			}

			if (mode_var0 == 3)
			{
			}
			if (mode_var0 == 4)
			{
				unsigned char unknown_var4 = 0;
				Read<unsigned char>("unknown", handler, unknown_var4, "\t\t");
			}

			if (mode_var0 == 5)
			{
				Server::CharacterListDeletePendingPacket output3;

				Read<long long>("characterId", handler, output3.CharacterId, "\t\t");
				Read<int>("messageId", handler, output3.MessageId, "\t\t");
				Read<long long>("deleteTime", handler, output3.DeleteTime, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListDeletePendingPacket>(output3);

					return;
				}
			}

			if (mode_var0 == 6)
			{
				Server::CharacterListCancelDeletePacket output4;

				Read<long long>("characterId", handler, output4.CharacterId, "\t\t");
				Read<int>("messageId", handler, output4.MessageId, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListCancelDeletePacket>(output4);

					return;
				}
			}

			if (mode_var0 == 7)
			{
				Server::CharacterListChangeNamePacket output5;

				Read<int>("confirm", handler, output5.Confirm, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListChangeNamePacket>(output5);

					return;
				}
			}
		}

		template <>
		void ParsePacket<2486, ServerPacket, 0x2b>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::FieldAddItemPacket output0;

			Read<int>("objectId", handler, output0.ObjectId, "\t");
			Read<int>("itemId", handler, output0.ItemId, "\t");
			Read<int>("amount", handler, output0.Amount, "\t");
			Read<long long>("uid", handler, output0.ItemInstanceId, "\t");
			Read<Vector3S>("position", handler, output0.Position, "\t");
			Read<int>("ownerId", handler, output0.OwnerId, "\t");

			unsigned char unknown_var6 = 0;
			Read<unsigned char>("unknown", handler, unknown_var6, "\t");
			Read<int>("rarity", handler, output0.Rarity, "\t");

			ValidateValues(stream, "rarity", "\t", output0.Rarity, (int)0, (int)1, (int)2, (int)3, (int)4, (int)5, (int)6);

			short unknown_var8 = 0;
			Read<short>("unknown", handler, unknown_var8, "\t");

			bool unknown_var9 = false;
			Read<bool>("unknown", handler, unknown_var9, "\t");

			ValidateValues(stream, "unknown", "\t", unknown_var9, (bool)0, (bool)1);

			bool unknown_var10 = false;
			Read<bool>("unknown", handler, unknown_var10, "\t");

			ValidateValues(stream, "unknown", "\t", unknown_var10, (bool)0, (bool)1);

			if (output0.ItemId < 90000001 || output0.ItemId > 90000003)
			{
				ParseItemData_v2486(handler, output0.ItemData, (unsigned int)output0.ItemId);
			}

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::FieldAddItemPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<2486, ServerPacket, 0x3d>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)8, (unsigned char)0, (unsigned char)1, (unsigned char)3, (unsigned char)4, (unsigned char)5, (unsigned char)6, (unsigned char)7);

			if (mode_var0 == 0)
			{
				Server::SkillDamageSyncPacket output0;

				Read<long long>("skillSn", handler, output0.SkillSn, "\t\t");
				Read<int>("casterId", handler, output0.CasterId, "\t\t");
				Read<unsigned int>("skillId", handler, output0.SkillId, "\t\t");

				if (output0.SkillId == 0)
				{
					unsigned char unknown_var4 = 0;
					Read<unsigned char>("unknown", handler, unknown_var4, "\t\t\t");
				}

				if (output0.SkillId != 0)
				{
					Read<unsigned short>("skillLevel", handler, output0.SkillLevel, "\t\t\t");
					Read<unsigned char>("motionPoint", handler, output0.MotionPoint, "\t\t\t");
					Read<unsigned char>("attackPoint", handler, output0.AttackPoint, "\t\t\t");
					Read<Vector3Short>("position", handler, output0.Position, "\t\t\t");
					Read<Vector3S>("direction", handler, output0.Direction, "\t\t\t");
					Read<bool>("isChaining", handler, output0.IsChaining, "\t\t\t");

					ValidateValues(stream, "isChaining", "\t\t\t", output0.IsChaining, (bool)0, (bool)1);

					unsigned char count_var11 = 0;
					Read<unsigned char>("count", handler, count_var11, "\t\t\t");

					ResizeVector(handler, output0.Hits, count_var11);

					for (unsigned char i = 0; i < count_var11 && !handler.PacketStream().HasRecentlyFailed; ++i)
					{
						long long unknown_var12 = 0;
						Read<long long>("unknown", handler, unknown_var12, "\t\t\t\t");
						Read<long long>("skillAttack", handler, output0.Hits[i].SkillAttack, "\t\t\t\t");
						Read<int>("targetId", handler, output0.Hits[i].TargetId, "\t\t\t\t");
						Read<short>("animation", handler, output0.Hits[i].Animation, "\t\t\t\t");
					}
				}

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageSyncPacket>(output0);

					return;
				}
			}

			if (mode_var0 == 1)
			{
				Server::SkillDamagePacket output1;
				ParseSkillDamagePacket_v12(handler, output1);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamagePacket>(output1);

					return;
				}
			}

			if (mode_var0 == 3)
			{
				Server::SkillDamageDotPacket output2;
				ParseSkillDamageDotPacket_v12(handler, output2);

				Read<int>("damage", handler, output2.Damage, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageDotPacket>(output2);

					return;
				}
			}

			if (mode_var0 == 4)
			{
				Server::SkillDamageHealPacket output3;
				ParseSkillDamageHealPacket_v12(handler, output3);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageHealPacket>(output3);

					return;
				}
			}

			if (mode_var0 == 5)
			{
				Server::SkillDamageRegionPacket output4;
				ParseSkillDamageRegionPacket_v12(handler, output4);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageRegionPacket>(output4);

					return;
				}
			}

			if (mode_var0 == 6)
			{
				Server::SkillDamageTilePacket output5;
				ParseSkillDamageTilePacket_v12(handler, output5);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageTilePacket>(output5);

					return;
				}
			}

			if (mode_var0 == 7)
			{
				int unknown_var17 = 0;
				Read<int>("unknown", handler, unknown_var17, "\t\t");

				int count_var18 = 0;
				Read<int>("count", handler, count_var18, "\t\t");

				for (int i = 0; i < count_var18 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					int unknown_var19 = 0;
					Read<int>("unknown", handler, unknown_var19, "\t\t\t");
				}
			}

			if (mode_var0 == 8)
			{
				long long skillSn_var20 = 0;
				Read<long long>("skillSn", handler, skillSn_var20, "\t\t");

				bool hasTarget_var21 = false;
				Read<bool>("hasTarget", handler, hasTarget_var21, "\t\t");

				ValidateValues(stream, "hasTarget", "\t\t", hasTarget_var21, (bool)0, (bool)1);

				if (hasTarget_var21)
				{
					unsigned int skillId_var22 = 0;
					Read<unsigned int>("skillId", handler, skillId_var22, "\t\t\t");

					unsigned short skillLevel_var23 = 0;
					Read<unsigned short>("skillLevel", handler, skillLevel_var23, "\t\t\t");

					int unknown_var24 = 0;
					Read<int>("unknown", handler, unknown_var24, "\t\t\t");

					int npcId_var25 = 0;
					Read<int>("npcId", handler, npcId_var25, "\t\t\t");
				}
			}
		}

		template <>
		void ParsePacket<2486, ServerPacket, 0x78>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::CharacterInfoPacket output0;

			Read<long long>("characterId", handler, output0.CharacterId, "\t");
			Read<bool>("found", handler, output0.Found, "\t");

			ValidateValues(stream, "found", "\t", output0.Found, (bool)0, (bool)1);

			if (output0.Found)
			{
				long long notAccountId_var2 = 0;
				Read<long long>("notAccountId", handler, notAccountId_var2, "\t\t");

				long long characterId2_var3 = 0;
				Read<long long>("characterId2", handler, characterId2_var3, "\t\t");
				Read<long long>("currentTime", handler, output0.CurrentTime, "\t\t");

				int bufferSize_var5 = 0;
				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer0(handler, (size_t)bufferSize_var5, true);
					ParseCharacterDetailsHeader_v12(handler, output0.Details);

					{
						ParseCharacterDetailsBasicStats_v12(handler, output0.Details.Stats);
						ParseCharacterDetailsSpecialStats_v12(handler, output0.Details.Stats);
					}
					ParseCharacterDetailsBody_v12(handler, output0.Details);
				}

				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer1(handler, (size_t)bufferSize_var5, true);
					ParseCharacterEquipmentItems_v2486(handler, output0.CharacterEquipment);

					unsigned char unknown_var6 = 0;
					Read<unsigned char>("unknown", handler, unknown_var6, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var6, (unsigned char)0);

					long long unknown_var7 = 0;
					Read<long long>("unknown", handler, unknown_var7, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var7, (long long)0);

					long long unknown_var8 = 0;
					Read<long long>("unknown", handler, unknown_var8, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var8, (long long)0);

					unsigned char unknown_var9 = 0;
					Read<unsigned char>("unknown", handler, unknown_var9, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var9, (unsigned char)0);
				}

				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer2(handler, (size_t)bufferSize_var5, true);
					ParseCharacterEquipmentBadges_v2486(handler, output0.CharacterEquipment);
				}
			}

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::CharacterInfoPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<13, ClientPacket, 0x3>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)0, (unsigned char)1, (unsigned char)2);

			std::wstring auth1_var1;
			Read<std::wstring>("auth1", handler, auth1_var1, "\t");

			std::wstring auth2_var2;
			Read<std::wstring>("auth2", handler, auth2_var2, "\t");

			unsigned long long unknown_var3 = 0;
			Read<unsigned long long>("unknown", handler, unknown_var3, "\t");

			unsigned long long unknown_var4 = 0;
			Read<unsigned long long>("unknown", handler, unknown_var4, "\t");

			int unknown_var5 = 0;
			Read<int>("unknown", handler, unknown_var5, "\t");

			short unknown_var6 = 0;
			Read<short>("unknown", handler, unknown_var6, "\t");

			int unknown_var7 = 0;
			Read<int>("unknown", handler, unknown_var7, "\t");

			int unknown_var8 = 0;
			Read<int>("unknown", handler, unknown_var8, "\t");
		}

		template <>
		void ParsePacket<13, ServerPacket, 0x47>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::BuffPacket output0;

			Read<unsigned char>("mode", handler, output0.Mode, "\t");

			ValidateValues(stream, "mode", "\t", output0.Mode, (unsigned char)0, (unsigned char)1, (unsigned char)2);

			Read<int>("targetId", handler, output0.TargetId, "\t");
			Read<int>("instanceId", handler, output0.InstanceId, "\t");
			Read<int>("sourceId", handler, output0.SourceId, "\t");

			if (output0.Mode == 0)
			{
				ParseEffectStats_v12(handler, output0.Stats);

				Read<bool>("hasShield", handler, output0.UpdateShield, "\t\t");

				ValidateValues(stream, "hasShield", "\t\t", output0.UpdateShield, (bool)0, (bool)1);

				Read<long long>("shieldHealth", handler, output0.ShieldHealth, "\t\t");
			}

			if (output0.Mode == 2)
			{
				int flags_var6 = 0;
				Read<int>("flags", handler, flags_var6, "\t\t");

				output0.UpdateEffect = GetBit(flags_var6, 0);
				output0.UpdateShield = GetBit(flags_var6, 1);

				if (GetBit(flags_var6, 0) == 1)
				{
					ParseEffectStats_v12(handler, output0.Stats);

					Read<bool>("enabled", handler, output0.Enabled, "\t\t\t");

					ValidateValues(stream, "enabled", "\t\t\t", output0.Enabled, (bool)0, (bool)1);
				}

				if (GetBit(flags_var6, 1) == 1)
				{
					Read<long long>("shieldHealth", handler, output0.ShieldHealth, "\t\t\t");
				}
			}

			if (output0.InstanceId == 0)
			{
				int unknown_var9 = 0;
				Read<int>("unknown", handler, unknown_var9, "\t\t");
			}

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::BuffPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<2497, ServerPacket, 0xc>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4, (unsigned char)5, (unsigned char)6, (unsigned char)7);

			if (mode_var0 == 0)
			{
				Server::CharacterListLoadPacket output0;

				unsigned char count_var1 = 0;
				Read<unsigned char>("count", handler, count_var1, "\t\t");

				ResizeVector(handler, output0.Characters, count_var1);

				for (unsigned char i = 0; i < count_var1 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					ParseCharacterListEntry_v2497(handler, output0.Characters[i]);
				}

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListLoadPacket>(output0);

					return;
				}
			}

			if (mode_var0 == 1)
			{
				Server::CharacterListAddPacket output1;
				ParseCharacterListEntry_v2497(handler, output1.Character);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListAddPacket>(output1);

					return;
				}
			}

			if (mode_var0 == 2)
			{
				Server::CharacterListDeletePacket output2;

				int unknown_var2 = 0;
				Read<int>("unknown", handler, unknown_var2, "\t\t");
				Read<long long>("characterId", handler, output2.CharacterId, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListDeletePacket>(output2);

					return;
				}
			}

			if (mode_var0 == 3)
			{
			}
			if (mode_var0 == 4)
			{
				unsigned char unknown_var4 = 0;
				Read<unsigned char>("unknown", handler, unknown_var4, "\t\t");
			}

			if (mode_var0 == 5)
			{
				Server::CharacterListDeletePendingPacket output3;

				Read<long long>("characterId", handler, output3.CharacterId, "\t\t");
				Read<int>("messageId", handler, output3.MessageId, "\t\t");
				Read<long long>("deleteTime", handler, output3.DeleteTime, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListDeletePendingPacket>(output3);

					return;
				}
			}

			if (mode_var0 == 6)
			{
				Server::CharacterListCancelDeletePacket output4;

				Read<long long>("characterId", handler, output4.CharacterId, "\t\t");
				Read<int>("messageId", handler, output4.MessageId, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListCancelDeletePacket>(output4);

					return;
				}
			}

			if (mode_var0 == 7)
			{
				Server::CharacterListChangeNamePacket output5;

				Read<int>("confirm", handler, output5.Confirm, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListChangeNamePacket>(output5);

					return;
				}
			}
		}

		template <>
		void ParsePacket<2497, ServerPacket, 0x2b>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::FieldAddItemPacket output0;

			Read<int>("objectId", handler, output0.ObjectId, "\t");
			Read<int>("itemId", handler, output0.ItemId, "\t");
			Read<int>("amount", handler, output0.Amount, "\t");
			Read<long long>("uid", handler, output0.ItemInstanceId, "\t");
			Read<Vector3S>("position", handler, output0.Position, "\t");
			Read<int>("ownerId", handler, output0.OwnerId, "\t");

			unsigned char unknown_var6 = 0;
			Read<unsigned char>("unknown", handler, unknown_var6, "\t");
			Read<int>("rarity", handler, output0.Rarity, "\t");

			ValidateValues(stream, "rarity", "\t", output0.Rarity, (int)0, (int)1, (int)2, (int)3, (int)4, (int)5, (int)6);

			short unknown_var8 = 0;
			Read<short>("unknown", handler, unknown_var8, "\t");

			bool unknown_var9 = false;
			Read<bool>("unknown", handler, unknown_var9, "\t");

			ValidateValues(stream, "unknown", "\t", unknown_var9, (bool)0, (bool)1);

			bool unknown_var10 = false;
			Read<bool>("unknown", handler, unknown_var10, "\t");

			ValidateValues(stream, "unknown", "\t", unknown_var10, (bool)0, (bool)1);

			if (output0.ItemId < 90000001 || output0.ItemId > 90000003)
			{
				ParseItemData_v2497(handler, output0.ItemData, (unsigned int)output0.ItemId);
			}

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::FieldAddItemPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<2497, ServerPacket, 0x78>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::CharacterInfoPacket output0;

			Read<long long>("characterId", handler, output0.CharacterId, "\t");
			Read<bool>("found", handler, output0.Found, "\t");

			ValidateValues(stream, "found", "\t", output0.Found, (bool)0, (bool)1);

			if (output0.Found)
			{
				long long notAccountId_var2 = 0;
				Read<long long>("notAccountId", handler, notAccountId_var2, "\t\t");

				long long characterId2_var3 = 0;
				Read<long long>("characterId2", handler, characterId2_var3, "\t\t");
				Read<long long>("currentTime", handler, output0.CurrentTime, "\t\t");

				int bufferSize_var5 = 0;
				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer0(handler, (size_t)bufferSize_var5, true);
					ParseCharacterDetailsHeader_v12(handler, output0.Details);

					{
						ParseCharacterDetailsBasicStats_v12(handler, output0.Details.Stats);
						ParseCharacterDetailsSpecialStats_v2497(handler, output0.Details.Stats);
					}
					ParseCharacterDetailsBody_v12(handler, output0.Details);

					long long unknown_var6 = 0;
					Read<long long>("unknown", handler, unknown_var6, "\t\t\t");
				}

				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer1(handler, (size_t)bufferSize_var5, true);
					ParseCharacterEquipmentItems_v2497(handler, output0.CharacterEquipment);
					ParseCharacterEquipmentSkins_v2497(handler, output0.CharacterEquipment);
				}

				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer2(handler, (size_t)bufferSize_var5, true);
					ParseCharacterEquipmentBadges_v2497(handler, output0.CharacterEquipment);
				}
			}

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::CharacterInfoPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<2496, ServerPacket, 0xc>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4, (unsigned char)5, (unsigned char)6, (unsigned char)7);

			if (mode_var0 == 0)
			{
				Server::CharacterListLoadPacket output0;

				unsigned char count_var1 = 0;
				Read<unsigned char>("count", handler, count_var1, "\t\t");

				ResizeVector(handler, output0.Characters, count_var1);

				for (unsigned char i = 0; i < count_var1 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					ParseCharacterListEntry_v2486(handler, output0.Characters[i]);
				}

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListLoadPacket>(output0);

					return;
				}
			}

			if (mode_var0 == 1)
			{
				Server::CharacterListAddPacket output1;
				ParseCharacterListEntry_v2486(handler, output1.Character);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListAddPacket>(output1);

					return;
				}
			}

			if (mode_var0 == 2)
			{
				Server::CharacterListDeletePacket output2;

				int unknown_var2 = 0;
				Read<int>("unknown", handler, unknown_var2, "\t\t");
				Read<long long>("characterId", handler, output2.CharacterId, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListDeletePacket>(output2);

					return;
				}
			}

			if (mode_var0 == 3)
			{
			}
			if (mode_var0 == 4)
			{
				unsigned char unknown_var4 = 0;
				Read<unsigned char>("unknown", handler, unknown_var4, "\t\t");
			}

			if (mode_var0 == 5)
			{
				Server::CharacterListDeletePendingPacket output3;

				Read<long long>("characterId", handler, output3.CharacterId, "\t\t");
				Read<int>("messageId", handler, output3.MessageId, "\t\t");
				Read<long long>("deleteTime", handler, output3.DeleteTime, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListDeletePendingPacket>(output3);

					return;
				}
			}

			if (mode_var0 == 6)
			{
				Server::CharacterListCancelDeletePacket output4;

				Read<long long>("characterId", handler, output4.CharacterId, "\t\t");
				Read<int>("messageId", handler, output4.MessageId, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListCancelDeletePacket>(output4);

					return;
				}
			}

			if (mode_var0 == 7)
			{
				Server::CharacterListChangeNamePacket output5;

				Read<int>("confirm", handler, output5.Confirm, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListChangeNamePacket>(output5);

					return;
				}
			}
		}

		template <>
		void ParsePacket<2496, ServerPacket, 0x78>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::CharacterInfoPacket output0;

			Read<long long>("characterId", handler, output0.CharacterId, "\t");
			Read<bool>("found", handler, output0.Found, "\t");

			ValidateValues(stream, "found", "\t", output0.Found, (bool)0, (bool)1);

			if (output0.Found)
			{
				long long notAccountId_var2 = 0;
				Read<long long>("notAccountId", handler, notAccountId_var2, "\t\t");

				long long characterId2_var3 = 0;
				Read<long long>("characterId2", handler, characterId2_var3, "\t\t");
				Read<long long>("currentTime", handler, output0.CurrentTime, "\t\t");

				int bufferSize_var5 = 0;
				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer0(handler, (size_t)bufferSize_var5, true);
					ParseCharacterDetailsHeader_v12(handler, output0.Details);

					{
						ParseCharacterDetailsBasicStats_v12(handler, output0.Details.Stats);
						ParseCharacterDetailsSpecialStats_v12(handler, output0.Details.Stats);
					}
					ParseCharacterDetailsBody_v12(handler, output0.Details);

					long long unknown_var6 = 0;
					Read<long long>("unknown", handler, unknown_var6, "\t\t\t");
				}

				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer1(handler, (size_t)bufferSize_var5, true);
					ParseCharacterEquipmentItems_v2486(handler, output0.CharacterEquipment);
					ParseCharacterEquipmentSkins_v2486(handler, output0.CharacterEquipment);
				}

				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer2(handler, (size_t)bufferSize_var5, true);
					ParseCharacterEquipmentBadges_v2486(handler, output0.CharacterEquipment);
				}
			}

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::CharacterInfoPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<2503, ClientPacket, 0x20>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4);

			if (mode_var0 == 0)
			{
				Client::SkillCastPacket output0;

				Read<long long>("skillSn", handler, output0.SkillSn, "\t\t");
				Read<int>("serverTick", handler, output0.ServerTick, "\t\t");
				Read<int>("skillId", handler, output0.SkillId, "\t\t");
				Read<short>("skillLevel", handler, output0.SkillLevel, "\t\t");
				Read<unsigned char>("attackPoint", handler, output0.AttackPoint, "\t\t");
				Read<Vector3S>("position", handler, output0.Position, "\t\t");
				Read<Vector3S>("direction", handler, output0.Direction, "\t\t");
				Read<Vector3S>("rotation", handler, output0.Rotation, "\t\t");

				float unknown_var9 = 0;
				Read<float>("unknown", handler, unknown_var9, "\t\t");

				ValidateValues(stream, "unknown", "\t\t", unknown_var9, (float)0);

				Read<int>("clientTick", handler, output0.ClientTick, "\t\t");

				bool unknown_var11 = false;
				Read<bool>("unknown", handler, unknown_var11, "\t\t");

				ValidateValues(stream, "unknown", "\t\t", unknown_var11, (bool)0, (bool)1);

				long long unknown_var12 = 0;
				Read<long long>("unknown", handler, unknown_var12, "\t\t");

				ValidateValues(stream, "unknown", "\t\t", unknown_var12, (long long)0);

				long long unknown_var13 = 0;
				Read<long long>("unknown", handler, unknown_var13, "\t\t");
				Read<bool>("flag", handler, output0.Flag, "\t\t");

				ValidateValues(stream, "flag", "\t\t", output0.Flag, (bool)0, (bool)1);

				if (output0.Flag)
				{
					int unknown_var15 = 0;
					Read<int>("unknown", handler, unknown_var15, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var15, (int)0);

					std::string unknown_var16;
					Read<std::string>("unknown", handler, unknown_var16, "\t\t\t");
				}

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::SkillCastPacket>(output0);

					return;
				}
			}

			if (mode_var0 == 1)
			{
				unsigned char damageMode_var17 = 0;
				Read<unsigned char>("damageMode", handler, damageMode_var17, "\t\t");

				ValidateValues(stream, "damageMode", "\t\t", damageMode_var17, (unsigned char)0, (unsigned char)1, (unsigned char)2);

				if (damageMode_var17 == 0)
				{
					Client::SkillSyncDamagePacket output1;

					Read<long long>("skillSn", handler, output1.SkillSn, "\t\t\t");
					Read<unsigned char>("attackPoint", handler, output1.AttackPoint, "\t\t\t");
					Read<Vector3S>("position", handler, output1.Position, "\t\t\t");
					Read<Vector3S>("rotation", handler, output1.Rotation, "\t\t\t");

					unsigned char count_var22 = 0;
					Read<unsigned char>("count", handler, count_var22, "\t\t\t");

					int unknown_var23 = 0;
					Read<int>("unknown", handler, unknown_var23, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var23, (int)0);

					ResizeVector(handler, output1.Projectiles, count_var22);

					for (unsigned char i = 0; i < count_var22 && !handler.PacketStream().HasRecentlyFailed; ++i)
					{
						auto& projectile_array0 = output1.Projectiles[i];

						Read<long long>("skillAttack", handler, projectile_array0.SkillAttack, "\t\t\t\t");
						Read<int>("targetId", handler, projectile_array0.TargetId, "\t\t\t\t");
						Read<unsigned char>("index", handler, projectile_array0.Index, "\t\t\t\t");

						bool moreTargets_var27 = false;
						Read<bool>("moreTargets", handler, moreTargets_var27, "\t\t\t\t");

						ValidateValues(stream, "moreTargets", "\t\t\t\t", moreTargets_var27, (bool)0, (bool)1);

						size_t j = 0;

						while (moreTargets_var27 && !handler.PacketStream().HasRecentlyFailed)
						{
							projectile_array0.ChainTargets.push_back({});

							auto& nextTarget_array1 = projectile_array0.ChainTargets[j];

							Read<long long>("skillAttack", handler, nextTarget_array1.SkillAttack, "\t\t\t\t\t");
							Read<int>("targetId", handler, nextTarget_array1.TargetId, "\t\t\t\t\t");
							Read<unsigned char>("index", handler, nextTarget_array1.Index, "\t\t\t\t\t");

							unsigned char unknown_var31 = 0;
							Read<unsigned char>("unknown", handler, unknown_var31, "\t\t\t\t\t");

							Read<bool>("moreTargets", handler, moreTargets_var27, "\t\t\t\t\t");

							ValidateValues(stream, "moreTargets", "\t\t\t\t\t", moreTargets_var27, (bool)0, (bool)1);

							++j;
						}
					}

					if (stream.Succeeded())
					{
						handler.PacketParsed<Client::SkillSyncDamagePacket>(output1);

						return;
					}
				}

				if (damageMode_var17 == 1)
				{
					Client::SkillDamagePacket output2;

					Read<long long>("skillSn", handler, output2.SkillSn, "\t\t\t");
					Read<long long>("skillAttack", handler, output2.SkillAttack, "\t\t\t");
					Read<Vector3S>("position", handler, output2.Position, "\t\t\t");
					Read<Vector3S>("hitPoint", handler, output2.HitPoint, "\t\t\t");
					Read<Vector3S>("rotation", handler, output2.Rotation, "\t\t\t");
					Read<unsigned char>("attackPoint", handler, output2.AttackPoint, "\t\t\t");

					unsigned char count_var38 = 0;
					Read<unsigned char>("count", handler, count_var38, "\t\t\t");

					int unknown_var39 = 0;
					Read<int>("unknown", handler, unknown_var39, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var39, (int)0);

					ResizeVector(handler, output2.Hits, count_var38);

					for (unsigned char i = 0; i < count_var38 && !handler.PacketStream().HasRecentlyFailed; ++i)
					{
						auto& hitActor_array2 = output2.Hits[i];

						Read<int>("entityId", handler, hitActor_array2.EntityId, "\t\t\t\t");

						unsigned char unknown_var41 = 0;
						Read<unsigned char>("unknown", handler, unknown_var41, "\t\t\t\t");

						ValidateValues(stream, "unknown", "\t\t\t\t", unknown_var41, (unsigned char)0);
					}

					if (stream.Succeeded())
					{
						handler.PacketParsed<Client::SkillDamagePacket>(output2);

						return;
					}
				}

				if (damageMode_var17 == 2)
				{
					Client::SkillRegionSkillPacket output3;

					Read<long long>("skillSn", handler, output3.SkillSn, "\t\t\t");
					Read<unsigned char>("attackPoint", handler, output3.AttackPoint, "\t\t\t");

					int unknown_var44 = 0;
					Read<int>("unknown", handler, unknown_var44, "\t\t\t");
					Read<int>("attackCounter", handler, output3.AttackCounter, "\t\t\t");
					Read<Vector3S>("position", handler, output3.Position, "\t\t\t");
					Read<Vector3S>("rotation", handler, output3.Rotation, "\t\t\t");

					if (stream.Succeeded())
					{
						handler.PacketParsed<Client::SkillRegionSkillPacket>(output3);

						return;
					}
				}
			}

			if (mode_var0 == 2)
			{
				Client::SkillSyncPacket output4;

				Read<long long>("skillSn", handler, output4.SkillSn, "\t\t");
				Read<int>("skillId", handler, output4.SkillId, "\t\t");
				Read<short>("skillLevel", handler, output4.SkillLevel, "\t\t");
				Read<unsigned char>("motionPoint", handler, output4.MotionPoint, "\t\t");
				Read<Vector3S>("position", handler, output4.Position, "\t\t");

				Vector3S unknown_var53;
				Read<Vector3S>("unknown", handler, unknown_var53, "\t\t");
				Read<Vector3S>("rotation", handler, output4.Rotation, "\t\t");

				Vector3S unknown_var55;
				Read<Vector3S>("unknown", handler, unknown_var55, "\t\t");

				bool unknown_var56 = false;
				Read<bool>("unknown", handler, unknown_var56, "\t\t");

				ValidateValues(stream, "unknown", "\t\t", unknown_var56, (bool)0, (bool)1);

				int unknown_var57 = 0;
				Read<int>("unknown", handler, unknown_var57, "\t\t");

				ValidateValues(stream, "unknown", "\t\t", unknown_var57, (int)0);

				unsigned char unknown_var58 = 0;
				Read<unsigned char>("unknown", handler, unknown_var58, "\t\t");

				ValidateValues(stream, "unknown", "\t\t", unknown_var58, (unsigned char)0);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::SkillSyncPacket>(output4);

					return;
				}
			}

			if (mode_var0 == 3)
			{
				Client::SkillSyncTickPacket output5;

				Read<long long>("skillSn", handler, output5.SkillSn, "\t\t");
				Read<int>("serverTick", handler, output5.ServerTick, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::SkillSyncTickPacket>(output5);

					return;
				}
			}

			if (mode_var0 == 4)
			{
				Client::SkillCancelPacket output6;

				Read<long long>("skillSn", handler, output6.SkillSn, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Client::SkillCancelPacket>(output6);

					return;
				}
			}
		}

		template <>
		void ParsePacket<2503, ServerPacket, 0x3d>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)8, (unsigned char)0, (unsigned char)1, (unsigned char)3, (unsigned char)4, (unsigned char)5, (unsigned char)6, (unsigned char)7);

			if (mode_var0 == 0)
			{
				Server::SkillDamageSyncPacket output0;

				Read<long long>("skillSn", handler, output0.SkillSn, "\t\t");
				Read<int>("casterId", handler, output0.CasterId, "\t\t");
				Read<unsigned int>("skillId", handler, output0.SkillId, "\t\t");

				if (output0.SkillId == 0)
				{
					unsigned char unknown_var4 = 0;
					Read<unsigned char>("unknown", handler, unknown_var4, "\t\t\t");
				}

				if (output0.SkillId != 0)
				{
					Read<unsigned short>("skillLevel", handler, output0.SkillLevel, "\t\t\t");
					Read<unsigned char>("motionPoint", handler, output0.MotionPoint, "\t\t\t");
					Read<unsigned char>("attackPoint", handler, output0.AttackPoint, "\t\t\t");
					Read<Vector3Short>("position", handler, output0.Position, "\t\t\t");
					Read<Vector3S>("direction", handler, output0.Direction, "\t\t\t");
					Read<bool>("isChaining", handler, output0.IsChaining, "\t\t\t");

					ValidateValues(stream, "isChaining", "\t\t\t", output0.IsChaining, (bool)0, (bool)1);

					unsigned char count_var11 = 0;
					Read<unsigned char>("count", handler, count_var11, "\t\t\t");

					ResizeVector(handler, output0.Hits, count_var11);

					for (unsigned char i = 0; i < count_var11 && !handler.PacketStream().HasRecentlyFailed; ++i)
					{
						long long unknown_var12 = 0;
						Read<long long>("unknown", handler, unknown_var12, "\t\t\t\t");
						Read<long long>("skillAttack", handler, output0.Hits[i].SkillAttack, "\t\t\t\t");
						Read<int>("targetId", handler, output0.Hits[i].TargetId, "\t\t\t\t");
						Read<short>("animation", handler, output0.Hits[i].Animation, "\t\t\t\t");
					}
				}

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageSyncPacket>(output0);

					return;
				}
			}

			if (mode_var0 == 1)
			{
				Server::SkillDamagePacket output1;
				ParseSkillDamagePacket_v12(handler, output1);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamagePacket>(output1);

					return;
				}
			}

			if (mode_var0 == 3)
			{
				Server::SkillDamageDotPacket output2;
				ParseSkillDamageDotPacket_v12(handler, output2);

				Read<long long>("damage", handler, output2.Damage, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageDotPacket>(output2);

					return;
				}
			}

			if (mode_var0 == 4)
			{
				Server::SkillDamageHealPacket output3;
				ParseSkillDamageHealPacket_v12(handler, output3);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageHealPacket>(output3);

					return;
				}
			}

			if (mode_var0 == 5)
			{
				Server::SkillDamageRegionPacket output4;
				ParseSkillDamageRegionPacket_v12(handler, output4);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageRegionPacket>(output4);

					return;
				}
			}

			if (mode_var0 == 6)
			{
				Server::SkillDamageTilePacket output5;
				ParseSkillDamageTilePacket_v12(handler, output5);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::SkillDamageTilePacket>(output5);

					return;
				}
			}

			if (mode_var0 == 7)
			{
				int unknown_var17 = 0;
				Read<int>("unknown", handler, unknown_var17, "\t\t");

				int count_var18 = 0;
				Read<int>("count", handler, count_var18, "\t\t");

				for (int i = 0; i < count_var18 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					int unknown_var19 = 0;
					Read<int>("unknown", handler, unknown_var19, "\t\t\t");
				}
			}

			if (mode_var0 == 8)
			{
				long long skillSn_var20 = 0;
				Read<long long>("skillSn", handler, skillSn_var20, "\t\t");

				bool hasTarget_var21 = false;
				Read<bool>("hasTarget", handler, hasTarget_var21, "\t\t");

				ValidateValues(stream, "hasTarget", "\t\t", hasTarget_var21, (bool)0, (bool)1);

				if (hasTarget_var21)
				{
					unsigned int skillId_var22 = 0;
					Read<unsigned int>("skillId", handler, skillId_var22, "\t\t\t");

					unsigned short skillLevel_var23 = 0;
					Read<unsigned short>("skillLevel", handler, skillLevel_var23, "\t\t\t");

					int unknown_var24 = 0;
					Read<int>("unknown", handler, unknown_var24, "\t\t\t");

					int npcId_var25 = 0;
					Read<int>("npcId", handler, npcId_var25, "\t\t\t");
				}
			}
		}

		template <>
		void ParsePacket<2509, ServerPacket, 0xc>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			unsigned char mode_var0 = 0;
			Read<unsigned char>("mode", handler, mode_var0, "\t");

			ValidateValues(stream, "mode", "\t", mode_var0, (unsigned char)0, (unsigned char)1, (unsigned char)2, (unsigned char)3, (unsigned char)4, (unsigned char)5, (unsigned char)6, (unsigned char)7);

			if (mode_var0 == 0)
			{
				Server::CharacterListLoadPacket output0;

				unsigned char count_var1 = 0;
				Read<unsigned char>("count", handler, count_var1, "\t\t");

				ResizeVector(handler, output0.Characters, count_var1);

				for (unsigned char i = 0; i < count_var1 && !handler.PacketStream().HasRecentlyFailed; ++i)
				{
					ParseCharacterListEntry_v2509(handler, output0.Characters[i]);
				}

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListLoadPacket>(output0);

					return;
				}
			}

			if (mode_var0 == 1)
			{
				Server::CharacterListAddPacket output1;
				ParseCharacterListEntry_v2509(handler, output1.Character);

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListAddPacket>(output1);

					return;
				}
			}

			if (mode_var0 == 2)
			{
				Server::CharacterListDeletePacket output2;

				int unknown_var2 = 0;
				Read<int>("unknown", handler, unknown_var2, "\t\t");
				Read<long long>("characterId", handler, output2.CharacterId, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListDeletePacket>(output2);

					return;
				}
			}

			if (mode_var0 == 3)
			{
			}
			if (mode_var0 == 4)
			{
				unsigned char unknown_var4 = 0;
				Read<unsigned char>("unknown", handler, unknown_var4, "\t\t");
			}

			if (mode_var0 == 5)
			{
				Server::CharacterListDeletePendingPacket output3;

				Read<long long>("characterId", handler, output3.CharacterId, "\t\t");
				Read<int>("messageId", handler, output3.MessageId, "\t\t");
				Read<long long>("deleteTime", handler, output3.DeleteTime, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListDeletePendingPacket>(output3);

					return;
				}
			}

			if (mode_var0 == 6)
			{
				Server::CharacterListCancelDeletePacket output4;

				Read<long long>("characterId", handler, output4.CharacterId, "\t\t");
				Read<int>("messageId", handler, output4.MessageId, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListCancelDeletePacket>(output4);

					return;
				}
			}

			if (mode_var0 == 7)
			{
				Server::CharacterListChangeNamePacket output5;

				Read<int>("confirm", handler, output5.Confirm, "\t\t");
				Read<long long>("characterId", handler, output5.CharacterId, "\t\t");
				Read<std::wstring>("name", handler, output5.Name, "\t\t");

				if (stream.Succeeded())
				{
					handler.PacketParsed<Server::CharacterListChangeNamePacket>(output5);

					return;
				}
			}
		}

		template <>
		void ParsePacket<2509, ServerPacket, 0x78>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::CharacterInfoPacket output0;

			Read<long long>("characterId", handler, output0.CharacterId, "\t");
			Read<bool>("found", handler, output0.Found, "\t");

			ValidateValues(stream, "found", "\t", output0.Found, (bool)0, (bool)1);

			if (output0.Found)
			{
				long long notAccountId_var2 = 0;
				Read<long long>("notAccountId", handler, notAccountId_var2, "\t\t");

				long long characterId2_var3 = 0;
				Read<long long>("characterId2", handler, characterId2_var3, "\t\t");
				Read<long long>("currentTime", handler, output0.CurrentTime, "\t\t");

				int bufferSize_var5 = 0;
				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer0(handler, (size_t)bufferSize_var5, true);
					ParseCharacterDetailsHeader_v12(handler, output0.Details);

					{
						ParseCharacterDetailsBasicStats_v12(handler, output0.Details.Stats);
						ParseCharacterDetailsSpecialStats_v2497(handler, output0.Details.Stats);
					}
					ParseCharacterDetailsBody_v12(handler, output0.Details);

					long long unknown_var6 = 0;
					Read<long long>("unknown", handler, unknown_var6, "\t\t\t");
				}

				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer1(handler, (size_t)bufferSize_var5, true);
					ParseCharacterEquipmentItems_v2509(handler, output0.CharacterEquipment);

					unsigned char unknown_var7 = 0;
					Read<unsigned char>("unknown", handler, unknown_var7, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var7, (unsigned char)0);

					long long unknown_var8 = 0;
					Read<long long>("unknown", handler, unknown_var8, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var8, (long long)0);

					long long unknown_var9 = 0;
					Read<long long>("unknown", handler, unknown_var9, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var9, (long long)0);

					unsigned char unknown_var10 = 0;
					Read<unsigned char>("unknown", handler, unknown_var10, "\t\t\t");

					ValidateValues(stream, "unknown", "\t\t\t", unknown_var10, (unsigned char)0);
				}

				Read<int>("bufferSize", handler, bufferSize_var5, "\t\t");

				{
					Buffer buffer2(handler, (size_t)bufferSize_var5, true);
					ParseCharacterEquipmentBadges_v2509(handler, output0.CharacterEquipment);
				}
			}

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::CharacterInfoPacket>(output0);

				return;
			}
		}

		template <>
		void ParsePacket<2512, ServerPacket, 0x49>(PacketHandler& handler)
		{
			using namespace ParserUtils::Packets;

			ParserUtils::DataStream& stream = handler.PacketStream();

			Server::JobPacket output0;

			Read<int>("actorId", handler, output0.ActorId, "\t");
			Read<unsigned char>("mode", handler, output0.Mode, "\t");

			ValidateValues(stream, "mode", "\t", output0.Mode, (unsigned char)8, (unsigned char)0, (unsigned char)9, (unsigned char)1, (unsigned char)10, (unsigned char)2, (unsigned char)3, (unsigned char)7, (unsigned char)11);
			ParseSkillTreeData_v2512(handler, output0.SkillTree);

			if (stream.Succeeded())
			{
				handler.PacketParsed<Server::JobPacket>(output0);

				return;
			}
		}

	}
}
