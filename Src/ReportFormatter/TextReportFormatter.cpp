/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#include "TextReportFormatter.hpp"

#include "Enums.hpp"
#include "Printer.hpp"

TextReportFormatter::TextReportFormatter(FLAGS flags)
{
}

TextReportFormatter::~TextReportFormatter()
{
    assert(m_ScopeStack.empty());
    Printer::PrintNewLine();
}

void TextReportFormatter::PushObject(std::wstring_view name)
{
    assert(!name.empty());

    // Need to skip new line when outputting first object
    if(!m_SkipNewLine)
    {
        Printer::PrintNewLine();
        Printer::PrintNewLine();
    }
    else
    {
        m_SkipNewLine = false;
    }

    PrintIndent();

    Printer::PrintString(name);
    Printer::PrintString(L":");

    PrintDivider(name.size() + 1);

    m_ScopeStack.push({ .Type = ScopeType::Object });
    ++m_IndentLevel;
}

void TextReportFormatter::PushArray(std::wstring_view name, ARRAY_SUFFIX suffix /* = ArraySuffix::SquareBrackets */)
{
    assert(!name.empty());

    m_ScopeStack.push({ .ArrayName = std::wstring(name), .Type = ScopeType::Array, .Suffix = suffix });
}

void TextReportFormatter::PushArrayItem()
{
    assert(!m_ScopeStack.empty());

    ScopeInfo& arrayScope = m_ScopeStack.top();

    assert(arrayScope.Type == ScopeType::Array);

    Printer::PrintNewLine();
    Printer::PrintNewLine();
    PrintIndent();

    std::wstring header;
    switch(arrayScope.Suffix)
    {
    case ARRAY_SUFFIX_SQUARE_BRACKETS:
        header = std::format(L"{}[{}]:", arrayScope.ArrayName, arrayScope.ElementCount++);
        break;
    default:
        assert(0);
        [[fallthrough]];
    case ARRAY_SUFFIX_NONE:
        header = std::format(L"{} {}:", arrayScope.ArrayName, arrayScope.ElementCount++);
        break;
    }

    Printer::PrintString(header);

    PrintDivider(header.size());

    m_ScopeStack.push({ .ElementCount = 0, .Type = ScopeType::Object });
    ++m_IndentLevel;
}

void TextReportFormatter::PopScope()
{
    assert(!m_ScopeStack.empty());
    ScopeInfo scope = m_ScopeStack.top();
    m_ScopeStack.pop();
    if(scope.Type != ScopeType::Array)
    {
        --m_IndentLevel;
    }
}

void TextReportFormatter::AddFieldString(std::wstring_view name, std::wstring_view value)
{
    assert(!name.empty());
    assert(!value.empty());
    PushElement();
    Printer::PrintFormat(L"{} = {}", std::make_wformat_args(name, value));
}

void TextReportFormatter::AddFieldStringArray(std::wstring_view name, const std::vector<std::wstring>& value)
{
    assert(!name.empty());

    Printer::PrintNewLine();
    PushElement();
    Printer::PrintString(name);
    Printer::PrintString(L":");
    PrintDivider(name.size() + 1);

    ++m_IndentLevel;
    for(const auto& element : value)
    {
        Printer::PrintNewLine();
        PrintIndent();
        Printer::PrintString(element);
    }
    --m_IndentLevel;
}

void TextReportFormatter::AddFieldBool(std::wstring_view name, bool value)
{
    assert(!name.empty());
    PushElement();
    const wchar_t* boolStr = value ? L"TRUE" : L"FALSE";
    Printer::PrintFormat(L"{} = {}", std::make_wformat_args(name, boolStr));
}

void TextReportFormatter::AddFieldUint32(std::wstring_view name, uint32_t value, std::wstring_view unit /*= {}*/)
{
    assert(!name.empty());
    PushElement();
    Printer::PrintFormat(L"{} = {}", std::make_wformat_args(name, value));
}

void TextReportFormatter::AddFieldUint64(std::wstring_view name, uint64_t value, std::wstring_view unit /*= {}*/)
{
    assert(!name.empty());
    PushElement();
    if(unit.empty())
    {
        Printer::PrintFormat(L"{} = {}", std::make_wformat_args(name, value));
    }
    else
    {
        Printer::PrintFormat(L"{} = {} {}", std::make_wformat_args(name, value, unit));
    }
}

void TextReportFormatter::AddFieldSize(std::wstring_view name, uint64_t value)
{
    assert(!name.empty());
    PushElement();

    const wchar_t* units[] = { L"B", L"KiB", L"MiB", L"GiB", L"TiB" };

    int selectedUnit = 0;
    uint64_t scale = 1;
    for(; selectedUnit < ARRAYSIZE(units) - 1; ++selectedUnit)
    {
        if(value / scale < 1024)
        {
            break;
        }
        scale = scale << 10;
    }

    if(selectedUnit == 0)
    {
        Printer::PrintFormat(L"{} = {} {}", std::make_wformat_args(name, value, units[selectedUnit]));
        return;
    }
    else
    {
        double valueScaled = double(value) / scale;
        Printer::PrintFormat(
            L"{} = {:.2f} {} ({} B)", std::make_wformat_args(name, valueScaled, units[selectedUnit], value));
    }
}

void TextReportFormatter::AddFieldSizeKilobytes(std::wstring_view name, uint64_t value)
{
    AddFieldSize(name, value * 1024);
}

void TextReportFormatter::AddFieldHex32(std::wstring_view name, uint32_t value)
{
    assert(!name.empty());
    PushElement();
    Printer::PrintFormat(L"{} = 0x{:X}", std::make_wformat_args(name, value));
}

void TextReportFormatter::AddFieldInt32(std::wstring_view name, int32_t value, std::wstring_view unit /*= {}*/)
{
    assert(!name.empty());
    PushElement();
    if(unit.empty())
    {
        Printer::PrintFormat(L"{} = {}", std::make_wformat_args(name, value));
    }
    else
    {
        Printer::PrintFormat(L"{} = {} {}", std::make_wformat_args(name, value, unit));
    }
}

void TextReportFormatter::AddFieldFloat(std::wstring_view name, float value, std::wstring_view unit /*= {}*/)
{
    assert(!name.empty());
    PushElement();
    if(unit.empty())
    {
        Printer::PrintFormat(L"{} = {}", std::make_wformat_args(name, value));
    }
    else
    {
        Printer::PrintFormat(L"{} = {} {}", std::make_wformat_args(name, value, unit));
    }
}

void TextReportFormatter::AddFieldEnum(std::wstring_view name, uint32_t value, const EnumItem* enumItems)
{
    assert(!name.empty());
    PushElement();
    const wchar_t* enumItemName = FindEnumItemName(value, enumItems);
    if(enumItemName != nullptr)
    {
        Printer::PrintFormat(L"{} = {} (0x{:X})", std::make_wformat_args(name, enumItemName, value));
    }
    else
    {
        Printer::PrintFormat(L"{} = 0x{:X}", std::make_wformat_args(name, value));
    }
}

void TextReportFormatter::AddFieldEnumSigned(std::wstring_view name, int32_t value, const EnumItem* enumItems)
{
    assert(!name.empty());
    PushElement();
    const wchar_t* enumItemName = FindEnumItemName(value, enumItems);
    if(enumItemName != nullptr)
    {
        Printer::PrintFormat(L"{} = {} ({})", std::make_wformat_args(name, enumItemName, value));
    }
    else
    {
        Printer::PrintFormat(L"{} = {}", std::make_wformat_args(name, value));
    }
}

void TextReportFormatter::AddEnumArray(
    std::wstring_view name, const uint32_t* values, size_t count, const EnumItem* enumItems)
{
    assert(!name.empty());

    Printer::PrintNewLine();
    PushElement();
    Printer::PrintString(name);
    Printer::PrintString(L":");
    PrintDivider(name.size() + 1);

    ++m_IndentLevel;
    for(size_t i = 0; i < count; ++i)
    {
        Printer::PrintNewLine();
        PrintIndent();
        const wchar_t* enumItemName = FindEnumItemName(values[i], enumItems);
        if(enumItemName != nullptr)
        {
            Printer::PrintFormat(L"[{}] = {} (0x{:X})", std::make_wformat_args(i, enumItemName, values[i]));
        }
        else
        {
            Printer::PrintFormat(L"[{}] = 0x{:X}", std::make_wformat_args(i, values[i]));
        }
    }
    --m_IndentLevel;
}

void TextReportFormatter::AddFieldFlags(std::wstring_view name, uint32_t value, const EnumItem* enumItems)
{
    assert(!name.empty());
    PushElement();
    Printer::PrintFormat(L"{} = 0x{:X}", std::make_wformat_args(name, value));

    ++m_IndentLevel;
    size_t zeroFlagIndex = SIZE_MAX;
    for(size_t i = 0; enumItems[i].m_Name != nullptr; ++i)
    {
        if(enumItems[i].m_Value == 0)
        {
            zeroFlagIndex = i;
        }
        else
        {
            if((value & enumItems[i].m_Value) != 0)
            {
                Printer::PrintNewLine();
                PrintIndent();
                Printer::PrintString(enumItems[i].m_Name);
            }
        }
    }
    if(value == 0 && zeroFlagIndex != SIZE_MAX)
    {
        Printer::PrintNewLine();
        PrintIndent();
        Printer::PrintString(enumItems[zeroFlagIndex].m_Name);
    }
    --m_IndentLevel;
}

void TextReportFormatter::AddFieldHexBytes(std::wstring_view name, const void* data, size_t byteCount)
{
    std::wstring valStr;
    for(size_t i = 0; i < byteCount; ++i)
    {
        valStr += std::format(L"{:02X}", *((const uint8_t*)data + i));
    }
    AddFieldString(name, valStr);
}

void TextReportFormatter::AddFieldVendorId(std::wstring_view name, uint32_t value)
{
    assert(!name.empty());

    if(value < 0xFFFF)
    {
        // PCI ID codepath
        AddFieldEnum(name, value, Enum_VendorId);
    }
    else
    {
        // Either ACPI ID or invalid codepath

        // ACPIID is NOT null terminated
        char ACPIID[4] = { char(value & 0xFF), char((value >> 8) & 0xFF), char((value >> 16) & 0xFF),
            char((value >> 24) & 0xFF) };
        bool valid = true;
        for(const char& charByte : ACPIID)
        {
            bool isDigit = charByte >= '0' && charByte <= '9';
            bool isUppercaseLetter = charByte >= 'A' && charByte <= 'Z';
            if(!isDigit && !isUppercaseLetter)
            {
                valid = false;
                break;
            }
        }

        PushElement();
        if(valid)
        {
            // ACPIIDString is null terminated
            std::wstring ACPIIDString(ACPIID, ACPIID + 4);
            const wchar_t* enumItemName = FindEnumItemName(value, Enum_VendorId);
            if(enumItemName == nullptr)
            {
                enumItemName = L"Unknown";
            }

            Printer::PrintFormat(
                L"{} = {} \"{}\" (0x{:X})", std::make_wformat_args(name, enumItemName, ACPIIDString, value));
        }
        else
        {
            Printer::PrintFormat(L"{} = Invalid (0x{:X})", std::make_wformat_args(name, value));
        }
    }
}

void TextReportFormatter::AddFieldSubsystemId(std::wstring_view name, uint32_t value)
{
    assert(!name.empty());
    PushElement();

    const wchar_t* enumItemName = FindEnumItemName(value & 0xFFFF, Enum_SubsystemVendorId);
    if(enumItemName != nullptr)
    {
        Printer::PrintFormat(L"{} = {} (0x{:X})", std::make_wformat_args(name, enumItemName, value));
    }
    else
    {
        Printer::PrintFormat(L"{} = 0x{:X}", std::make_wformat_args(name, value));
    }
}

void TextReportFormatter::AddFieldMicrosoftVersion(std::wstring_view name, uint64_t value)
{
    assert(!name.empty());
    PushElement();
    uint64_t major = value >> 48;
    uint64_t minor = (value >> 32) & 0xFFFF;
    uint64_t build = (value >> 16) & 0xFFFF;
    uint64_t revision = value & 0xFFFF;
    Printer::PrintFormat(L"{} = {}.{}.{}.{}", std::make_wformat_args(name, major, minor, build, revision));
}

void TextReportFormatter::AddFieldAMDVersion(std::wstring_view name, uint64_t value)
{
    assert(!name.empty());
    PushElement();
    uint64_t major = value >> 22;
    uint64_t minor = (value >> 12) & 0b11'1111'1111;
    uint64_t patch = value & 0b1111'1111'1111;
    Printer::PrintFormat(L"{} = {}.{}.{}", std::make_wformat_args(name, major, minor, patch));
}

void TextReportFormatter::AddFieldNvidiaImplementationID(std::wstring_view name, uint32_t architectureId,
    uint32_t implementationId, const EnumItem* architecturePlusImplementationIDEnum)
{
    // Prints only implementationId as the numerical value, but searches enum
    // using architectureId + implementationId.

    assert(!name.empty());
    PushElement();

    const wchar_t* enumItemName =
        FindEnumItemName(architectureId + implementationId, architecturePlusImplementationIDEnum);
    if(enumItemName == nullptr)
    {
        enumItemName = L"Unknown";
    }

    Printer::PrintFormat(L"{} = {} (0x{:X})", std::make_wformat_args(name, enumItemName, implementationId));
}

void TextReportFormatter::PrintIndent() const
{
    int effectiveIndentLevel = std::max(m_IndentLevel - 1, 0);
    Printer::PrintString(std::wstring(INDENT_SIZE * effectiveIndentLevel, INDENT_CHAR));
}

void TextReportFormatter::PushElement()
{
    m_ScopeStack.top().ElementCount++;
    Printer::PrintNewLine();
    PrintIndent();
}

void TextReportFormatter::PrintDivider(size_t size)
{
    Printer::PrintNewLine();
    PrintIndent();
    wchar_t dividerChar;

    switch(m_IndentLevel)
    {
    case 0:
        dividerChar = L'=';
        break;
    default:
        dividerChar = L'-';
        break;
    }

    Printer::PrintString(std::wstring(size, dividerChar));
}
