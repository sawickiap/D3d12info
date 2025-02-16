/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/

#pragma once

#include <string_view>
#include <wtypes.h>

struct EnumItem;

class ReportFormatter
{
public:
	enum class Flags : uint32_t
	{
		None = 0,
		UseJson = 1 << 0,
		UseJsonPrettyPrint = 1 << 1
	};

	enum class ArraySuffix
	{
		SquareBrackets,
		None
	};

	static void CreateInstance(Flags flags);
	static void DestroyInstance();
	static ReportFormatter& GetInstance();
	static Flags GetFlags();

	virtual ~ReportFormatter() = default;

	virtual void PushObject(std::wstring_view name) = 0;
	virtual void PushArray(std::wstring_view name, ArraySuffix suffix = ArraySuffix::SquareBrackets) = 0;
	virtual void PushArrayItem() = 0;
	virtual void PopScope() = 0;

	// Fields
	// Strings
	virtual void AddFieldString(std::wstring_view name, std::wstring_view value) = 0;
	// Booleans
	virtual void AddFieldBool(std::wstring_view name, bool value) = 0;
	// Integers
	virtual void AddFieldUint32(std::wstring_view name, uint32_t value, std::wstring_view unit = {}) = 0;
	virtual void AddFieldUint64(std::wstring_view name, uint64_t value, std::wstring_view unit = {}) = 0;
	virtual void AddFieldSize(std::wstring_view name, uint64_t value) = 0;
	virtual void AddFieldSizeKilobytes(std::wstring_view name, uint64_t value) = 0;
	virtual void AddFieldHex32(std::wstring_view name, uint32_t value) = 0;
	virtual void AddFieldInt32(std::wstring_view name, int32_t value, std::wstring_view unit = {}) = 0;
	// Floats
	virtual void AddFieldFloat(std::wstring_view name, float value, std::wstring_view unit = {}) = 0;
	// Enums
	virtual void AddFieldEnum(std::wstring_view name, uint32_t value, const EnumItem* enumItems) = 0;
	virtual void AddFieldEnumSigned(std::wstring_view name, int32_t value, const EnumItem* enumItems) = 0;
	virtual void AddEnumArray(std::wstring_view name, const uint32_t* values, size_t count, const EnumItem* enumItems) = 0;
	virtual void AddFieldFlags(std::wstring_view name, uint32_t value, const EnumItem* enumItems) = 0;
	// Binary data
	virtual void AddFieldHexBytes(std::wstring_view name, const void* data, size_t byteCount) = 0;
	// Custom types
	virtual void AddFieldVendorId(std::wstring_view name, uint32_t value) = 0;
	virtual void AddFieldSubsystemId(std::wstring_view name, uint32_t value) = 0;
	virtual void AddFieldMicrosoftVersion(std::wstring_view name, uint64_t value) = 0;
	virtual void AddFieldAMDVersion(std::wstring_view name, uint64_t value) = 0;
	virtual void AddFieldNvidiaImplementationID(std::wstring_view name, uint32_t architectureId, uint32_t implementationId, const EnumItem* architecturePlusImplementationIDEnum) = 0;
};

ReportFormatter::Flags operator&(ReportFormatter::Flags a, ReportFormatter::Flags b);
ReportFormatter::Flags operator|(ReportFormatter::Flags a, ReportFormatter::Flags b);
bool operator||(bool a, ReportFormatter::Flags b);
bool operator&&(bool a, ReportFormatter::Flags b);
bool operator!(ReportFormatter::Flags a);

class ReportScopeObject
{
public:
	ReportScopeObject(std::wstring_view name);
	~ReportScopeObject();
};

class ReportScopeArray
{
public:
	ReportScopeArray(std::wstring_view name, ReportFormatter::ArraySuffix suffix =  ReportFormatter::ArraySuffix::SquareBrackets);
	~ReportScopeArray();
};

class ReportScopeArrayItem
{
public:
	ReportScopeArrayItem();
	~ReportScopeArrayItem();
};

class ReportScopeObjectConditional
{
public:
	ReportScopeObjectConditional(std::wstring_view name);
	ReportScopeObjectConditional(bool enable, std::wstring_view name);
	~ReportScopeObjectConditional();
	void Enable();
private:
	std::wstring m_Name;
	bool m_Enabled = false;
};

class ReportScopeArrayConditional
{
public:
	ReportScopeArrayConditional(std::wstring_view name, ReportFormatter::ArraySuffix suffix = ReportFormatter::ArraySuffix::SquareBrackets);
	ReportScopeArrayConditional(bool enable, std::wstring_view name, ReportFormatter::ArraySuffix suffix = ReportFormatter::ArraySuffix::SquareBrackets);
	~ReportScopeArrayConditional();
	void Enable();
private:
	std::wstring m_Name;
	ReportFormatter::ArraySuffix m_Suffix;
	bool m_Enabled = false;
};

class ReportScopeArrayItemConditional
{
public:
	ReportScopeArrayItemConditional();
	ReportScopeArrayItemConditional(bool enable);
	~ReportScopeArrayItemConditional();
	void Enable();
private:
	bool m_Enabled = false;
};

bool IsOutputConsole();
bool IsOutputJson();
std::wstring_view OutputSpecificString(std::wstring_view consoleString, std::wstring_view jsonString);
std::string_view OutputSpecificString(std::string_view consoleString, std::string_view jsonString);
