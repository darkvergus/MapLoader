#include "PacketSchema.h"

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <stdlib.h>
#include <string>

#include "OutputSchema.h"
#include "ModuleWriter.h"

namespace PacketSchema
{
	const std::unordered_map<std::string, PacketType> PacketTypes = {
		{ "bool", { "bool", "bool", "false", 1}},
		{ "ubyte", { "ubyte", "unsigned char", "0", 1}},
		{ "sbyte", { "sbyte", "signed char", "0", 1}},
		{ "byte", { "byte", "unsigned char", "0", 1}},
		{ "char", { "char", "char", "0", 1}},
		{ "ushort", { "ushort", "unsigned short", "0", 2}},
		{ "short", { "short", "short", "0", 2}},
		{ "uint", { "uint", "unsigned int", "0", 4}},
		{ "int", { "int", "int", "0", 4}},
		{ "ulong", { "ulong", "unsigned long long", "0", 8}},
		{ "long", { "long", "long long", "0", 8}},
		{ "float", { "float", "float", "0", 4 }},
		{ "double", { "double", "double", "0", 8 }},
		{ "string", { "string", "std::string", "", 0 }},
		{ "wstring", { "wstring", "std::wstring", "", 0 }},
		{ "Vector3S", { "Vector3S", "Vector3S", "", 0 }},
		{ "Vector3Short", { "Vector3Short", "Vector3Short", "", 0 }},
		{ "vector", { "vector", "std::vector", "", 0 }},
		{ "Color4I", { "Color4I", "Color4I_BGRA", "", 0 }}
	};

	std::unordered_map<unsigned short, PacketVersion> PacketVersions;

	struct DataOutput
	{
		std::string Name;
		const PacketOutput* Output = nullptr;
		const OutputSchema::SchemaMember* Member = nullptr;
		const OutputSchema::SchemaClass* Class = nullptr;
	};

	struct StackEntry
	{
		size_t StartIndex = (size_t)-1;
		size_t EndIndex = (size_t)-1;
		DataOutput Output;
		bool IsLoop = false;
		bool IncrementLoopCounter = false;
	};

	DataResult findDataReference(const PacketOpcode& opcode, const std::vector<StackEntry>& stack, const std::string_view& name, size_t index)
	{
		index = index < opcode.Layout.size() ? index : opcode.Layout.size() - 1;

		const PacketInfo& current = index < opcode.Layout.size() ? opcode.Layout[index] : opcode.Layout.back();
		
		DataResult result;

		for (size_t i = 0; !result.Found && i < index; ++i)
		{
			const PacketInfo& info = opcode.Layout[i];

			if (info.Type != PacketInfoType::Data)
			{
				continue;
			}

			if (info.StackDepth > current.StackDepth)
			{
				continue;
			}

			size_t startIndex = stack[info.StackDepth - 1].StartIndex;

			if (startIndex > i)
			{
				continue;
			}

			const PacketInfo& stackStartInfo = opcode.Layout[i];

			size_t regionEnd = opcode.Layout.size();

			switch (stackStartInfo.Type)
			{
			case PacketInfoType::Condition: regionEnd = opcode.Conditions[stackStartInfo.Index].RegionEnd; break;
			case PacketInfoType::OutputMember: regionEnd = opcode.OutputMembers[stackStartInfo.Index].RegionEnd; break;
			case PacketInfoType::Array: regionEnd = opcode.Arrays[stackStartInfo.Index].RegionEnd; break;
			case PacketInfoType::Buffer: regionEnd = opcode.Buffers[stackStartInfo.Index].RegionEnd; break;
			}

			if (regionEnd <= index)
			{
				continue;
			}

			const PacketData& data = opcode.Data[info.Index];

			if (data.Name == name)
			{
				return { true, i };
			}
		}

		for (size_t i = 0; i < opcode.Parameters.size(); ++i)
		{
			if (opcode.Parameters[i] == name)
			{
				return { true, (size_t)-1 - opcode.Parameters.size() + i };
			}
		}

		return { false, (size_t)-1 };
	}

	DataResult OpcodeParser::FindDataReference(const std::string_view& name, size_t index) const
	{
		index = index < Opcode.Layout.size() ? index : Opcode.Layout.size() - 1;

		const PacketInfo& current = index < Opcode.Layout.size() ? Opcode.Layout[index] : Opcode.Layout.back();

		DataResult result;

		for (size_t i = 0; !result.Found && i < Opcode.Layout.size(); ++i)
		{
			const PacketInfo& info = Opcode.Layout[i];

			if (info.Type != PacketInfoType::Data)
			{
				continue;
			}

			if (info.StackDepth > current.StackDepth)
			{
				continue;
			}

			size_t startIndex = NodeStack[info.StackDepth - 1].StartIndex;

			if (startIndex > i)
			{
				continue;
			}

			const PacketInfo& stackStartInfo = Opcode.Layout[i];

			size_t regionEnd = Opcode.Layout.size();

			switch (stackStartInfo.Type)
			{
			case PacketInfoType::Condition: regionEnd = Opcode.Conditions[stackStartInfo.Index].RegionEnd; break;
			case PacketInfoType::OutputMember: regionEnd = Opcode.OutputMembers[stackStartInfo.Index].RegionEnd; break;
			case PacketInfoType::Array: regionEnd = Opcode.Arrays[stackStartInfo.Index].RegionEnd; break;
			case PacketInfoType::Buffer: regionEnd = Opcode.Buffers[stackStartInfo.Index].RegionEnd; break;
			}

			if (regionEnd <= index)
			{
				continue;
			}

			const PacketData& data = Opcode.Data[info.Index];

			if (data.Name == name)
			{
				return { true, i };
			}
		}

		for (size_t i = 0; i < Opcode.Parameters.size(); ++i)
		{
			if (Opcode.Parameters[i] == name)
			{
				return { true, (size_t)-1 - Opcode.Parameters.size() + i };
			}
		}

		return { false, (size_t)-1 };
	}

	void OpcodeParser::ReadOutput(tinyxml2::XMLElement* element)
	{
		PacketOutput& output = Opcode.Outputs.back();

		for (const tinyxml2::XMLAttribute* attribute = element->FirstAttribute(); attribute; attribute = attribute->Next())
		{
			const char* name = attribute->Name();
			const char* value = attribute->Value();

			if (strcmp(name, "name") == 0)
			{
				output.Name = value;

				continue;
			}

			if (strcmp(name, "type") == 0)
			{
				output.TypeName = value;

				continue;
			}

			if (strcmp(name, "schemaName") == 0)
			{
				output.SchemaName = value;

				continue;
			}

			if (strcmp(name, "noReturn") == 0)
			{
				output.ReturnOnFinish = strcmp(value, "true") != 0;

				continue;
			}

			std::cout << FileName << ": unknown attribute '" << name << "' in output node" << std::endl;
		}

		OutputSchema::SchemaEntry entry = OutputSchema::findSchemaEntry(output.TypeName, output.SchemaName);

		if (entry.Type == OutputSchema::SchemaEntryType::None)
		{
			std::cout << FileName << ": failed to find type '" << output.TypeName << "' in schema '" << output.SchemaName << "'" << std::endl;

			return;
		}

		if (entry.Type != OutputSchema::SchemaEntryType::Class)
		{
			std::cout << FileName << ": type '" << output.TypeName << "' in schema '" << output.SchemaName << "' isn't a class" << std::endl;

			return;
		}

		output.Class = entry.Class;
	}
	
	void OpcodeParser::ReadOutputMember(tinyxml2::XMLElement* element)
	{
		PacketOutputMember& outputMember = Opcode.OutputMembers.back();

		for (const tinyxml2::XMLAttribute* attribute = element->FirstAttribute(); attribute; attribute = attribute->Next())
		{
			const char* name = attribute->Name();
			const char* value = attribute->Value();

			if (strcmp(name, "output") == 0)
			{
				outputMember.Output = value;

				continue;
			}

			if (strcmp(name, "target") == 0)
			{
				outputMember.Target = value;

				continue;
			}

			std::cout << FileName << ": unknown attribute '" << name << "' in output node" << std::endl;
		}
	}

	void OpcodeParser::ReadDataRead(tinyxml2::XMLElement* element)
	{
		PacketRead& dataRead = Opcode.Reads.back();

		DataResult result;

		for (const tinyxml2::XMLAttribute* attribute = element->FirstAttribute(); attribute; attribute = attribute->Next())
		{
			const char* name = attribute->Name();
			const char* value = attribute->Value();

			if (strcmp(name, "name") == 0)
			{
				result = FindDataReference(value);

				if (!result.Found)
				{
					std::cout << FileName << ": 1` data '" << value << "' in read node" << std::endl;

					continue;
				}

				if (result.DataIndex < Opcode.Layout.size())
				{
					const PacketInfo& info = Opcode.Layout[result.DataIndex];
					PacketData& data = Opcode.Data[info.Index];

					data.Referenced = true;
				}

				dataRead.DataIndex = result.DataIndex;
				dataRead.Name = value;

				continue;
			}

			std::cout << FileName << ": unknown attribute '" << name << "' in read node" << std::endl;
		}
	}

	void OpcodeParser::ReadBuffer(tinyxml2::XMLElement* element)
	{
		PacketBuffer& buffer = Opcode.Buffers.back();

		DataResult result;

		for (const tinyxml2::XMLAttribute* attribute = element->FirstAttribute(); attribute; attribute = attribute->Next())
		{
			const char* name = attribute->Name();
			const char* value = attribute->Value();

			if (strcmp(name, "sizeData") == 0)
			{
				result = FindDataReference(value);

				if (!result.Found)
				{
					std::cout << FileName << ": unknown referenced data '" << value << "' in buffer node sizeData" << std::endl;

					continue;
				}

				if (result.DataIndex < Opcode.Layout.size())
				{
					const PacketInfo& info = Opcode.Layout[result.DataIndex];
					PacketData& data = Opcode.Data[info.Index];

					data.Referenced = true;
				}

				buffer.BufferSizeDataIndex = result.DataIndex;

				continue;
			}

			if (strcmp(name, "deflatedData") == 0)
			{
				result = FindDataReference(value);

				if (!result.Found)
				{
					std::cout << FileName << ": unknown referenced data '" << value << "' in buffer node deflatedData" << std::endl;

					continue;
				}

				if (result.DataIndex < Opcode.Layout.size())
				{
					const PacketInfo& info = Opcode.Layout[result.DataIndex];
					PacketData& data = Opcode.Data[info.Index];

					data.Referenced = true;
				}

				buffer.IsDeflatedDataIndex = result.DataIndex;

				continue;
			}

			std::cout << FileName << ": unknown attribute '" << name << "' in read node" << std::endl;
		}
	}

	void OpcodeParser::ReadFunction(tinyxml2::XMLElement* element)
	{
		PacketFunction& function = Opcode.Functions.back();

		DataResult result;

		for (const tinyxml2::XMLAttribute* attribute = element->FirstAttribute(); attribute; attribute = attribute->Next())
		{
			const char* name = attribute->Name();
			const char* value = attribute->Value();

			if (strcmp(name, "name") == 0)
			{
				function.Name = value;

				continue;
			}

			if (strcmp(name, "data") == 0)
			{
				result = FindDataReference(value);

				if (!result.Found)
				{
					std::cout << FileName << ": unknown referenced data '" << value << "' in function node" << std::endl;

					FindDataReference(value);

					continue;
				}

				if (result.DataIndex < Opcode.Layout.size())
				{
					const PacketInfo& info = Opcode.Layout[result.DataIndex];
					PacketData& data = Opcode.Data[info.Index];

					data.Referenced = true;
				}

				function.DataIndex = result.DataIndex;

				continue;
			}

			if (strcmp(name, "params") == 0)
			{
				std::string_view valueString = value;

				size_t start = 0;
				size_t i = 0;

				for (i = 0; i <= valueString.size(); ++i)
				{
					if (i < valueString.size() && valueString[i] != ',')
					{
						continue;
					}

					if (start == i)
					{
						start = i + 1;

						continue;
					}

					std::string_view current = { value + start, i - start };

					DataResult paramResult = FindDataReference(current);

					if (!paramResult.Found)
					{
						std::cout << FileName << ": unknown referenced param data '" << current << "' in function node" << std::endl;

						FindDataReference(value);

						continue;
					}

					if (paramResult.DataIndex < Opcode.Layout.size())
					{
						const PacketInfo& info = Opcode.Layout[paramResult.DataIndex];
						PacketData& data = Opcode.Data[info.Index];

						data.Referenced = true;
					}

					function.ParamDataIndices.push_back(paramResult.DataIndex);

					start = i + 1;
				}

				continue;
			}

			std::cout << FileName << ": unknown attribute '" << name << "' in function node" << std::endl;
		}
	}

	void OpcodeParser::ReadData(tinyxml2::XMLElement* element)
	{
		PacketData& data = Opcode.Data.back();

		for (const tinyxml2::XMLAttribute* attribute = element->FirstAttribute(); attribute; attribute = attribute->Next())
		{
			const char* name = attribute->Name();
			const char* value = attribute->Value();

			if (strcmp(name, "name") == 0)
			{
				data.Name = value;

				continue;
			}

			if (strcmp(name, "type") == 0)
			{
				auto typeEntry = PacketTypes.find(value);

				if (typeEntry == PacketTypes.end())
				{
					std::cout << FileName << ": unknown type '" << value << "' in data node" << std::endl;

					continue;
				}

				data.Type = &typeEntry->second;

				if (data.Type->Name == "bool")
				{
					data.EnumNames[0] = "false";
					data.EnumNames[1] = "true";
				}

				continue;
			}

			if (strcmp(name, "output") == 0)
			{
				data.Output = value;

				continue;
			}

			if (strcmp(name, "target") == 0)
			{
				data.Target = value;

				continue;
			}

			if (strcmp(name, "doRead") == 0)
			{
				data.Read = strcmp(value, "true") == 0;

				continue;
			}

			std::cout << FileName << ": unknown attribute '" << name << "' in data node" << std::endl;
		}

		if (data.Name.size() == 0)
		{
			std::cout << FileName << ": data given empty name" << std::endl;
		}

		if (data.Type == nullptr)
		{
			std::cout << FileName << ": data '" << data.Name << "' has no type" << std::endl;
		}

		for (tinyxml2::XMLElement* child = element->FirstChildElement(); child; child = child->NextSiblingElement())
		{
			const char* name = child->Name();

			if (strcmp(name, "enum") == 0)
			{
				const tinyxml2::XMLAttribute* nameAttrib = child->FindAttribute("name");
				const tinyxml2::XMLAttribute* valueAttrib = child->FindAttribute("value");

				if (nameAttrib == nullptr || valueAttrib == nullptr)
				{
					if (nameAttrib == nullptr)
					{
						std::cout << FileName << ": enum node missing name attribute" << std::endl;
					}

					if (valueAttrib == nullptr)
					{
						std::cout << FileName << ": enum node missing value attribute" << std::endl;
					}

					continue;
				}

				std::string_view valueData = valueAttrib->Value();

				bool isHex = valueData.size() > 2 && valueData[0] == '0' && (valueData[1] == 'x' || valueData[1] == 'X');

				size_t value = isHex ? (size_t)strtoll (valueData.data() + 2, nullptr, 16) : (size_t)atoll(valueData.data());

				data.EnumNames[value] = nameAttrib->Value();

				continue;
			}

			if (strcmp(name, "bit") == 0)
			{
				const tinyxml2::XMLAttribute* nameAttrib = child->FindAttribute("name");
				const tinyxml2::XMLAttribute* bitIndexAttrib = child->FindAttribute("bitIndex");
				const tinyxml2::XMLAttribute* outputAttrib = child->FindAttribute("output");

				if (nameAttrib == nullptr || bitIndexAttrib == nullptr)
				{
					if (nameAttrib == nullptr)
					{
						std::cout << FileName << ": bit node missing name attribute" << std::endl;
					}

					if (bitIndexAttrib == nullptr)
					{
						std::cout << FileName << ": bit node missing bitIndex attribute" << std::endl;
					}

					continue;
				}

				size_t bitIndex = (size_t)atoi(bitIndexAttrib->Value());

				if (bitIndex >= data.Flags.size())
				{
					data.Flags.resize(bitIndex + 1);
				}

				data.Flags[bitIndex].Name = nameAttrib->Value();

				if (outputAttrib != nullptr)
				{
					data.HasBitOutputs = true;

					data.Flags[bitIndex].Output = outputAttrib->Value();
				}

				continue;
			}

			std::cout << FileName << ": unknown node '" << name << "' in data node" << std::endl;
		}
	}

	void OpcodeParser::ReadCondition(tinyxml2::XMLElement* element)
	{
		PacketCondition& condition = Opcode.Conditions.back();

		DataResult result;

		for (const tinyxml2::XMLAttribute* attribute = element->FirstAttribute(); attribute; attribute = attribute->Next())
		{
			const char* name = attribute->Name();
			const char* value = attribute->Value();

			if (strcmp(name, "data") == 0)
			{
				result = FindDataReference(value);

				if (!result.Found)
				{
					std::cout << FileName << ": unknown referenced data '" << value << "' in condition node" << std::endl;

					continue;
				}

				if (result.DataIndex < Opcode.Layout.size())
				{
					const PacketInfo& info = Opcode.Layout[result.DataIndex];
					PacketData& data = Opcode.Data[info.Index];

					data.Referenced = true;
				}

				condition.DataIndex = result.DataIndex;

				continue;
			}

			if (strcmp(name, "value") == 0)
			{
				std::string_view valueData = value;

				bool isHex = valueData.size() > 2 && valueData[0] == '0' && (valueData[1] == 'x' || valueData[1] == 'X');

				condition.Value = isHex ? (size_t)strtoll(valueData.data() + 2, nullptr, 16) : (size_t)atoll(valueData.data());

				continue;
			}

			if (strcmp(name, "value2") == 0)
			{
				std::string_view valueData = value;

				bool isHex = valueData.size() > 2 && valueData[0] == '0' && (valueData[1] == 'x' || valueData[1] == 'X');

				condition.Value2 = isHex ? (size_t)strtoll(valueData.data() + 2, nullptr, 16) : (size_t)atoll(valueData.data());

				continue;
			}

			if (strcmp(name, "bitIndex") == 0)
			{
				if (result.Found)
				{
					size_t bitIndex = (size_t)atoi(value);

					const PacketData& data = Opcode.Data[Opcode.Layout[condition.DataIndex].Index];

					if (data.Type == nullptr)
					{
						std::cout << FileName << ": setting condition bit on data '" << data.Name << "' with null type" << std::endl;
					}
					else if (bitIndex >= data.Type->Size * 8)
					{
						std::cout << FileName << ": condition bit index set out of bounds of '" << data.Name << "' type '" << data.Type->Name << "' at index " << bitIndex << std::endl;
					}

					condition.BitIndex = (unsigned char)bitIndex;
				}

				continue;
			}

			if (strcmp(name, "comparison") == 0)
			{
				if (strcmp(value, "equal") == 0)
				{
					condition.Comparison = PacketInfoComparison::Equal;
				}
				else if (strcmp(value, "notEqual") == 0)
				{
					condition.Comparison = PacketInfoComparison::NotEqual;
				}
				else if (strcmp(value, "between") == 0)
				{
					condition.Comparison = PacketInfoComparison::Between;
				}
				else if (strcmp(value, "notBetween") == 0)
				{
					condition.Comparison = PacketInfoComparison::NotBetween;
				}
				else
				{
					std::cout << FileName << ": unknown comparison type '" << value << "'" << std::endl;
				}

				continue;
			}

			std::cout << FileName << ": unknown attribute '" << name << "' in data node" << std::endl;
		}

		if (!result.Found)
		{
			std::cout << FileName << ": condition data not found" << std::endl;
		}
	}

	void OpcodeParser::ReadArray(tinyxml2::XMLElement* element)
	{
		PacketArray& array = Opcode.Arrays.back();

		DataResult result;

		for (const tinyxml2::XMLAttribute* attribute = element->FirstAttribute(); attribute; attribute = attribute->Next())
		{
			const char* name = attribute->Name();
			const char* value = attribute->Value();

			if (strcmp(name, "data") == 0)
			{
				result = FindDataReference(value);

				if (!result.Found)
				{
					std::cout << FileName << ": unknown referenced data '" << value << "' in array node" << std::endl;

					FindDataReference(value);

					continue;
				}

				if (result.DataIndex < Opcode.Layout.size())
				{
					const PacketInfo& info = Opcode.Layout[result.DataIndex];
					PacketData& data = Opcode.Data[info.Index];

					data.Referenced = true;
				}

				array.DataIndex = result.DataIndex;

				continue;
			}

			if (strcmp(name, "output") == 0)
			{
				array.Output = value;

				continue;
			}

			if (strcmp(name, "target") == 0)
			{
				array.Target = value;

				continue;
			}

			if (strcmp(name, "name") == 0)
			{
				array.Name = value;

				continue;
			}

			if (strcmp(name, "type") == 0)
			{
				if (strcmp(value, "while") == 0)
				{
					array.Type = PacketArray::TypeEnum::While;
				}

				continue;
			}

			std::cout << FileName << ": unknown attribute '" << name << "' in data node" << std::endl;
		}

		if (!result.Found)
		{
			std::cout << FileName << ": condition data not found" << std::endl;
		}
	}

	void OpcodeParser::Read(tinyxml2::XMLElement* element)
	{
		NodeStack.push_back({ element->FirstChildElement() });
		NodeStack.back().StartIndex = 0;

		while (NodeStack.size())
		{
			NodeStackEntry& node = NodeStack.back();
			bool forcePopEarly = false;

			if (node.Element != nullptr)
			{
				const char* name = node.Element->Name();

				if (strcmp(name, "data") == 0)
				{
					Opcode.Layout.push_back({ PacketInfoType::Data, Opcode.Data.size(), NodeStack.size() });
					Opcode.Data.push_back({ Opcode.Data.size() });

					ReadData(node.Element);
				}
				else if (strcmp(name, "condition") == 0)
				{
					node.ConditionIndex = Opcode.Conditions.size();

					Opcode.Layout.push_back({ PacketInfoType::Condition, Opcode.Conditions.size(), NodeStack.size() });
					Opcode.Conditions.push_back({});

					ReadCondition(node.Element);

					NodeStack.push_back({ node.Element->FirstChildElement() });
					NodeStack.back().StartIndex = Opcode.Layout.size();

					continue;
				}
				else if (strcmp(name, "output") == 0)
				{
					Opcode.Layout.push_back({ PacketInfoType::Output, Opcode.Outputs.size(), NodeStack.size() });
					Opcode.Outputs.push_back({ Opcode.Outputs.size() });

					ReadOutput(node.Element);
				}
				else if (strcmp(name, "array") == 0)
				{
					node.ArrayIndex = Opcode.Arrays.size();

					Opcode.Layout.push_back({ PacketInfoType::Array, Opcode.Arrays.size(), NodeStack.size() });
					Opcode.Arrays.push_back({});

					ReadArray(node.Element);

					NodeStack.push_back({ node.Element->FirstChildElement() });
					NodeStack.back().StartIndex = Opcode.Layout.size();

					continue;
				}
				else if (strcmp(name, "read") == 0)
				{
					Opcode.Layout.push_back({ PacketInfoType::DataRead, Opcode.Reads.size(), NodeStack.size() });
					Opcode.Reads.push_back({ Opcode.Reads.size() });

					ReadDataRead(node.Element);
				}
				else if (strcmp(name, "function") == 0)
				{
					Opcode.Layout.push_back({ PacketInfoType::Function, Opcode.Functions.size(), NodeStack.size() });
					Opcode.Functions.push_back({ Opcode.Functions.size() });

					ReadFunction(node.Element);
				}
				else if (strcmp(name, "outputMember") == 0)
				{
					node.OutputMemberIndex = Opcode.OutputMembers.size();

					Opcode.Layout.push_back({ PacketInfoType::OutputMember, Opcode.OutputMembers.size(), NodeStack.size() });
					Opcode.OutputMembers.push_back({ Opcode.OutputMembers.size() });

					ReadOutputMember(node.Element);

					NodeStack.push_back({ node.Element->FirstChildElement() });
					NodeStack.back().StartIndex = Opcode.Layout.size();

					continue;
				}
				else if (strcmp(name, "buffer") == 0)
				{
					node.BufferIndex = Opcode.Buffers.size();

					Opcode.Layout.push_back({ PacketInfoType::Buffer, Opcode.Buffers.size(), NodeStack.size() });
					Opcode.Buffers.push_back({ Opcode.Buffers.size() });

					ReadBuffer(node.Element);

					NodeStack.push_back({ node.Element->FirstChildElement() });
					NodeStack.back().StartIndex = Opcode.Layout.size();

					continue;
				}
				else if (strcmp(name, "useBlock") == 0)
				{
					const tinyxml2::XMLAttribute* nameAttrib = node.Element->FindAttribute("name");
					const tinyxml2::XMLAttribute* versionAttrib = node.Element->FindAttribute("version");
					const tinyxml2::XMLAttribute* outputAttrib = node.Element->FindAttribute("output");
					const tinyxml2::XMLAttribute* targetAttrib = node.Element->FindAttribute("target");

					if (nameAttrib == nullptr || versionAttrib == nullptr)
					{
						if (!nameAttrib)
						{
							std::cout << "block node missing name attribute" << std::endl;
						}

						if (!versionAttrib)
						{
							std::cout << "block node missing version attribute" << std::endl;
						}

						continue;
					}

					if (outputAttrib != nullptr)
					{
						node.OutputMemberIndex = Opcode.OutputMembers.size();

						Opcode.Layout.push_back({ PacketInfoType::OutputMember, Opcode.OutputMembers.size(), NodeStack.size() });
						Opcode.OutputMembers.push_back({ Opcode.OutputMembers.size() });

						Opcode.OutputMembers.back().Output = outputAttrib->Value();

						if (targetAttrib)
						{
							Opcode.OutputMembers.back().Target = targetAttrib->Value();
						}

						NodeStack.push_back({ nullptr });
						NodeStack.back().StartIndex = Opcode.Layout.size();

						forcePopEarly = true;
					}

					const char* name = nameAttrib->Value();
					unsigned short version = (unsigned short)atoi(versionAttrib->Value());

					const auto& versionEntry = PacketVersions.find(version);

					if (versionEntry == PacketVersions.end())
					{
						std::cout << "useBlock node referencing unknown packet version " << version << std::endl;

						continue;
					}

					const auto& blockEntry = versionEntry->second.Blocks.find(name);

					if (versionEntry == PacketVersions.end())
					{
						std::cout << "useBlock node referencing unknown block '" << name << "' in packet version " << version << std::endl;

						continue;
					}

					const PacketOpcode& block = blockEntry->second;

					size_t dataCount = Opcode.Data.size();
					size_t layoutCount = Opcode.Layout.size();
					size_t start = Opcode.Layout.size();

					Opcode.Layout.resize(start + block.Layout.size());

					const auto updateData = [this, &block, layoutCount](size_t& index, size_t infoIndex)
					{
						size_t newLayoutCount = Opcode.Layout.size();

						if (index < block.Layout.size())
						{
							index += layoutCount;

							if (index >= newLayoutCount)
							{
								std::cout << Opcode.Name << ": data index adjusted to out of bounds index: " << index << " vs " << newLayoutCount << std::endl;
							}
							
							return;
						}

						if (index < (size_t)-1 && index >= (size_t)-1 - block.Parameters.size())
						{
							size_t param = index - ((size_t)-1 - block.Parameters.size());

							DataResult result = FindDataReference(block.Parameters[param], infoIndex);

							if (result.Found)
							{
								index = result.DataIndex;

								if (index >= newLayoutCount && !(index < (size_t)-1 && index >= (size_t)-1 - Opcode.Parameters.size()))
								{
									std::cout << Opcode.Name << ": data index adjusted to out of bounds index: " << index << " vs " << newLayoutCount << std::endl;
								}

								return;
							}
						}
					};

					start = Opcode.Data.size();

					Opcode.Data.resize(start + block.Data.size());

					for (size_t i = start; i < Opcode.Data.size(); ++i)
					{
						auto& data = Opcode.Data[i];

						data = block.Data[i - start];

						data.Index = i;
					}

					start = Opcode.Conditions.size();

					Opcode.Conditions.resize(start + block.Conditions.size());

					for (size_t i = start; i < Opcode.Conditions.size(); ++i)
					{
						auto& condition = Opcode.Conditions[i];

						condition = block.Conditions[i - start];

						condition.RegionEnd += layoutCount;
					}

					start = Opcode.Outputs.size();

					Opcode.Outputs.resize(start + block.Outputs.size());

					for (size_t i = start; i < Opcode.Outputs.size(); ++i)
					{
						auto& output = Opcode.Outputs[i];

						output = block.Outputs[i - start];

						output.OutputIndex += start;
					}

					start = Opcode.OutputMembers.size();

					Opcode.OutputMembers.resize(start + block.OutputMembers.size());

					for (size_t i = start; i < Opcode.OutputMembers.size(); ++i)
					{
						auto& outputMember = Opcode.OutputMembers[i];

						outputMember = block.OutputMembers[i - start];

						outputMember.OutputMemberIndex += start;
						outputMember.RegionEnd += layoutCount;
					}

					start = Opcode.Arrays.size();

					Opcode.Arrays.resize(start + block.Arrays.size());

					for (size_t i = start; i < Opcode.Arrays.size(); ++i)
					{
						auto& array = Opcode.Arrays[i];

						array = block.Arrays[i - start];

						array.RegionEnd += layoutCount;
					}

					start = Opcode.Reads.size();

					Opcode.Reads.resize(start + block.Reads.size());

					for (size_t i = start; i < Opcode.Reads.size(); ++i)
					{
						auto& read = Opcode.Reads[i];

						read = block.Reads[i - start];

						read.Index += start;
					}

					start = Opcode.Functions.size();

					Opcode.Functions.resize(start + block.Functions.size());

					for (size_t i = start; i < Opcode.Functions.size(); ++i)
					{
						auto& function = Opcode.Functions[i];

						function = block.Functions[i - start];

						function.Index += start;
					}

					start = Opcode.Buffers.size();

					Opcode.Buffers.resize(start + block.Buffers.size());

					for (size_t i = start; i < Opcode.Buffers.size(); ++i)
					{
						auto& buffer = Opcode.Buffers[i];

						buffer = block.Buffers[i - start];

						buffer.BufferIndex += start;
						buffer.RegionEnd += layoutCount;
					}

					for (size_t i = layoutCount; i < Opcode.Layout.size(); ++i)
					{
						auto& layout = Opcode.Layout[i];

						layout = block.Layout[i - layoutCount];

						layout.StackDepth += NodeStack.size() - 1;

						switch (layout.Type)
						{
						case PacketInfoType::Data: layout.Index += Opcode.Data.size() - block.Data.size(); break;
						case PacketInfoType::Condition:
							layout.Index += Opcode.Conditions.size() - block.Conditions.size();
							updateData(Opcode.Conditions[layout.Index].DataIndex, i);

							break;
						case PacketInfoType::Output: layout.Index += Opcode.Outputs.size() - block.Outputs.size(); break;
						case PacketInfoType::OutputMember: layout.Index += Opcode.OutputMembers.size() - block.OutputMembers.size(); break;
						case PacketInfoType::Array:
							layout.Index += Opcode.Arrays.size() - block.Arrays.size();
							updateData(Opcode.Arrays[layout.Index].DataIndex, i);

							break;
						case PacketInfoType::DataRead:
							layout.Index += Opcode.Reads.size() - block.Reads.size();
							updateData(Opcode.Reads[layout.Index].DataIndex, i);

							break;
						case PacketInfoType::Function:
							layout.Index += Opcode.Functions.size() - block.Functions.size();
							updateData(Opcode.Functions[layout.Index].DataIndex, i);
							for (size_t& paramIndex : Opcode.Functions[layout.Index].ParamDataIndices)
							{
								updateData(paramIndex, i);
							}

							break;
						case PacketInfoType::Buffer:
							layout.Index += Opcode.Buffers.size() - block.Buffers.size();
							updateData(Opcode.Buffers[layout.Index].BufferSizeDataIndex, i);
							updateData(Opcode.Buffers[layout.Index].IsDeflatedDataIndex, i);

							break;
						}
					}
				}
				else
				{
					std::cout << FileName << ": unknown node '" << name << "'" << std::endl;
				}
			}

			while (NodeStack.size() && (NodeStack.back().Element == nullptr || forcePopEarly))
			{
				forcePopEarly = false;

				NodeStack.pop_back();

				if (NodeStack.size())
				{
					NodeStackEntry& next = NodeStack.back();

					if (next.ConditionIndex != (size_t)-1)
					{
						Opcode.Conditions[next.ConditionIndex].RegionEnd = Opcode.Layout.size();
					}

					if (next.ArrayIndex != (size_t)-1)
					{
						Opcode.Arrays[next.ArrayIndex].RegionEnd = Opcode.Layout.size();
					}

					if (next.OutputMemberIndex != (size_t)-1)
					{
						Opcode.OutputMembers[next.OutputMemberIndex].RegionEnd = Opcode.Layout.size();
					}

					if (next.BufferIndex != (size_t)-1)
					{
						Opcode.Buffers[next.BufferIndex].RegionEnd = Opcode.Layout.size();
					}

					next.ConditionIndex = (size_t)-1;
					next.ArrayIndex = (size_t)-1;
					next.OutputMemberIndex = (size_t)-1;
					next.BufferIndex = (size_t)-1;
				}
			}

			if (NodeStack.size())
			{
				NodeStack.back().Element = NodeStack.back().Element->NextSiblingElement();
				NodeStack.back().ConditionIndex = (size_t)-1;
				NodeStack.back().ArrayIndex = (size_t)-1;
				NodeStack.back().OutputMemberIndex = (size_t)-1;
				NodeStack.back().BufferIndex = (size_t)-1;
			}
		}
	}

	void OpcodeParser::Print()
	{
		if constexpr (!PrintGeneratedOpcodes)
		{
			return;
		}

		char tabs[32] = { 0 };
		const char* const typeNames[] = {
			"Data",
			"Condition",
			"Output",
			"OutputMember",
			"Array",
			"Function",
			"Buffer",
		};
		const char* const compares[] = {
			"Equal",
			"NotEqual",
			"Between",
			"NotBetween"
		};
		bool compare2[] = {
			false,
			false,
			true,
			true
		};

		const auto indent = [&tabs](size_t depth)
		{
			tabs[0] = '\n';

			for (int i = 0; i <= (int)depth + 1; ++i)
			{
				tabs[i + 1] = i < depth ? '\t' : 0;
			}

			return tabs;
		};

		for (size_t i = 0; i < Opcode.Layout.size(); ++i)
		{
			const auto& layout = Opcode.Layout[i];

			size_t index = layout.Index;

			std::cout << (indent(layout.StackDepth) + 1) << "[" << i << "] " << typeNames[(int)layout.Type] << " <" << layout.StackDepth << ">: " << index;

			switch (layout.Type)
			{
			case PacketInfoType::Data:
				std::cout << indent(layout.StackDepth + 1) << Opcode.Data[index].Type->Name << " " << Opcode.Data[index].Name << " - " << Opcode.Data[index].Output << " [" << Opcode.Data[index].Target << "]";
				break;
			case PacketInfoType::DataRead:
				std::cout << indent(layout.StackDepth + 1) << Opcode.Reads[index].Name << "; Data: [" << Opcode.Reads[index].DataIndex << "]";
				break;
			case PacketInfoType::Condition:
				std::cout << " -> [" << Opcode.Conditions[index].RegionEnd << "]" << indent(layout.StackDepth + 1) << "Data: [" << Opcode.Conditions[index].DataIndex << "]: " << compares[(int)Opcode.Conditions[index].Comparison] << "(" << Opcode.Conditions[index].Value;
				if (compare2[(int)Opcode.Conditions[index].Comparison])
					std::cout << ", " << Opcode.Conditions[index].Value2;
				std::cout << ")";
				break;
			case PacketInfoType::Output:
				std::cout << indent(layout.StackDepth + 1) << Opcode.Outputs[index].Class->Name << " " << Opcode.Outputs[index].Name;
				break;
			case PacketInfoType::OutputMember:
				std::cout << indent(layout.StackDepth + 1) << Opcode.OutputMembers[index].Output << " [" << Opcode.OutputMembers[index].Target << "]";
				break;
			case PacketInfoType::Array:
				std::cout << " -> [" << Opcode.Arrays[index].RegionEnd << "]" << indent(layout.StackDepth + 1) << "Data: [" << Opcode.Arrays[index].DataIndex << "]: " << Opcode.Arrays[index].Output << " [" << Opcode.Arrays[index].Target << "]";
				break;
			case PacketInfoType::Function:
				std::cout << indent(layout.StackDepth + 1) << "Data: [" << Opcode.Functions[index].DataIndex << "]: " << Opcode.Functions[index].Name << "(";
				for (size_t i = 0; i < Opcode.Functions[index].ParamDataIndices.size(); ++i)
				{
					std::cout << "[" << Opcode.Functions[index].ParamDataIndices[i] << "]";
					if (i + 1 < Opcode.Functions[index].ParamDataIndices.size())
						std::cout << ", ";
				}
				std::cout << ")";
				break;
			case PacketInfoType::Buffer:
				std::cout << " -> [" << Opcode.Buffers[index].RegionEnd << "]" << indent(layout.StackDepth + 1) << "Buffer([" << Opcode.Buffers[index].BufferSizeDataIndex << "], [" << Opcode.Buffers[index].IsDeflatedDataIndex << "])";
				break;
			}

			std::cout << std::endl;
		}
	}

	void readSchema(const fs::path& filePath)
	{
		tinyxml2::XMLDocument document;

		document.LoadFile(filePath.string().c_str());

		tinyxml2::XMLElement* root = document.RootElement();

		if (root == nullptr)
		{
			std::cout << "invalid xml " << filePath << std::endl;

			return;
		}

		const tinyxml2::XMLAttribute* versionAttrib = root->FindAttribute("version");

		if (versionAttrib == nullptr)
		{
			return;
		}

		unsigned short version = (unsigned short)atoi(versionAttrib->Value());

		PacketVersion& schema = PacketVersions[version];

		schema.Version = version;

		const tinyxml2::XMLAttribute* inheritAttrib = root->FindAttribute("inheritPrevious");

		if (inheritAttrib != nullptr)
		{
			schema.InheritPrevious = strcmp(inheritAttrib->Value(), "true") == 0;
		}

		for (tinyxml2::XMLElement* child = root->FirstChildElement(); child; child = child->NextSiblingElement())
		{
			if (strcmp(child->Name(), "blocks") == 0)
			{
				for (tinyxml2::XMLElement* blockElement = child->FirstChildElement(); blockElement; blockElement = blockElement->NextSiblingElement())
				{
					if (strcmp(blockElement->Name(), "block") != 0)
					{
						std::cout << filePath.string() << ": unknown node '" << blockElement->Name() << "' in blocks" << std::endl;

						continue;
					}

					const tinyxml2::XMLAttribute* nameAttrib = blockElement->FindAttribute("name");
					const tinyxml2::XMLAttribute* paramsAttrib = blockElement->FindAttribute("params");

					if (nameAttrib == nullptr)
					{
						std::cout << filePath.string() << ": opcode node missing name attribute" << std::endl;

						continue;
					}

					const char* name = nameAttrib->Value();

					PacketOpcode& block = schema.Blocks[name];

					block.Name = name;
					block.IsServer = false;

					OpcodeParser parser = { filePath.string(), block };

					if (paramsAttrib != nullptr)
					{
						std::string_view paramsValue = paramsAttrib->Value();

						size_t start = 0;
						size_t length = 0;

						while (start <= paramsValue.size())
						{
							for (length; start + length < paramsValue.size() && paramsValue[start + length] != ','; ++length);

							if (length == 0)
							{
								++start;
							}

							std::string_view name = { paramsValue.data() + start, length };

							start += length + 1;
							length = 0;

							block.Parameters.push_back(std::string(name));
						}
					}

					if constexpr (PrintGeneratedOpcodes)
					{
						std::cout << "Block: " << name << std::endl;
					}

					parser.Read(blockElement);
					parser.Print();
				}

				continue;
			}

			if (strcmp(child->Name(), "source") != 0)
			{
				std::cout << filePath.string() << ": unknown node '" << child->Name() << "' in packetSchema" << std::endl;

				continue;
			}

			const tinyxml2::XMLAttribute* nameAttrib = child->FindAttribute("name");

			if (nameAttrib == nullptr)
			{
				std::cout << filePath.string() << ": source node missing name attribute" << std::endl;

				continue;
			}

			const char* name = nameAttrib->Value();

			bool isServer = strcmp(name, "server") == 0;
			bool isClient = strcmp(name, "client") == 0;

			if (!isServer && !isClient)
			{
				std::cout << filePath.string() << ": source node name is invalid value '" << name << "'" << std::endl;

				continue;
			}

			for (tinyxml2::XMLElement* opcodeElement = child->FirstChildElement(); opcodeElement; opcodeElement = opcodeElement->NextSiblingElement())
			{
				const char* name = opcodeElement->Name();

				if (strcmp(name, "opcodeReference") == 0)
				{
					const tinyxml2::XMLAttribute* valueAttrib = opcodeElement->FindAttribute("value");
					const tinyxml2::XMLAttribute* removeAttrib = opcodeElement->FindAttribute("removeReference");
					const tinyxml2::XMLAttribute* versionAttrib = opcodeElement->FindAttribute("version");
					const tinyxml2::XMLAttribute* opcodeAttrib = opcodeElement->FindAttribute("opcode");

					bool remove = false;

					if (removeAttrib)
					{
						if (!valueAttrib)
						{
							std::cout << filePath.string() << ": opcodeReference node missing version attribute" << std::endl;

							continue;
						}

						remove = strcmp(removeAttrib->Value(), "true") == 0;
					}

					if (!remove && (versionAttrib == nullptr || valueAttrib == nullptr || opcodeAttrib == nullptr))
					{
						if (!versionAttrib)
						{
							std::cout << filePath.string() << ": opcodeReference node missing version attribute" << std::endl;
						}

						if (!valueAttrib)
						{
							std::cout << filePath.string() << ": opcodeReference node missing value attribute" << std::endl;
						}

						if (!opcodeAttrib)
						{
							std::cout << filePath.string() << ": opcodeReference node missing opcode attribute" << std::endl;
						}

						continue;
					}

					unsigned short value = 0;
					unsigned short version = 0;
					unsigned short opcode = 0;

					{
						size_t valueStart = 0;
						const char* valueData = valueAttrib->Value();

						for (size_t i = 0; !valueStart && valueData[i]; ++i)
						{
							if (valueData[i] == 'x')
							{
								valueStart = i + 1;
							}
						}

						if (valueStart == 0)
						{
							value = (unsigned short)atoi(valueData);
						}
						else
						{
							value = (unsigned short)strtol(valueData + valueStart, nullptr, 16);
						}
					}

					if (remove)
					{
						(isServer ? schema.DoNotInheritServer : schema.DoNotInheritClient).insert(value);

						continue;
					}

					version = (unsigned short)atoi(versionAttrib->Value());

					{
						size_t valueStart = 0;
						const char* valueData = opcodeAttrib->Value();

						for (size_t i = 0; !valueStart && valueData[i]; ++i)
						{
							if (valueData[i] == 'x')
							{
								valueStart = i + 1;
							}
						}

						if (valueStart == 0)
						{
							opcode = (unsigned short)atoi(valueData);
						}
						else
						{
							opcode = (unsigned short)strtol(valueData + valueStart, nullptr, 16);
						}
					}

					(isServer ? schema.ServerOpcodeReferences : schema.ClientOpcodeReferences)[value] = { value, version, opcode };

					continue;
				}

				if (strcmp(name, "opcode") != 0)
				{
					continue;
				}

				const tinyxml2::XMLAttribute* nameAttrib = opcodeElement->FindAttribute("name");
				const tinyxml2::XMLAttribute* valueAttrib = opcodeElement->FindAttribute("value");

				if (nameAttrib == nullptr || valueAttrib == nullptr)
				{
					if (!nameAttrib)
					{
						std::cout << filePath.string() << ": opcode node missing name attribute" << std::endl;
					}

					if (!valueAttrib)
					{
						std::cout << filePath.string() << ": opcode node missing value attribute" << std::endl;
					}

					continue;
				}

				unsigned short value = 0;
				size_t valueStart = 0;
				const char* valueData = valueAttrib->Value();

				for (size_t i = 0; !valueStart && valueData[i]; ++i)
				{
					if (valueData[i] == 'x')
					{
						valueStart = i + 1;
					}
				}

				if (valueStart == 0)
				{
					value = (unsigned short)atoi(valueData);
				}
				else
				{
					value = (unsigned short)strtol(valueData + valueStart, nullptr, 16);
				}

				PacketOpcode& opcode = (isServer ? schema.ServerOpcodes : schema.ClientOpcodes)[value];

				opcode.Name = nameAttrib->Value();
				opcode.Value = value;
				opcode.IsServer = isServer;

				OpcodeParser parser = { filePath.string(), opcode };

				if constexpr (PrintGeneratedOpcodes)
				{
					std::cout << "Opcode: " << (isServer ? "[Server] 0x" : "[Client] 0x") << std::hex << value << std::dec << std::endl;
				}

				parser.Read(opcodeElement);
				parser.Print();
			}
		}
	}

	void readSchemas(const fs::path& directory)
	{
		std::vector<unsigned int> versions;
		std::vector<fs::path> paths;

		for (const auto& entry : fs::recursive_directory_iterator{ directory })
		{
			const fs::path& path = entry.path();

			if (!fs::is_regular_file(path) && !path.has_extension()) continue;

			fs::path extension = path.extension();

			if (extension != ".xml") continue;

			unsigned int version = (unsigned int)atoi(path.filename().stem().string().c_str());
			unsigned int index = (unsigned int)paths.size();

			paths.push_back(path);
			versions.push_back((version << 16) | index);
		}

		std::sort(versions.begin(), versions.end());

		for (unsigned int data : versions)
		{
			unsigned int index = data & 0xFFFF;

			readSchema(paths[index]);
		}
	}

	const fs::path packetProjDir = "./../PacketProcessing";

	struct Generator
	{
		size_t Depth = 3;
		char Tabs[32] = "\t\t\t";
		char LoopIndex = 'i';

		void Push()
		{
			SetDepth(Depth + 1);
		}

		void Pop()
		{
			SetDepth(Depth - 1);
		}

		void SetDepth(size_t depth)
		{
			size_t min = std::min(Depth, depth) - 1;
			size_t max = std::max(Depth, depth);

			for (size_t i = min; i < max; ++i)
			{
				Tabs[i] = i < depth ? '\t' : 0;
			}

			Depth = depth;
		}
	};

	std::ostream& operator<<(std::ostream& out, const PacketData& data)
	{
		return out << data.Name << "_var" << data.Index;
	}

	std::ostream& operator<<(std::ostream& out, const PacketOutput& output)
	{
		if (output.Name.size())
		{
			out << output.Name << "_";
		}

		return out << "output" << output.OutputIndex;
	}

	DataOutput findOutput(const std::string& name, const std::unordered_map<std::string, const PacketOutput*>& outputs, const PacketOutput* topOutput, const std::vector<StackEntry>& stack)
	{
		if (name.size())
		{
			auto entry = outputs.find(name);

			if (entry != outputs.end())
			{
				return { "", entry->second };
			}

			return {};
		}

		for (size_t i = 0; i < stack.size(); ++i)
		{
			const auto& entry = stack[stack.size() - i - 1];

			if (entry.Output.Output)
			{
				return entry.Output;
			}
		}

		return { "", topOutput };
	}

	DataOutput findOutput(DataOutput output, const std::string& path)
	{
		const OutputSchema::SchemaClass* outputClass = output.Class == nullptr && output.Output != nullptr ? output.Output->Class : output.Class;

		if (!output.Output)
		{
			return {};
		}

		output.Member = OutputSchema::findSchemaMember(outputClass, path);

		if (!output.Member)
		{
			return {};
		}

		if (output.Name.size())
		{
			output.Name += "." + path;
		}
		else
		{
			std::stringstream stream;

			stream << *output.Output << "." << path;

			output.Name = stream.str();
		}

		if (output.Member->ContainsType.size())
		{
			if (output.Member->Type == "array")
			{
				return output;
			}

			OutputSchema::SchemaEntry entry = OutputSchema::findSchemaEntry(output.Member->ContainsType, output.Output->SchemaName);

			output.Class = entry.Class;

			if (entry.Class == nullptr)
			{
				std::cout << "failed to find class: '" << output.Member->ContainsType << "'" << std::endl;
			}
		}
		else
		{
			OutputSchema::SchemaEntry entry = OutputSchema::findSchemaEntry(output.Member->Type, output.Member->ParentSchemaName);

			output.Class = entry.Class;
		}

		return output;
	}

	DataOutput findOutput(const PacketOpcode& opcode, size_t index, const std::string& name, const std::unordered_map<std::string, const PacketOutput*>& outputs, const PacketOutput* topOutput, const std::vector<StackEntry>& stack, const std::string& path)
	{
		DataOutput output = findOutput(name, outputs, topOutput, stack);

		size_t start = 0;
		size_t length = 0;

		while (start < path.size())
		{
			for (length; start + length < path.size() && path[start + length] != '.'; ++length);

			if (length == 0)
			{
				return {};
			}

			std::string_view name = { path.data() + start, length };

			start += length + 1;
			length = 0;

			std::string_view indexValue;

			if (name.back() == ']')
			{
				size_t openBracket = name.size() - 1;

				while (openBracket < name.size() && name[openBracket] != '[')
				{
					--openBracket;
				}

				if (openBracket >= name.size())
				{
					std::cout << "missing open bracket on output '" << path << "'" << std::endl;

					return {};
				}

				indexValue = { name.data() + openBracket + 1, name.size() - 2 - openBracket };

				name = { name.data(), openBracket };
			}

			DataOutput nextOutput = findOutput(output, std::string(name));

			if (indexValue.size())
			{
				if (nextOutput.Member && nextOutput.Member->Type == "array")
				{
					nextOutput.Name += '[' + std::string(indexValue) + ']';

					return nextOutput;
				}

				if (!nextOutput.Member || !nextOutput.Member->ContainsType.size())
				{
					std::cout << "attempt to use subscript '" << indexValue << "' in output '" << path << "' that isn't an indexable type" << std::endl;

					return {};
				}

				DataResult result = findDataReference(opcode, stack, indexValue, index);

				if (!result.Found)
				{
					nextOutput.Name += '[' + std::string(indexValue) + ']';

					continue;
				}

				const PacketData& paramData = opcode.Data[opcode.Layout[result.DataIndex].Index];

				DataOutput paramOutput = paramData.Output.size() ? findOutput(opcode, index, paramData.Target, outputs, topOutput, stack, paramData.Output) : DataOutput{};

				bool makeVariable = !paramData.Read || paramData.HasBitOutputs || paramData.Referenced || paramData.Output.size() == 0 || (paramOutput.Member && paramOutput.Member->Type != paramData.Type->TypeName);

				nextOutput.Name += '[';

				if (makeVariable || !paramOutput.Member)
				{
					std::stringstream out;
					out << paramData;
					nextOutput.Name += out.str();
				}
				else
				{
					nextOutput.Name += paramOutput.Name;
				}

				nextOutput.Name += ']';
			}

			output = nextOutput;
		}

		return output;
	}

	std::string getReference(size_t dataIndex, const PacketOpcode& opcode, size_t index, const std::unordered_map<std::string, const PacketOutput*>& outputs, const PacketOutput* topOutput, const std::vector<StackEntry>& stack)
	{
		const PacketData& paramData = opcode.Data[opcode.Layout[dataIndex].Index];

		DataOutput paramOutput = paramData.Output.size() ? findOutput(opcode, index, paramData.Target, outputs, topOutput, stack, paramData.Output) : DataOutput{};

		bool memberTypeDoesntMatch = paramOutput.Member && (paramOutput.Member->Type == "array" ? paramOutput.Member->ContainsType != paramData.Type->TypeName : paramOutput.Member->Type != paramData.Type->TypeName);
		bool makeVariable = !paramData.Read || paramData.HasBitOutputs || paramData.Referenced || paramData.Output.size() == 0 || memberTypeDoesntMatch;

		if (makeVariable || !paramOutput.Member)
		{
			std::stringstream out;
			out << paramData;

			return out.str();
		}
		else
		{
			return paramOutput.Name;
		}
	}

	std::unordered_map<const OutputSchema::SchemaClass*, bool> outputsReferenced;

	void generateParser(Generator& generator, std::ofstream& out, const PacketOpcode& opcode)
	{
		generator.SetDepth(3);

		const auto& tabs = generator.Tabs;

		PacketInfoType lastType = PacketInfoType::Condition;

		std::vector<StackEntry> stack;
		std::unordered_map<std::string, const PacketOutput*> outputs;

		out << tabs << "using namespace ParserUtils::Packets;\n\n";
		out << tabs << "ParserUtils::DataStream& stream = handler.PacketStream;\n\n";

		const PacketOutput* topOutput = nullptr;

		for (size_t i = 0; i < opcode.Layout.size(); ++i)
		{
			while (stack.size() && i >= stack.back().EndIndex)
			{
				DataOutput& outputData = stack.back().Output;

				const PacketOutput* output = outputData.Output;

				if (output != nullptr && outputData.Member == nullptr)
				{
					if (!outputsReferenced.contains(output->Class))
					{
						outputsReferenced[output->Class] = opcode.IsServer;
					}

					out << "\n" << tabs << "if (stream.Succeeded())\n" << tabs << "{\n";
					out << tabs << "\thandler.PacketParsed<" << OutputSchema::swapSeparator(OutputSchema::stripCommonNamespaces(output->Class->Scope, "Networking.Packets"), "::") << ">(" << *output << ");\n";

					if (output->ReturnOnFinish)
					{
						out << "\n\t" << tabs << "return;\n";
					}

					out << tabs << "}\n";

					if (outputs.contains(output->Name))
					{
						outputs.erase(output->Name);
					}
				}

				if (stack.back().IsLoop)
				{
					--generator.LoopIndex;

					if (stack.back().IncrementLoopCounter)
					{
						out << "\n" << tabs << "++" << generator.LoopIndex << ";\n";
					}
				}

				generator.Pop();
				out << tabs << "}\n";
				stack.pop_back();
			}

			PacketInfoType type = opcode.Layout[i].Type;
			size_t index = opcode.Layout[i].Index;

			if (opcode.Layout[i].StackDepth - 1 != stack.size())
			{
				std::cout << opcode.Name << "[" << i << "]: stack size mismatch: " << opcode.Layout[i].StackDepth - 1 << " vs " << stack.size() << std::endl;
			}

			if (type == PacketInfoType::Data || type == PacketInfoType::DataRead || type == PacketInfoType::Function)
			{
				size_t functionIndex = index;

				if (type == PacketInfoType::DataRead)
				{
					const PacketRead& dataRead = opcode.Reads[index];

					index = opcode.Layout[dataRead.DataIndex].Index;
				}
				else if (type == PacketInfoType::Function)
				{
					const PacketFunction& function = opcode.Functions[index];

					index = opcode.Layout[function.DataIndex].Index;
				}

				const PacketData& data = opcode.Data[index];

				DataOutput output = data.Output.size() ? findOutput(opcode, i, data.Target, outputs, topOutput, stack, data.Output) : DataOutput{};

				bool memberTypeDoesntMatch = output.Member && (output.Member->Type == "array" ? output.Member->ContainsType != data.Type->TypeName : output.Member->Type != data.Type->TypeName);
				bool makeVariable = !data.Read || data.HasBitOutputs || data.Referenced || data.Output.size() == 0 || memberTypeDoesntMatch;

				if ((lastType != PacketInfoType::Data || makeVariable || !output.Member) && lastType != PacketInfoType::Condition && lastType != PacketInfoType::Array && lastType != PacketInfoType::OutputMember && lastType != PacketInfoType::Buffer)
				{
					out << "\n";
				}

				if (type != PacketInfoType::Function)
				{
					if (type == PacketInfoType::Data)
					{
						if (makeVariable || !output.Member)
						{
							out << tabs << data.Type->TypeName << " " << data;

							if (data.Type->DefaultValue.size())
							{
								out << " = " << data.Type->DefaultValue;
							}

							out << ";\n";
						}
					}

					if (data.Read)
					{
						out << tabs << "Read<" << data.Type->TypeName << ">(\"" << data.Name << "\", handler, ";

						if (makeVariable || !output.Member)
						{
							out << data;
						}
						else
						{
							out << output.Name;
						}

						out << ", \"";

						for (size_t i = 0; i <= stack.size(); ++i)
						{
							out << "\\t";
						}

						out << "\"" << ");\n";
					}
				}
				else
				{
					const PacketFunction& function = opcode.Functions[functionIndex];

					out << tabs;

					if (makeVariable || !output.Member)
					{
						out << data;
					}
					else
					{
						out << output.Name;
					}

					out << " = handler." << function.Name << "(";

					for (size_t j = 0; j < function.ParamDataIndices.size(); ++j)
					{

						std::string param = getReference(function.ParamDataIndices[j], opcode, i, outputs, topOutput, stack);

						out << param;

						if (j + 1 < function.ParamDataIndices.size())
						{
							out << ", ";
						}
					}

					out << ");\n";
				}

				bool isDataInitialization = opcode.Layout[i].Type == PacketInfoType::Data && opcode.Layout[i].Index == index;

				if (data.EnumNames.size() && (data.Read || !isDataInitialization))
				{
					type = PacketInfoType::Validation;

					out << "\n" << tabs << "ValidateValues(stream, \"" << data.Name << "\", \"";

					for (size_t i = 0; i <= stack.size(); ++i)
					{
						out << "\\t";
					}
					
					out << "\", ";

					if (makeVariable || !output.Member)
					{
						out << data;
					}
					else
					{
						out << output.Name;
					}

					out << ", ";

					size_t i = 0;

					std::string typeName = OutputSchema::swapSeparator(OutputSchema::stripCommonNamespaces(data.Type->TypeName, "Networking.Packets"), "::");

					for (const auto& enumEntry : data.EnumNames)
					{
						++i;

						out << "(" << typeName << ")" << enumEntry.first;

						if (i < data.EnumNames.size())
						{
							out << ", ";
						}
					}

					out << ");\n";
				}

				if (output.Member && makeVariable)
				{
					type = PacketInfoType::ValueWrite;

					out << "\n" << tabs << output.Name << " = " << "(" << OutputSchema::swapSeparator(OutputSchema::stripCommonNamespaces(output.Member->Type, "Networking.Packets"), "::") << ")" << data << ";\n";
				}

				if (data.HasBitOutputs)
				{
					out << "\n";

					for (size_t i = 0; i < data.Flags.size(); ++i)
					{
						const PacketDataBit& bit = data.Flags[i];

						if (bit.Output.size())
						{
							DataOutput output = findOutput(opcode, i, bit.Target, outputs, topOutput, stack, bit.Output);

							out << tabs << output.Name << " = GetBit(" << data << ", " << i << ");\n";
						}
					}

					type = PacketInfoType::ValueWrite;
				}

				lastType = type;

				continue;
			}

			if (type == PacketInfoType::OutputMember)
			{
				const PacketOutputMember& outputMember = opcode.OutputMembers[index];

				DataOutput output = outputMember.Output.size() ? findOutput(opcode, i, outputMember.Target, outputs, topOutput, stack, outputMember.Output) : DataOutput{};

				out << "\n" << tabs << "{\n";

				stack.push_back({ i, outputMember.RegionEnd, output });
				generator.Push();
				
				continue;
			}

			if (type == PacketInfoType::Buffer)
			{
				const PacketBuffer& buffer = opcode.Buffers[index];

				out << "\n" << tabs << "{\n";

				stack.push_back({ i, buffer.RegionEnd });
				generator.Push();

				std::string sizeParam = getReference(buffer.BufferSizeDataIndex, opcode, i, outputs, topOutput, stack);
				std::string isDeflatedParam = getReference(buffer.IsDeflatedDataIndex, opcode, i, outputs, topOutput, stack);

				out << tabs << "Buffer buffer" << index << "(handler, " << sizeParam << ", " << isDeflatedParam << ");\n";

				continue;
			}

			if (type == PacketInfoType::Array)
			{
				const PacketArray& array = opcode.Arrays[index];
				const PacketData& data = opcode.Data[opcode.Layout[array.DataIndex].Index];

				std::string param = getReference(array.DataIndex, opcode, i, outputs, topOutput, stack);

				DataOutput output = array.Output.size() ? findOutput(opcode, i, array.Target, outputs, topOutput, stack, array.Output) : DataOutput{};

				if (lastType != PacketInfoType::Condition && lastType != PacketInfoType::Array)
				{
					out << "\n";
				}

				std::string containerName = output.Name;

				output.Name = containerName + '[' + generator.LoopIndex + ']';

				if (array.Type == PacketArray::TypeEnum::For)
				{
					if (output.Output)
					{
						out << tabs << "ResizeVector(handler, " << containerName << ", " << data << ");\n\n";
					}

					out << tabs << "for (" << data.Type->TypeName << " " << generator.LoopIndex << " = 0; " << generator.LoopIndex << " < " << param << " && !handler.PacketStream.HasRecentlyFailed; ++" << generator.LoopIndex << ")\n";
					out << tabs << "{\n";

					stack.push_back({ i, array.RegionEnd, output, true });
					generator.Push();

					lastType = PacketInfoType::Array;

					if (array.Name.size())
					{
						std::stringstream newName;

						newName << array.Name << "_array" << index;

						out << tabs << "auto& " << newName.str() << " = " << output.Name << ";\n";

						stack.back().Output.Name = newName.str();

						lastType = PacketInfoType::ArrayOutput;
					}

					++generator.LoopIndex;

					continue;
				}

				if (array.Type == PacketArray::TypeEnum::While)
				{
					out << tabs << "size_t " << generator.LoopIndex << " = 0;\n\n";

					out << tabs << "while (" << data << " && !handler.PacketStream.HasRecentlyFailed)\n";
					out << tabs << "{\n";

					stack.push_back({ i, array.RegionEnd, output, true, true });
					generator.Push();

					if (output.Output)
					{
						out << tabs << containerName << ".push_back({});\n";
					}

					if (array.Name.size())
					{
						std::stringstream newName;

						newName << array.Name << "_array" << index;

						out << "\n" << tabs << "auto& " << newName.str() << " = " << output.Name << ";\n";

						stack.back().Output.Name = newName.str();

						lastType = PacketInfoType::ArrayOutput;
					}

					++generator.LoopIndex;

					continue;
				}

				continue;
			}

			if (type == PacketInfoType::Condition)
			{
				const PacketCondition& condition = opcode.Conditions[index];

				size_t dataIndex = opcode.Layout[condition.DataIndex].Index;

				const PacketData& data = opcode.Data[dataIndex];

				if (lastType != PacketInfoType::Condition && lastType != PacketInfoType::Array)
				{
					out << "\n";
				}

				out << tabs << "if (";

				std::string param = getReference(condition.DataIndex, opcode, i, outputs, topOutput, stack);

				if (condition.BitIndex != 0xFF)
				{
					out << "GetBit(" << param << ", " << (int)condition.BitIndex << ")";
				}
				else
				{
					out << param;
				}

				if (condition.Comparison == PacketInfoComparison::Equal)
				{
					if (condition.Value != 1 || data.Type->TypeName != "bool")
					{
						out << " == " << condition.Value;
					}
				}
				else if (condition.Comparison == PacketInfoComparison::NotEqual)
				{
					out << " != " << condition.Value;
				}
				else if (condition.Comparison == PacketInfoComparison::Between)
				{
					out << " >= " << condition.Value << " && " << param << " <= " << condition.Value2;
				}
				else if (condition.Comparison == PacketInfoComparison::NotBetween)
				{
					out << " < " << condition.Value << " || " << param << " > " << condition.Value2;
				}

				out << ")\n";
				out << tabs << "{\n";

				stack.push_back({ i, condition.RegionEnd });
				generator.Push();

				lastType = type;

				continue;
			}

			if (type == PacketInfoType::Output)
			{
				const PacketOutput& output = opcode.Outputs[index];

				out << tabs << OutputSchema::swapSeparator(OutputSchema::stripCommonNamespaces(output.Class->Scope, "Networking.Packets"), "::") << " " << output << ";\n";

				if (stack.size())
				{
					stack.back().Output.Output = &output;
				}
				else
				{
					topOutput = &output;
				}

				outputs[output.Name] = &output;

				lastType = type;

				continue;
			}
		}

		while (stack.size())
		{
			DataOutput& outputData = stack.back().Output;

			const PacketOutput* output = outputData.Output;

			if (output != nullptr && outputData.Member == nullptr)
			{
				if (!outputsReferenced.contains(output->Class))
				{
					outputsReferenced[output->Class] = opcode.IsServer;
				}

				out << "\n" << tabs << "if (stream.Succeeded())\n" << tabs << "{\n";
				out << tabs << "\thandler.PacketParsed<" << OutputSchema::swapSeparator(OutputSchema::stripCommonNamespaces(output->Class->Scope, "Networking.Packets"), "::") << ">(" << *output << ");\n";

				if (output->ReturnOnFinish)
				{
					out << "\n\t" << tabs << "return;\n";
				}

				out << tabs << "}\n";

				if (outputs.contains(output->Name))
				{
					outputs.erase(output->Name);
				}
			}

			if (stack.back().IsLoop)
			{
				--generator.LoopIndex;

				if (stack.back().IncrementLoopCounter)
				{
					out << "\n" << tabs << "++" << generator.LoopIndex << ";\n";
				}
			}
			
			generator.Pop();
			out << tabs << "}\n";
			stack.pop_back();
		}

		if (topOutput)
		{
			if (!outputsReferenced.contains(topOutput->Class))
			{
				outputsReferenced[topOutput->Class] = opcode.IsServer;
			}

			out << "\n" << tabs << "if (stream.Succeeded())\n" << tabs << "{\n";
			out << tabs << "\thandler.PacketParsed<" << OutputSchema::swapSeparator(OutputSchema::stripCommonNamespaces(topOutput->Class->Scope, "Networking.Packets"), "::") << ">(" << *topOutput << ");\n";

			if (topOutput->ReturnOnFinish)
			{
				out << "\n\t" << tabs << "return;\n";
			}

			out << tabs << "}\n";
		}

		generator.LoopIndex = 'i';
	}

	void generateParsers()
	{
		fs::path headerPath = packetProjDir / "src/PacketProcessing/PacketParser.h";

		{
			std::ofstream headerOut(headerPath);

			headerOut << "#pragma once\n\n";

			headerOut << "#include \"PacketParserBase.h\"\n\n";

			headerOut << "namespace Networking\n{\n";

			headerOut << "\tnamespace Packets\n\t{\n";

			for (const auto& versionEntry : PacketVersions)
			{
				short version = versionEntry.first;

				for (const auto& opcodeEntry : versionEntry.second.ClientOpcodes)
				{
					short opcode = opcodeEntry.first;

					headerOut << "\t\ttemplate <>\n";
					headerOut << "\t\tvoid ParsePacket<" << version << ", ClientPacket, 0x" << std::hex << opcode << std::dec << ">(PacketHandler& handler);\n\n";
				}

				for (const auto& opcodeEntry : versionEntry.second.ServerOpcodes)
				{
					short opcode = opcodeEntry.first;

					headerOut << "\t\ttemplate <>\n";
					headerOut << "\t\tvoid ParsePacket<" << version << ", ServerPacket, 0x" << std::hex << opcode << std::dec << ">(PacketHandler& handler);\n\n";
				}
			}

			headerOut << "\t}\n";
			headerOut << "}\n";
		}

		fs::path cppPath = packetProjDir / "src/PacketProcessing/PacketParser.cpp";

		outputsReferenced.clear();

		{
			std::ofstream cppOut(cppPath);

			cppOut << "#include \"PacketParser.h\"\n\n";
			cppOut << "#include <vector>\n\n";
			cppOut << "#include <ParserUtils/PacketParsing.h>\n\n";

			cppOut << "namespace Networking\n{\n";

			cppOut << "\tnamespace Packets\n\t{\n";

			Generator generator;

			unsigned short minVersion = 0xFFFF;
			unsigned short maxVersion = 0;

			for (const auto& versionEntry : PacketVersions)
			{
				if (versionEntry.first > 13)
				{
					minVersion = std::min(minVersion, versionEntry.first);
					maxVersion = std::max(maxVersion, versionEntry.first);
				}
			}

			if (minVersion != 0xFFFF && PacketVersions.contains(13))
			{
				--minVersion;
			}

			struct OpcodeData
			{
				const PacketOpcode* Opcode = nullptr;
				const PacketOpcodeReference* Reference = nullptr;
				std::string Name;
				unsigned short Version = 0;
			};

			struct VersionData
			{
				const PacketVersion* Version = nullptr;
				size_t Index = (size_t)-1;

				unsigned short MinServerOpcode = 0xFFFF;
				unsigned short MaxServerOpcode = 0;
				unsigned short MinClientOpcode = 0xFFFF;
				unsigned short MaxClientOpcode = 0;

				std::vector<OpcodeData> ServerOpcodes;
				std::vector<OpcodeData> ClientOpcodes;
			};

			std::vector<VersionData> versions;
			std::vector<VersionData> versionOverrides(maxVersion - minVersion + 1);

			if (PacketVersions.contains(12))
			{
				versions.push_back({ &PacketVersions[12], 12 });
			}

			if (PacketVersions.contains(13))
			{
				versionOverrides[0] = { &PacketVersions[13], 13 };
			}

			size_t baseVersionCount = versions.size();

			for (const auto& versionEntry : PacketVersions)
			{
				if (versionEntry.first == 12)
				{
					continue;
				}

				versionOverrides[std::max(versionEntry.first, minVersion) - minVersion] = { &versionEntry.second };
			}

			for (size_t i = 0; i < versionOverrides.size(); ++i)
			{
				VersionData& data = versionOverrides[i];

				if (!data.Version)
				{
					versionOverrides[i] = versionOverrides[i - 1];

					continue;
				}

				data.Index = versions.size();
				
				versions.push_back({ data.Version, (unsigned short)data.Version->Version });
			}

			cppOut << "\t\tvoid ParsePacket(PacketHandler& handler, unsigned short version, bool isServer, unsigned short opcode)\n";
			cppOut << "\t\t{\n";

			cppOut << "\t\t\tstatic const std::vector<size_t> versionIndices = {\n";
			cppOut << "\t\t\t\t";

			for (size_t i = 0; i < versionOverrides.size(); ++i)
			{
				cppOut << versionOverrides[i].Index;

				if (i + 1 < versionOverrides.size())
				{
					cppOut << ", ";
				}
			}

			cppOut << "\n";
			cppOut << "\t\t\t};\n";

			cppOut << "\t\t\tstatic const std::vector<PacketVersionData> versions = {\n";

			for (size_t i = 0; i < versions.size(); ++i)
			{
				size_t versionId = versions[i].Index;

				{

					VersionData& versionData = versions[i];
					const PacketVersion* version = versionData.Version;

					bool hasServerOpcodes = false;
					bool hasClientOpcodes = false;

					unsigned short& minServerOpcode = versionData.MinServerOpcode;
					unsigned short& maxServerOpcode = versionData.MaxServerOpcode;
					unsigned short& minClientOpcode = versionData.MinClientOpcode;
					unsigned short& maxClientOpcode = versionData.MaxClientOpcode;

					if (i > 0 && version->InheritPrevious)
					{
						minServerOpcode = versions[i - 1].MinServerOpcode;
						maxServerOpcode = versions[i - 1].MaxServerOpcode;
						minClientOpcode = versions[i - 1].MinClientOpcode;
						maxClientOpcode = versions[i - 1].MaxClientOpcode;

						hasServerOpcodes = versions[i - 1].ServerOpcodes.size() > 0;
						hasClientOpcodes = versions[i - 1].ClientOpcodes.size() > 0;
					}

					for (const auto& opcodeEntry : version->ServerOpcodes)
					{
						minServerOpcode = std::min(minServerOpcode, opcodeEntry.first);
						maxServerOpcode = std::max(maxServerOpcode, opcodeEntry.first);
					}

					for (const auto& opcodeEntry : version->ClientOpcodes)
					{
						minClientOpcode = std::min(minClientOpcode, opcodeEntry.first);
						maxClientOpcode = std::max(maxClientOpcode, opcodeEntry.first);
					}

					for (const auto& opcodeEntry : version->ServerOpcodeReferences)
					{
						minServerOpcode = std::min(minServerOpcode, opcodeEntry.first);
						maxServerOpcode = std::max(maxServerOpcode, opcodeEntry.first);
					}

					for (const auto& opcodeEntry : version->ClientOpcodeReferences)
					{
						minClientOpcode = std::min(minClientOpcode, opcodeEntry.first);
						maxClientOpcode = std::max(maxClientOpcode, opcodeEntry.first);
					}

					hasServerOpcodes |= version->ServerOpcodes.size() || version->ServerOpcodeReferences.size();
					hasClientOpcodes |= version->ClientOpcodes.size() || version->ClientOpcodeReferences.size();

					std::vector<OpcodeData>& serverOpcodes = versionData.ServerOpcodes;
					std::vector<OpcodeData>& clientOpcodes = versionData.ClientOpcodes;

					serverOpcodes.resize(hasServerOpcodes ? maxServerOpcode - minServerOpcode + 1 : 0);
					clientOpcodes.resize(hasClientOpcodes ? maxClientOpcode - minClientOpcode + 1 : 0);

					if (i > 0 && version->InheritPrevious)
					{
						for (size_t j = minServerOpcode; j <= maxServerOpcode; ++j)
						{
							if (!version->DoNotInheritServer.contains((unsigned short)j))
							{
								serverOpcodes[j - minServerOpcode] = versions[i - 1].ServerOpcodes[j - versions[i - 1].MinServerOpcode];
							}
						}

						for (size_t j = minClientOpcode; j <= maxClientOpcode; ++j)
						{
							if (!version->DoNotInheritClient.contains((unsigned short)j))
							{
								clientOpcodes[j - minClientOpcode] = versions[i - 1].ClientOpcodes[j - versions[i - 1].MinClientOpcode];
							}
						}
					}

					for (const auto& opcodeEntry : version->ServerOpcodes)
					{
						serverOpcodes[opcodeEntry.first - minServerOpcode] = { &opcodeEntry.second, nullptr, opcodeEntry.second.Name, version->Version };
					}

					for (const auto& opcodeEntry : version->ClientOpcodes)
					{
						clientOpcodes[opcodeEntry.first - minClientOpcode] = { &opcodeEntry.second, nullptr, opcodeEntry.second.Name, version->Version };
					}

					for (const auto& opcodeEntry : version->ServerOpcodeReferences)
					{
						const PacketOpcodeReference& reference = opcodeEntry.second;
						std::string name = "error";

						size_t index = 0;

						if (reference.TargetVersion == 12)
						{
							index = 0;
						}
						else if (reference.TargetVersion == 13)
						{
							index = baseVersionCount;
						}
						if (reference.TargetVersion >= minVersion && reference.TargetVersion <= maxVersion)
						{
							index = versionOverrides[reference.TargetVersion - minVersion].Index;
						}

						name = versions[index].ServerOpcodes[reference.TargetOpcode - versions[index].MinServerOpcode].Name;

						serverOpcodes[opcodeEntry.first - minServerOpcode] = { nullptr, &reference, name };
					}

					for (const auto& opcodeEntry : version->ClientOpcodeReferences)
					{
						const PacketOpcodeReference& reference = opcodeEntry.second;
						std::string name = "error";

						size_t index = 0;

						if (reference.TargetVersion == 12)
						{
							index = 0;
						}
						else if (reference.TargetVersion == 13)
						{
							index = baseVersionCount;
						}
						else if (reference.TargetVersion >= minVersion && reference.TargetVersion <= maxVersion)
						{
							index = versionOverrides[reference.TargetVersion - minVersion].Index;
						}

						name = versions[index].ClientOpcodes[reference.TargetOpcode - versions[index].MinClientOpcode].Name;

						clientOpcodes[opcodeEntry.first - minClientOpcode] = { nullptr, &opcodeEntry.second, name };
					}

					cppOut << "\t\t\t\t{\n";
					cppOut << "\t\t\t\t\t0x" << std::hex << minServerOpcode << ", 0x" << minClientOpcode << std::dec << ",\n";

					if (serverOpcodes.size() > 0)
					{
						cppOut << "\t\t\t\t\t{\n";

						for (size_t i = 0; i < serverOpcodes.size(); ++i)
						{
							const OpcodeData& opcodeData = serverOpcodes[i];

							if (opcodeData.Reference != nullptr)
							{
								cppOut << "\t\t\t\t\t\t{ \"" << opcodeData.Name << "\", &ParsePacket<" << opcodeData.Reference->TargetVersion << ", ServerPacket, 0x" << std::hex << opcodeData.Reference->TargetOpcode << std::dec << "> }" << (i + 1 < serverOpcodes.size() ? ",\n" : "\n");

								continue;
							}

							const PacketOpcode* opcode = opcodeData.Opcode;

							if (!opcode)
							{
								cppOut << "\t\t\t\t\t\t{ \"\", &ParsePacket<0, false, 0> }" << (i + 1 < serverOpcodes.size() ? ",\n" : "\n");

								continue;
							}

							cppOut << "\t\t\t\t\t\t{ \"" << opcode->Name << "\", &ParsePacket<" << opcodeData.Version << ", ServerPacket, 0x" << std::hex << (minServerOpcode + i) << std::dec << (i + 1 < serverOpcodes.size() ? "> },\n" : "> }\n");
						}

						cppOut << "\t\t\t\t\t},\n";
					}
					else
					{
						cppOut << "\t\t\t\t\t{ },\n";
					}

					if (clientOpcodes.size() > 0)
					{
						cppOut << "\t\t\t\t\t{\n";

						for (size_t i = 0; i < clientOpcodes.size(); ++i)
						{
							const OpcodeData& opcodeData = clientOpcodes[i];

							if (opcodeData.Reference != nullptr)
							{
								cppOut << "\t\t\t\t\t\t{ \"" << opcodeData.Name << "\", &ParsePacket<" << opcodeData.Reference->TargetVersion << ", ClientPacket, 0x" << std::hex << opcodeData.Reference->TargetOpcode << std::dec << "> }" << (i + 1 < clientOpcodes.size() ? ",\n" : "\n");

								continue;
							}

							const PacketOpcode* opcode = opcodeData.Opcode;

							if (!opcode)
							{
								cppOut << "\t\t\t\t\t\t{ \"\", &ParsePacket<0, false, 0> }" << (i + 1 < clientOpcodes.size() ? ",\n" : "\n");

								continue;
							}

							cppOut << "\t\t\t\t\t\t{ \"" << opcode->Name << "\", &ParsePacket<" << opcodeData.Version << ", ClientPacket, 0x" << std::hex << (minClientOpcode + i) << std::dec << (i + 1 < clientOpcodes.size() ? "> },\n" : "> }\n");
						}

						cppOut << "\t\t\t\t\t}\n";
					}
					else
					{
						cppOut << "\t\t\t\t\t{ }\n";
					}

					cppOut << "\t\t\t\t}";

					if (i + 1 < versions.size())
					{
						cppOut << ",";
					}

					cppOut << "\n";
				}
			}

			cppOut << "\t\t\t};\n\n";

			cppOut << "\t\t\tsize_t versionIndex = version == 12 ? 0 : versionIndices[std::min(" << maxVersion - minVersion << ", std::max(version, (unsigned short)" << minVersion << ") - " << minVersion << ")];\n";

			cppOut << "\t\t\tconst PacketVersionData& versionData = versions[versionIndex];\n\n";

			cppOut << "\t\t\tsize_t minOpcode = isServer ? versionData.MinServerOpcode : versionData.MinClientOpcode;\n\n";
			cppOut << "\t\t\tconst auto& opcodes = isServer ? versionData.ServerCallbacks : versionData.ClientCallbacks;\n\n";

			cppOut << "\t\t\tif (opcode < minOpcode || opcode - minOpcode >= opcodes.size())\n";
			cppOut << "\t\t\t{\n";
			
			cppOut << "\t\t\t\treturn;\n";
			
			cppOut << "\t\t\t}\n\n";
			cppOut << "\t\t\tif constexpr (ParserUtils::Packets::PrintOutput)\n";
			cppOut << "\t\t\t{\n";
			cppOut << "\t\t\t\tstd::cout << \"[\" << version << \"] Packet Opcode 0x\" << std::hex << opcode << std::dec << \": \" << opcodes[opcode - minOpcode].Name << std::endl;\n";
			cppOut << "\t\t\t}\n\n";

			cppOut << "\t\t\topcodes[opcode - minOpcode].Callback(handler);\n";
;
			cppOut << "\t\t}\n\n";

			for (const auto& versionEntry : PacketVersions)
			{
				short version = versionEntry.first;

				for (const auto& opcodeEntry : versionEntry.second.ClientOpcodes)
				{
					short opcode = opcodeEntry.first;

					cppOut << "\t\ttemplate <>\n";
					cppOut << "\t\tvoid ParsePacket<" << version << ", ClientPacket, 0x" << std::hex << opcode << std::dec << ">(PacketHandler& handler)\n";
					cppOut << "\t\t{\n";

					generateParser(generator, cppOut, opcodeEntry.second);

					cppOut << "\t\t}\n\n";
				}

				for (const auto& opcodeEntry : versionEntry.second.ServerOpcodes)
				{
					short opcode = opcodeEntry.first;

					cppOut << "\t\ttemplate <>\n";
					cppOut << "\t\tvoid ParsePacket<" << version << ", ServerPacket, 0x" << std::hex << opcode << std::dec << ">(PacketHandler& handler)\n";
					cppOut << "\t\t{\n";

					generateParser(generator, cppOut, opcodeEntry.second);

					cppOut << "\t\t}\n\n";
				}
			}

			cppOut << "\t}\n";
			cppOut << "}\n";
		}
	}

	void generateHandler(const std::string& name)
	{
		fs::path handlerPath = packetProjDir / "src/PacketProcessing/Handlers" / name;
		fs::path headerPath = handlerPath / (name + ".h");

		{
			std::ofstream headerOut(headerPath);

			headerOut << "#pragma once\n\n";

			headerOut << "#include \"" << name << "-decl.h\"\n\n";
			headerOut << "#include \"./../../PacketParserBase.h\"\n\n";

			for (const auto& entry : outputsReferenced)
			{
				const OutputSchema::SchemaClass* schemaClass = entry.first;

				headerOut << "#include <GameData/" << schemaClass->Directory << "/" << schemaClass->Name << ".h>\n";
			}

			headerOut << "\nnamespace Networking\n{\n";

			headerOut << "\tnamespace Packets\n\t{\n";

			for (const auto& entry : outputsReferenced)
			{
				const OutputSchema::SchemaClass* schemaClass = entry.first;

				std::string typeName = OutputSchema::swapSeparator(OutputSchema::stripCommonNamespaces(schemaClass->Scope, "Networking.Packets"), "::");
				headerOut << "\t\ttemplate <>\n";
				headerOut << "\t\tvoid " << name << "::PacketParsed<" << typeName << ">(const " << typeName << "& packet);\n\n";
			}

			headerOut << "\t}\n";
			headerOut << "}\n";
		}

		fs::path vcxprojPath = packetProjDir / "PacketProcessing.vcxproj";
		fs::path filtersPath = packetProjDir / "PacketProcessing.vcxproj.filters";

		tinyxml2::XMLDocument vcxproj;
		tinyxml2::XMLDocument filters;

		vcxproj.LoadFile(vcxprojPath.string().c_str());
		filters.LoadFile(filtersPath.string().c_str());

		tinyxml2::XMLElement* vcxprojRoot = vcxproj.RootElement();
		tinyxml2::XMLElement* filtersRoot = filters.RootElement();

		fs::create_directories(handlerPath / "Server");
		fs::create_directories(handlerPath / "Client");

		for (const auto& entry : outputsReferenced)
		{
			const OutputSchema::SchemaClass* schemaClass = entry.first;
			bool isServer = entry.second;

			fs::path cppPath = handlerPath / (isServer ? "Server" : "Client") / (schemaClass->Name + (isServer ? "-s-" : "-c-") + name + ".cpp");

			if (fs::exists(cppPath))
			{
				continue;
			}

			std::ofstream cppOut(cppPath);

			cppOut << "#include \"./../" << name << ".h\"\n\n";

			cppOut << "namespace Networking\n{\n";

			cppOut << "\tnamespace Packets\n\t{\n";

			std::string typeName = OutputSchema::swapSeparator(OutputSchema::stripCommonNamespaces(schemaClass->Scope, "Networking.Packets"), "::");
			cppOut << "\t\ttemplate <>\n";
			cppOut << "\t\tvoid " << name << "::PacketParsed<" << typeName << ">(const " << typeName << "& packet)\n\t\t{\n\t\t\t\n\t\t}\n";

			cppOut << "\t}\n";
			cppOut << "}\n";

			std::string filterData = "Source Files\\Handlers\\" + name + (isServer ? "\\Server" : "\\Client");

			OutputSchema::addProjectNode(vcxprojRoot, "ClCompile", cppPath.string(), nullptr);
			OutputSchema::addProjectNode(filtersRoot, "ClCompile", cppPath.string(), filterData.c_str());
		}

		vcxproj.SaveFile(vcxprojPath.string().c_str());
		filters.SaveFile(filtersPath.string().c_str());
	}
}