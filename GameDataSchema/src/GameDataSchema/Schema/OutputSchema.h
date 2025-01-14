#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

#include <tinyxml2/tinyxml2.h>

namespace fs = std::filesystem;

namespace OutputSchema
{
	struct SchemaMember
	{
		std::string Type;
		std::string Name;
		std::string DefaultValue;
		std::string ContainsType;
		std::string SchemaName;
		std::string Scope;
		std::string ParentSchemaName;
		size_t Length = 0;
		size_t ChildClassIndex = (size_t)-1;
		std::unordered_map<std::string, std::string> DefaultInitializerValues;
	};

	struct SchemaEnumValue
	{
		std::string Name;
		std::string Value;
	};

	struct SchemaEnum
	{
		std::string Name;
		std::string Scope;
		std::string Directory = "Enums";
		std::string SchemaName;
		std::string ParentType;
		std::string FileName;

		std::vector<SchemaEnumValue> Values;
	};

	struct SchemaClass
	{
		bool IsOptional = false;
		bool IsMember = true;
		bool IsVector = true;
		bool IsMap = true;
		std::string Name;
		std::string TypeName;
		std::string KeyTypeName;
		std::string MemberName;
		std::string Scope;
		std::string ChildTypeSuffix;
		std::string Directory = "Data";
		std::string FileName;
		std::string SchemaName;

		std::vector<SchemaClass> ChildClasses;
		std::vector<SchemaMember> Members;
		std::vector<SchemaEnum> Enums;

		std::unordered_set<std::string> RequiredHeaders;
	};

	struct SchemaCollection
	{
		std::string Name;
		std::string KeyName;
		std::string KeyType;
		std::string StoredClass;
		std::string Scope;
		std::string SchemaName;
	};

	struct SchemaNamespace
	{
		std::string Name;
		std::string Scope;
		std::string SchemaName;

		std::vector<SchemaNamespace> Namespaces;
		std::vector<SchemaCollection> Collections;
		std::vector<SchemaClass> Classes;
		std::vector<SchemaEnum> Enums;
	};

	struct CollectionSchema
	{
		std::string Name;

		SchemaNamespace Global;
	};

	enum class SchemaEntryType
	{
		None,
		Namespace,
		Class,
		Collection,
		Enum
	};

	struct SchemaEntry
	{
		SchemaEntryType Type = SchemaEntryType::None;

		const SchemaNamespace* Namespace = nullptr;
		const SchemaClass* Class = nullptr;
		const SchemaCollection* Collection = nullptr;
		const SchemaEnum* Enum = nullptr;
	};

	extern std::unordered_map<std::string, CollectionSchema> CollectionSchemas;
	extern std::vector<CollectionSchema*> GlobalSchemas;

	void readNamespace(SchemaNamespace& schemaNamespace, tinyxml2::XMLElement* element);

	void readClass(SchemaClass& schemaClass, tinyxml2::XMLElement* element, SchemaClass* rootClass, const std::string& typeSuffix = "");

	void readCollection(SchemaCollection& schemaCollection, tinyxml2::XMLElement* element);

	void readSchemaElement(SchemaNamespace& parentNamespace, tinyxml2::XMLElement* element);

	void readSchema(const fs::path& filePath);

	void readSchemas(const fs::path& directory);

	SchemaEntry findSchemaEntry(const std::string_view& path, const std::string& schemaName, char separator = '.');
	std::string findIncludeDirectory(const std::string_view& path, const std::string& schemaName, char separator = '.');
	const SchemaMember* findSchemaMember(const SchemaClass* schemaClass, const std::string& path, char separator = '.');
	std::string pickSchema(const std::string & override, const std::string& base);

	std::string stripCommonNamespaces(const std::string& primaryNamespace, const std::string& secondaryNamespace, char separator = '.');
	std::string findFullPath(const std::string& currentPath, const std::string& subPath, const std::string& schemaName, char separator = '.');
	std::string stripLastPath(const std::string& path, char separator = '.');
	std::string swapSeparator(const std::string& path, std::string newSeparator, char separator = '.');
}