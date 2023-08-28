#pragma once

#include <iostream>
#include <source_location>
#include <vector>

#include "DataStream.h"

std::string toString(const std::wstring& wstr);

std::ostream& operator<<(std::ostream& out, const std::wstring& string);

namespace ParserUtils
{
	namespace Packets
	{
		const bool PrintOutput = false;
		const bool PrintErrors = true;
		const bool PrintUnknownValues = true;

		template <typename T, typename Size, typename HandlerType>
		void ResizeVector(HandlerType& handler, std::vector<T>& vector, Size size, const std::source_location location = std::source_location::current())
		{
			DataStream& stream = handler.PacketStream;

			if (stream.HasRecentlyFailed)
			{
				return;
			}

			if constexpr (std::is_signed_v<Size>)
			{
				if (size < 0)
				{
					stream.HasRecentlyFailed = true;

					if constexpr (PrintErrors)
					{
						if (!stream.SuppressErrors)
						{
							std::cout << location.file_name() << "[" << location.line() << "," << location.column() << "] " << location.function_name() << ": attempt to set vector to a negative size: " << size << std::endl;
						}
					}

					return;
				}
			}

			size_t remaining = stream.Data.size() - stream.Index;

			if (size > remaining)
			{
				stream.HasRecentlyFailed = true;

				if constexpr (PrintErrors)
				{
					if (!stream.SuppressErrors)
					{
						std::cout << location.file_name() << "[" << location.line() << "," << location.column() << "] " << location.function_name() << ": attempt to set vector to a size larger than the remaining number of bytes : " << size << "; remaining: " << remaining << std::endl;
					}
				}

				return;
			}

			vector.resize(remaining);
		}

		template <typename T, typename HandlerType>
		void Read(const char* name, HandlerType& handler, T& value, const char* tabs, const std::source_location location = std::source_location::current())
		{
			DataStream& stream = handler.PacketStream;

			if (stream.HasRecentlyFailed)
			{
				return;
			}

			size_t lastIndex = stream.Index;
			bool succeeded = stream.Read<T>(value);

			if (!succeeded)
			{
				if constexpr (PrintErrors)
				{
					if (!stream.SuppressErrors)
					{
						std::cout << location.file_name() << "[" << location.line() << "," << location.column() << "] " << location.function_name() << ": failed to load packet value '" << name << "'" << std::endl;
					}
				}

				return;
			}

			if constexpr (PrintOutput || PrintErrors)
			{
				if (!(!PrintOutput && stream.SuppressErrors))
				{
					std::ostream& out = PrintOutput ? std::cout : handler.FoundValues;

					out << tabs << "[" << (stream.Index - lastIndex) << "] " << name << ": ";

					if constexpr (std::is_same_v<T, char> || std::is_same_v<T, unsigned char> || std::is_same_v<T, signed char>)
					{
						out << (int)value;
					}
					else
					{
						out << value;
					}

					out << std::endl;
				}
			}
		}

		template <typename T>
		concept IntegerType = requires(T param)
		{
			{ std::is_integral_v<T> };
		};

		template <IntegerType T>
		bool GetBit(T value, unsigned char bitIndex)
		{
			return ((value >> bitIndex) & 1) != 0;
		}

		template <typename T, typename... Types>
		void ValidateValues(ParserUtils::DataStream& stream, const char* name, const char* tabs, const T& value, Types... expected)
		{
			if constexpr (PrintUnknownValues)
			{
				bool found = stream.SuppressErrors || stream.IgnoreUnknownValues || ((value == expected) || ...);

				if (!found)
				{
					std::cout << tabs << "'" << name << "' unknown value: ";

					if constexpr (std::is_integral_v<T> && !std::is_floating_point_v<T>)
					{
						if constexpr(std::is_signed_v<T>)
						{
							std::cout << (long long)value << std::endl;
						}
						else
						{
							std::cout << (unsigned long long)value << std::endl;
						}
					}
					else
					{
						std::cout << value << std::endl;
					}

					stream.FoundUnknownValue = true;
				}
			}
		}
	}
}