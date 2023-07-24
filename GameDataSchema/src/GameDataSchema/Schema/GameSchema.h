#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <regex>
#include <string_view>

#include <tinyxml2/tinyxml2.h>
#include <ArchiveParser/ArchiveReader.h>

namespace GameSchema
{
	struct SchemaType
	{
		std::string Name;

		bool(*ValidateValue)(const std::string_view& value);

		SchemaType(const std::string& name) : Name(name), ValidateValue(nullptr) {}

		template <typename T>
		SchemaType(const std::string& name, const T& validator) : Name(name)
		{
			ValidateValue = &T::ValidateValue;
		}
	};

	extern const std::unordered_map<std::string, SchemaType> Types;

	struct SchemaFile
	{
		std::string Name;
		std::regex NameRegex;

		std::vector<std::string> NameArguments;
		std::vector<const SchemaType*> NameArgumentTypes;
	};

	struct SchemaDirectory
	{
		bool Recursive = false;
		std::string Path;

		std::vector<SchemaFile> Files;
	};

	struct SchemaProperty
	{
		std::string Name;
		std::string Default;

		std::vector<const SchemaType*> Types;
	};

	struct SchemaNode
	{
		std::string Name;
		bool Optional = false;
		bool AllowMultiple = false;
		bool AllowUnknownChildren = false;

		std::vector<SchemaNode> ChildNodes;
		std::vector<SchemaProperty> Properties;
	};

	struct Schema
	{
		std::string Name;
		std::string Root;
		bool AllowMultipleRoots = false;
		bool AllowUnknownChildren = false;

		std::vector<SchemaDirectory> Directories;
		std::vector<SchemaNode> Nodes;
	};

	extern std::unordered_map<std::string, Schema> Gms2Schemas;
	extern std::unordered_map<std::string, Schema> Kms2Schemas;

	void readFile(Schema& schema, SchemaFile& file, tinyxml2::XMLElement* fileElement);

	void readDirectory(Schema& schema, SchemaDirectory& directory, tinyxml2::XMLElement* directoryElement);

	void readProperty(Schema& schema, SchemaProperty& property, tinyxml2::XMLElement* propertyElement);

	void readNode(Schema& schema, SchemaNode& node, tinyxml2::XMLElement* nodeElement);

	void readLayout(Schema& schema, tinyxml2::XMLElement* layoutElement);

	void readSchema(const fs::path& filePath, bool isGms2);

	void readSchemas(const fs::path& directory, bool isGms2);

	bool validateNodeSchema(const Schema& schema, const SchemaNode& node, tinyxml2::XMLElement* element, const std::string& path, bool printSuccesses);

	bool validateFileSchema(const Schema& schema, const Archive::ArchivePath& path, bool printSuccesses);

	void validateSchemas(Archive::ArchiveReader& reader, bool isGms2, bool printSuccesses = false);
}