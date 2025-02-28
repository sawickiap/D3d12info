/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#include "JSONReportFormatter.hpp"

#include "Printer.hpp"

JSONReportFormatter::JSONReportFormatter(FLAGS flags)
    : m_PrettyPrint((flags & FLAGS::FLAG_JSON_PRETTY_PRINT) != FLAGS::FLAG_NONE)
{
    Printer::PrintString(L"{");
    m_ScopeStack.push({ .ElementCount = 0, .Type = ScopeType::Object });
}

JSONReportFormatter::~JSONReportFormatter()
{
    PopScope();
    PrintNewLine();
    assert(m_ScopeStack.empty());
}

void JSONReportFormatter::PushObject(std::wstring_view name)
{
    assert(!name.empty());

    PushNewElement();

    Printer::PrintFormat(m_PrettyPrint ? L"\"{}\": {{" : L"\"{}\":{{", std::make_wformat_args(name));

    m_ScopeStack.push({ .ElementCount = 0, .Type = ScopeType::Object });
}

void JSONReportFormatter::PushArray(std::wstring_view name, ARRAY_SUFFIX suffix /* = ArraySuffix::SquareBrackets */)
{
    assert(!name.empty());

    PushNewElement();

    Printer::PrintFormat(m_PrettyPrint ? L"\"{}\": [" : L"\"{}\":[", std::make_wformat_args(name));

    m_ScopeStack.push({ .ElementCount = 0, .Type = ScopeType::Array });
}

void JSONReportFormatter::PushArrayItem()
{
    assert(!m_ScopeStack.empty());
    assert(m_ScopeStack.top().Type == ScopeType::Array);

    PushNewElement();

    Printer::PrintString(L"{");

    m_ScopeStack.push({ .ElementCount = 0, .Type = ScopeType::Object });
}

void JSONReportFormatter::PopScope()
{
    assert(!m_ScopeStack.empty());

    ScopeInfo scope = m_ScopeStack.top();
    m_ScopeStack.pop();

    if (scope.ElementCount != 0)
    {
        PrintNewLine();
        PrintIndent();
    }

    Printer::PrintString(scope.Type == ScopeType::Object ? L"}" : L"]");
}

void JSONReportFormatter::AddFieldString(std::wstring_view name, std::wstring_view value)
{
    assert(!name.empty());
    assert(!value.empty());
    PushNewElement();
    Printer::PrintFormat(m_PrettyPrint ? L"\"{}\": \"{}\"" : L"\"{}\":\"{}\"", std::make_wformat_args(name, value));
}

void JSONReportFormatter::AddFieldStringArray(std::wstring_view name, const std::vector<std::wstring>& value)
{
    assert(!name.empty());
    PushNewElement();
    Printer::PrintFormat(m_PrettyPrint ? L"\"{}\": [" : L"\"{}\":[", std::make_wformat_args(name));
    for (size_t i = 0; i < value.size(); ++i)
    {
        if (i > 0)
        {
            Printer::PrintString(L",");
        }
        PrintNewLine();
        PrintIndent(1);
        Printer::PrintFormat(L"\"{}\"", std::make_wformat_args(value[i]));
    }
    PrintNewLine();
    PrintIndent();
    Printer::PrintString(L"]");
}

void JSONReportFormatter::AddFieldBool(std::wstring_view name, bool value)
{
    assert(!name.empty());
    PushNewElement();
    const wchar_t* boolStr = value ? L"true" : L"false";
    Printer::PrintFormat(m_PrettyPrint ? L"\"{}\": {}" : L"\"{}\":{}", std::make_wformat_args(name, boolStr));
}

void JSONReportFormatter::AddFieldUint32(std::wstring_view name, uint32_t value, std::wstring_view unit /* = {}*/)
{
    assert(!name.empty());
    PushNewElement();
    Printer::PrintFormat(m_PrettyPrint ? L"\"{}\": {}" : L"\"{}\":{}", std::make_wformat_args(name, value));
}

void JSONReportFormatter::AddFieldUint64(std::wstring_view name, uint64_t value, std::wstring_view unit /* = {}*/)
{
    assert(!name.empty());
    PushNewElement();
    Printer::PrintFormat(m_PrettyPrint ? L"\"{}\": {}" : L"\"{}\":{}", std::make_wformat_args(name, value));
}

void JSONReportFormatter::AddFieldSize(std::wstring_view name, uint64_t value)
{
    AddFieldUint64(name, value);
}

void JSONReportFormatter::AddFieldSizeKilobytes(std::wstring_view name, uint64_t value)
{
    AddFieldUint64(name, value);
}

void JSONReportFormatter::AddFieldHex32(std::wstring_view name, uint32_t value)
{
    AddFieldUint32(name, value);
}

void JSONReportFormatter::AddFieldInt32(std::wstring_view name, int32_t value, std::wstring_view unit /* = {}*/)
{
    assert(!name.empty());
    PushNewElement();
    Printer::PrintFormat(m_PrettyPrint ? L"\"{}\": {}" : L"\"{}\":{}", std::make_wformat_args(name, value));
}

void JSONReportFormatter::AddFieldFloat(std::wstring_view name, float value, std::wstring_view unit /* = {}*/)
{
    assert(!name.empty());
    PushNewElement();
    Printer::PrintFormat(m_PrettyPrint ? L"\"{}\": {}" : L"\"{}\":{}", std::make_wformat_args(name, value));
}

void JSONReportFormatter::AddFieldEnum(std::wstring_view name, uint32_t value, const EnumItem* enumItems)
{
    AddFieldUint32(name, value);
}

void JSONReportFormatter::AddFieldEnumSigned(std::wstring_view name, int32_t value, const EnumItem* enumItems)
{
    AddFieldInt32(name, value);
}

void JSONReportFormatter::AddEnumArray(std::wstring_view name, const uint32_t* values, size_t count,
                                       const EnumItem* enumItems)
{
    assert(!name.empty());
    PushNewElement();
    Printer::PrintFormat(m_PrettyPrint ? L"\"{}\": [" : L"\"{}\":[", std::make_wformat_args(name));
    for (size_t i = 0; i < count; ++i)
    {
        if (i > 0)
        {
            Printer::PrintString(L",");
        }
        PrintNewLine();
        PrintIndent(1);
        Printer::PrintFormat(L"{}", std::make_wformat_args(values[i]));
    }
    PrintNewLine();
    PrintIndent();
    Printer::PrintString(L"]");
}

void JSONReportFormatter::AddFieldFlags(std::wstring_view name, uint32_t value, const EnumItem* enumItems)
{
    AddFieldUint32(name, value);
}

void JSONReportFormatter::AddFieldHexBytes(std::wstring_view name, const void* data, size_t byteCount)
{
    std::wstring valStr;
    for (size_t i = 0; i < byteCount; ++i)
    {
        valStr += std::format(L"{:02X}", *((const uint8_t*)data + i));
    }
    AddFieldString(name, valStr);
}

void JSONReportFormatter::AddFieldVendorId(std::wstring_view name, uint32_t value)
{
    AddFieldUint32(name, value);
}

void JSONReportFormatter::AddFieldSubsystemId(std::wstring_view name, uint32_t value)
{
    AddFieldUint32(name, value);
}

void JSONReportFormatter::AddFieldMicrosoftVersion(std::wstring_view name, uint64_t value)
{
    AddFieldUint64(name, value);
}

void JSONReportFormatter::AddFieldAMDVersion(std::wstring_view name, uint64_t value)
{
    AddFieldUint64(name, value);
}

void JSONReportFormatter::AddFieldNvidiaImplementationID(std::wstring_view name, uint32_t architectureId,
                                                         uint32_t implementationId,
                                                         const EnumItem* architecturePlusImplementationIDEnum)
{
    AddFieldUint32(name, implementationId);
}

void JSONReportFormatter::PushNewElement()
{
    if (m_ScopeStack.top().ElementCount > 0)
    {
        Printer::PrintString(L",");
    }
    m_ScopeStack.top().ElementCount++;
    PrintNewLine();
    PrintIndent();
}

void JSONReportFormatter::PrintIndent(size_t additionalIndentation /*= 0*/)
{
    if (!m_PrettyPrint)
    {
        return;
    }

    Printer::PrintString(std::wstring((m_ScopeStack.size() + additionalIndentation) * INDENT_SIZE, INDENT_CHAR));
}

void JSONReportFormatter::PrintNewLine()
{
    if (m_PrettyPrint)
    {
        Printer::PrintNewLine();
    }
}
