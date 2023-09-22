#pragma once

#include <GameData/Enums/Handles.h>

namespace Networking
{
	namespace Packets
	{
		namespace Server
		{
			struct PlayInstrumentPlayNotePacket
			{
				Enum::InstrumentId InstrumentId = (Enum::InstrumentId)0;
				Enum::ActorId ActorId = (Enum::ActorId)0;
				int MidiNote = 0;
			};
		}
	}
}