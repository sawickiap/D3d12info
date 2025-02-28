/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#include "ReportFormatter.hpp"

#include "JSONReportFormatter.hpp"
#include "TextReportFormatter.hpp"

static ReportFormatter* s_Instance = nullptr;
static ReportFormatter::FLAGS s_Flags = ReportFormatter::FLAGS::FLAG_NONE;

void ReportFormatter::CreateInstance(FLAGS flags)
{
    assert(s_Instance == nullptr);
    if ((flags & FLAGS::FLAG_JSON) != FLAGS::FLAG_NONE)
    {
        s_Instance = new JSONReportFormatter(flags);
    }
    else
    {
        s_Instance = new TextReportFormatter(flags);
    }
    s_Flags = flags;
}

void ReportFormatter::DestroyInstance()
{
    assert(s_Instance != nullptr);
    delete s_Instance;
    s_Instance = nullptr;
}

ReportFormatter& ReportFormatter::GetInstance()
{
    assert(s_Instance != nullptr);
    return *s_Instance;
}

ReportFormatter::FLAGS ReportFormatter::GetFlags()
{
    assert(s_Instance != nullptr);
    return s_Flags;
}

ReportFormatter::FLAGS& operator|=(ReportFormatter::FLAGS& lhs, ReportFormatter::FLAGS rhs)
{
    lhs = static_cast<ReportFormatter::FLAGS>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    return lhs;
}

ReportScopeObjectConditional::ReportScopeObjectConditional(std::wstring_view name)
    : m_Name(name)
{
}

ReportScopeObjectConditional::ReportScopeObjectConditional(bool enable, std::wstring_view name)
    : ReportScopeObjectConditional(name)
{
    if (enable)
    {
        Enable();
    }
}

ReportScopeObjectConditional::~ReportScopeObjectConditional()
{
    if (m_Enabled)
    {
        ReportFormatter::GetInstance().PopScope();
    }
}

void ReportScopeObjectConditional::Enable()
{
    if (!m_Enabled)
    {
        ReportFormatter::GetInstance().PushObject(m_Name);
        m_Enabled = true;
    }
}

ReportScopeArrayConditional::ReportScopeArrayConditional(
    std::wstring_view name, ReportFormatter::ARRAY_SUFFIX suffix /*= ReportFormatter::SquareBrackets*/)
    : m_Name(name)
    , m_Suffix(suffix)
{
}

ReportScopeArrayConditional::ReportScopeArrayConditional(
    bool enable, std::wstring_view name, ReportFormatter::ARRAY_SUFFIX suffix /*= ReportFormatter::SquareBrackets*/)
    : ReportScopeArrayConditional(name, suffix)
{
    if (enable)
    {
        Enable();
    }
}

ReportScopeArrayConditional::~ReportScopeArrayConditional()
{
    if (m_Enabled)
    {
        ReportFormatter::GetInstance().PopScope();
    }
}

void ReportScopeArrayConditional::Enable()
{
    if (!m_Enabled)
    {
        ReportFormatter::GetInstance().PushArray(m_Name, m_Suffix);
        m_Enabled = true;
    }
}

ReportScopeArrayItemConditional::ReportScopeArrayItemConditional()
{
}

ReportScopeArrayItemConditional::ReportScopeArrayItemConditional(bool enable)
{
    if (enable)
    {
        Enable();
    }
}

ReportScopeArrayItemConditional::~ReportScopeArrayItemConditional()
{
    if (m_Enabled)
    {
        ReportFormatter::GetInstance().PopScope();
    }
}

void ReportScopeArrayItemConditional::Enable()
{
    if (!m_Enabled)
    {
        ReportFormatter::GetInstance().PushArrayItem();
        m_Enabled = true;
    }
}

bool IsTextOutput()
{
    return (ReportFormatter::GetFlags() & ReportFormatter::FLAGS::FLAG_JSON) == ReportFormatter::FLAGS::FLAG_NONE;
}

bool IsJsonOutput()
{
    return !IsTextOutput();
}

std::wstring_view SelectString(std::wstring_view textString, std::wstring_view jsonString)
{
    if (IsJsonOutput())
    {
        return jsonString;
    }
    else
    {
        return textString;
    }
}

std::string_view SelectString(std::string_view textString, std::string_view jsonString)
{
    if (IsJsonOutput())
    {
        return jsonString;
    }
    else
    {
        return textString;
    }
}
