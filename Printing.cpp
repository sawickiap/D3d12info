#include "pch.hpp"
#include "Printing.hpp"
#include "Json.hpp"
#include "Utils.hpp"

////////////////////////////////////////////////////////////////////////////////
// PRIVATE

////////////////////////////////////////////////////////////////////////////////
// PUBLIC

bool g_UseJson = false;
uint32_t g_Indent = 0;

void PrintEmptyLine()
{
    assert(!g_UseJson);
    wprintf(L"\n");
}

void PrintHeader(const wchar_t* s, uint8_t headerLevel)
{
    assert(!g_UseJson);
    assert(headerLevel < 2);

    const size_t len = wcslen(s);
    assert(len > 0 && len < 80);
    PrintIndent();
    wprintf(L"%s:\n", s);
    static const wchar_t* underline1 = L"================================================================================";
    static const wchar_t* underline2 = L"--------------------------------------------------------------------------------";
    const wchar_t* const currUnderline = headerLevel == 0 ? underline1 : underline2;
    PrintIndent();
    wprintf(L"%s\n", currUnderline + 80 - len - 1);
}

void PrintIndent()
{
    assert(!g_UseJson);
    if(g_Indent == 0)
        return;
    static const wchar_t* maxIndentStr = L"                                                                ";
    const uint32_t offset = 64u - (g_Indent * 4u);
    assert(offset < wcslen(maxIndentStr));
    wprintf(L"%s", maxIndentStr + offset);
}

void PrintName(const wchar_t* name)
{
    assert(!g_UseJson);
    wprintf(L"%s", name);
}

void Print_string(const wchar_t* name, const wchar_t* value)
{
    if(g_UseJson)
    {
        Json::WriteString(name);
        Json::WriteString(value);
    }
    else
    {
        PrintIndent();
        PrintName(name);
        wprintf(L" = %s\n", value);
    }
}

void PrintStructBegin(const wchar_t* structName)
{
    if(g_UseJson)
    {
        Json::WriteString(structName);
        Json::BeginObject();
    }
    else
    {
        PrintHeader(structName, 1);
        ++g_Indent;
    }
}

void PrintStructEnd()
{
    if(g_UseJson)
    {
        Json::EndObject();
    }
    else
    {
        --g_Indent;
        PrintEmptyLine();
    }
}

void Print_BOOL(const wchar_t* name, BOOL value)
{
    if(g_UseJson)
    {
        Json::WriteString(name);
        Json::WriteBool(value != FALSE);
    }
    else
    {
        PrintIndent();
        PrintName(name);
        wprintf(L" = %s\n", value ? L"TRUE" : L"FALSE");
    }
}

void Print_uint32(const wchar_t* name, uint32_t value)
{
    if(g_UseJson)
    {
        Json::WriteString(name);
        Json::WriteNumber(value);
    }
    else
    {
        PrintIndent();
        PrintName(name);
        wprintf(L" = %u\n", value);
    }
}

void Print_uint64(const wchar_t* name, uint64_t value)
{
    if(g_UseJson)
    {
        Json::WriteString(name);
        Json::WriteNumber(value);
    }
    else
    {
        PrintIndent();
        PrintName(name);
        wprintf(L" = %llu\n", value);
    }
}

void Print_size(const wchar_t* name, uint64_t value)
{
    if(g_UseJson)
    {
        Json::WriteString(name);
        Json::WriteNumber(value);
    }
    else
    {
        PrintIndent();
        PrintName(name);
        if(value == 0)
            wprintf(L" = 0\n");
        else if(value < 1024)
            wprintf(L" = %zu (0x%llx)\n", value, value);
        else
        {
            wstring sizeStr = SizeToStr(value);
            wprintf(L" = %zu (0x%llx) (%s)\n", value, value, sizeStr.c_str());
        }
    }
}

void Print_hex32(const wchar_t* name, uint32_t value)
{
    if(g_UseJson)
    {
        Json::WriteString(name);
        Json::WriteNumber(value);
    }
    else
    {
        PrintIndent();
        PrintName(name);
        wprintf(L" = 0x%X\n", value);
    }
}

void PrintEnum(const wchar_t* name, uint32_t value, const EnumItem* enumItems)
{
    if(g_UseJson)
    {
        Json::WriteString(name);
        Json::WriteNumber(value);
    }
    else
    {
        PrintIndent();
        PrintName(name);
        const wchar_t* enumItemName = FindEnumItemName(value, enumItems);
        if(enumItemName != nullptr)
            wprintf(L" = %s (0x%X)\n", enumItemName, value);
        else
            wprintf(L" = 0x%X\n", value);
    }
}

void PrintFlags(const wchar_t* name, uint32_t value, const EnumItem* enumItems)
{
    if(g_UseJson)
    {
        Json::WriteString(name);
        Json::WriteNumber(value);
    }
    else
    {
        PrintIndent();
        PrintName(name);
        wprintf(L" = 0x%X\n", value);

        ++g_Indent;
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
                    PrintIndent();
                    wprintf(L"%s\n", enumItems[i].m_Name);
                }
            }
        }
        if(value == 0 && zeroFlagIndex != SIZE_MAX)
        {
            PrintIndent();
            wprintf(L"%s\n", enumItems[zeroFlagIndex].m_Name);
        }
        --g_Indent;
    }
}
