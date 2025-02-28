/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/

#pragma once

struct EnumItem;

class ReportFormatter
{
public:
    enum FLAGS : uint32_t
    {
        FLAG_NONE = 0,
        FLAG_JSON = 1 << 0,
        FLAG_JSON_PRETTY_PRINT = 1 << 1
    };

    enum ARRAY_SUFFIX
    {
        ARRAY_SUFFIX_SQUARE_BRACKETS,
        ARRAY_SUFFIX_NONE
    };

    static void CreateInstance(FLAGS flags);
    static void DestroyInstance();
    static ReportFormatter& GetInstance();
    static FLAGS GetFlags();

    virtual ~ReportFormatter() = default;

    virtual void PushObject(std::wstring_view name) = 0;
    virtual void PushArray(std::wstring_view name, ARRAY_SUFFIX suffix = ARRAY_SUFFIX_SQUARE_BRACKETS) = 0;
    virtual void PushArrayItem() = 0;
    virtual void PopScope() = 0;

    // Fields
    // Strings
    virtual void AddFieldString(std::wstring_view name, std::wstring_view value) = 0;
    virtual void AddFieldStringArray(std::wstring_view name, const std::vector<std::wstring>& value) = 0;
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
    virtual void AddEnumArray(std::wstring_view name, const uint32_t* values, size_t count,
                              const EnumItem* enumItems) = 0;
    virtual void AddFieldFlags(std::wstring_view name, uint32_t value, const EnumItem* enumItems) = 0;
    // Binary data
    virtual void AddFieldHexBytes(std::wstring_view name, const void* data, size_t byteCount) = 0;
    // Custom types
    virtual void AddFieldVendorId(std::wstring_view name, uint32_t value) = 0;
    virtual void AddFieldSubsystemId(std::wstring_view name, uint32_t value) = 0;
    virtual void AddFieldMicrosoftVersion(std::wstring_view name, uint64_t value) = 0;
    virtual void AddFieldAMDVersion(std::wstring_view name, uint64_t value) = 0;
    virtual void AddFieldNvidiaImplementationID(std::wstring_view name, uint32_t architectureId,
                                                uint32_t implementationId,
                                                const EnumItem* architecturePlusImplementationIDEnum) = 0;
};

ReportFormatter::FLAGS& operator|=(ReportFormatter::FLAGS& lhs, ReportFormatter::FLAGS rhs);

class ReportFormatterScope
{
public:
    ReportFormatterScope(ReportFormatter::FLAGS flags)
    {
        ReportFormatter::CreateInstance(flags);
    }

    ~ReportFormatterScope()
    {
        ReportFormatter::DestroyInstance();
    }
};

class ReportScopeObject
{
public:
    ReportScopeObject(std::wstring_view name)
    {
        ReportFormatter::GetInstance().PushObject(name);
    }

    ~ReportScopeObject()
    {
        ReportFormatter::GetInstance().PopScope();
    }
};

class ReportScopeArray
{
public:
    ReportScopeArray(std::wstring_view name,
                     ReportFormatter::ARRAY_SUFFIX suffix = ReportFormatter::ARRAY_SUFFIX_SQUARE_BRACKETS)
    {
        ReportFormatter::GetInstance().PushArray(name, suffix);
    }

    ~ReportScopeArray()
    {
        ReportFormatter::GetInstance().PopScope();
    }
};

class ReportScopeArrayItem
{
public:
    ReportScopeArrayItem()
    {
        ReportFormatter::GetInstance().PushArrayItem();
    }

    ~ReportScopeArrayItem()
    {
        ReportFormatter::GetInstance().PopScope();
    }
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
    ReportScopeArrayConditional(std::wstring_view name,
                                ReportFormatter::ARRAY_SUFFIX suffix = ReportFormatter::ARRAY_SUFFIX_SQUARE_BRACKETS);
    ReportScopeArrayConditional(bool enable, std::wstring_view name,
                                ReportFormatter::ARRAY_SUFFIX suffix = ReportFormatter::ARRAY_SUFFIX_SQUARE_BRACKETS);
    ~ReportScopeArrayConditional();
    void Enable();

private:
    std::wstring m_Name;
    ReportFormatter::ARRAY_SUFFIX m_Suffix;
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

bool IsTextOutput();
bool IsJsonOutput();
std::wstring_view SelectString(std::wstring_view textString, std::wstring_view jsonString);
std::string_view SelectString(std::string_view textString, std::string_view jsonString);
