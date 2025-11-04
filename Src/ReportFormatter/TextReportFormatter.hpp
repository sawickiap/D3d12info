/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/

#pragma once

#include "ReportFormatter.hpp"

class TextReportFormatter final : public ReportFormatter
{
public:
    TextReportFormatter(FLAGS flags);
    ~TextReportFormatter();

    void PushObject(std::wstring_view name) final;
    void PushArray(std::wstring_view name, ARRAY_SUFFIX suffix = ARRAY_SUFFIX_SQUARE_BRACKETS);
    void PushArrayItem() final;
    void PopScope() final;

    void AddFieldString(std::wstring_view name, std::wstring_view value) final;
    void AddFieldStringArray(std::wstring_view name, const std::vector<std::wstring>& value) final;
    void AddFieldBool(std::wstring_view name, bool value);
    void AddFieldUint32(std::wstring_view name, uint32_t value, std::wstring_view unit = {}) final;
    void AddFieldUint64(std::wstring_view name, uint64_t value, std::wstring_view unit = {}) final;
    void AddFieldSize(std::wstring_view name, uint64_t value) final;
    void AddFieldSizeKilobytes(std::wstring_view name, uint64_t value) final;
    void AddFieldHex32(std::wstring_view name, uint32_t value) final;
    void AddFieldInt32(std::wstring_view name, int32_t value, std::wstring_view unit = {}) final;
    void AddFieldFloat(std::wstring_view name, float value, std::wstring_view unit = {}) final;
    void AddFieldEnum(std::wstring_view name, uint32_t value, const EnumItem* enumItems) final;
    void AddFieldEnumSigned(std::wstring_view name, int32_t value, const EnumItem* enumItems) final;
    void AddEnumArray(std::wstring_view name, const uint32_t* values, size_t count, const EnumItem* enumItems) final;
    void AddFieldFlags(std::wstring_view name, uint32_t value, const EnumItem* enumItems) final;
    void AddFieldHexBytes(std::wstring_view name, const void* data, size_t byteCount) final;
    void AddFieldVendorId(std::wstring_view name, uint32_t value) final;
    void AddFieldSubsystemId(std::wstring_view name, uint32_t value) final;
    void AddFieldMicrosoftVersion(std::wstring_view name, uint64_t value) final;
    void AddFieldAMDVersion(std::wstring_view name, uint64_t value) final;
    void AddFieldNvidiaImplementationID(std::wstring_view name, uint32_t architectureId, uint32_t implementationId,
        const EnumItem* architecturePlusImplementationIDEnum) final;

private:
    static constexpr size_t INDENT_SIZE = 4;
    static constexpr wchar_t INDENT_CHAR = L' ';

    enum class ScopeType
    {
        Object,
        Array
    };
    struct ScopeInfo
    {
        std::wstring ArrayName;
        size_t ElementCount = 0;
        ScopeType Type;
        ARRAY_SUFFIX Suffix;
    };
    std::stack<ScopeInfo> m_ScopeStack;
    int m_IndentLevel = 0;
    bool m_SkipNewLine = true;

    void PrintIndent() const;
    void PushElement();
    void PrintDivider(size_t size);
};
