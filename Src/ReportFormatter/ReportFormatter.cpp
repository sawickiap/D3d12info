/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#include "ReportFormatter.hpp"

#include "JSONReportFormatter.hpp"
#include "ConsoleReportFormatter.hpp"

static ReportFormatter* s_Instance = nullptr;
static ReportFormatter::Flags s_Flags = ReportFormatter::Flags::None;

void ReportFormatter::CreateInstance(Flags flags)
{
	assert(s_Instance == nullptr);
	if ((flags & Flags::UseJson) != Flags::None)
	{
		s_Instance = new JSONReportFormatter(flags);
	}
	else
	{
		s_Instance = new ConsoleReportFormatter(flags);
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

ReportFormatter::Flags ReportFormatter::GetFlags()
{
	assert(s_Instance != nullptr);
	return s_Flags;
}

ReportFormatter::Flags operator&(ReportFormatter::Flags a, ReportFormatter::Flags b)
{
	return static_cast<ReportFormatter::Flags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

ReportFormatter::Flags operator|(ReportFormatter::Flags a, ReportFormatter::Flags b)
{
	return static_cast<ReportFormatter::Flags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

bool operator||(bool a, ReportFormatter::Flags b)
{
	return a || (b != ReportFormatter::Flags::None);
}

bool operator&&(bool a, ReportFormatter::Flags b)
{
	return a && (b != ReportFormatter::Flags::None);
}

bool operator!(ReportFormatter::Flags a)
{
	return a == ReportFormatter::Flags::None;
}

ReportScopeObject::ReportScopeObject(std::wstring_view name)
{
	ReportFormatter::GetInstance().PushObject(name);
}

ReportScopeObject::~ReportScopeObject()
{
	ReportFormatter::GetInstance().PopScope();
}

ReportScopeArray::ReportScopeArray(std::wstring_view name, ReportFormatter::ArraySuffix suffix /*= ReportFormatter::ArraySuffix::SquareBrackets*/)
{
	ReportFormatter::GetInstance().PushArray(name, suffix);
}

ReportScopeArray::~ReportScopeArray()
{
	ReportFormatter::GetInstance().PopScope();
}

ReportScopeArrayItem::ReportScopeArrayItem()
{
	ReportFormatter::GetInstance().PushArrayItem();
}

ReportScopeArrayItem::~ReportScopeArrayItem()
{
	ReportFormatter::GetInstance().PopScope();
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

ReportScopeArrayConditional::ReportScopeArrayConditional(std::wstring_view name, ReportFormatter::ArraySuffix suffix /*= ReportFormatter::ArraySuffix::SquareBrackets*/)
	: m_Name(name)
	, m_Suffix(suffix)
{
}

ReportScopeArrayConditional::ReportScopeArrayConditional(bool enable, std::wstring_view name, ReportFormatter::ArraySuffix suffix /*= ReportFormatter::ArraySuffix::SquareBrackets*/)
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

bool IsOutputConsole()
{
	return (ReportFormatter::GetFlags() & ReportFormatter::Flags::UseJson) == ReportFormatter::Flags::None;
}

bool IsOutputJson()
{
	return !IsOutputConsole();
}

std::wstring_view OutputSpecificString(std::wstring_view consoleString, std::wstring_view jsonString)
{
	if ((ReportFormatter::GetFlags() & ReportFormatter::Flags::UseJson) != ReportFormatter::Flags::None)
	{
		return jsonString;
	}
	else
	{
		return consoleString;
	}
}

std::string_view OutputSpecificString(std::string_view consoleString, std::string_view jsonString)
{
	if ((ReportFormatter::GetFlags() & ReportFormatter::Flags::UseJson) != ReportFormatter::Flags::None)
	{
		return jsonString;
	}
	else
	{
		return consoleString;
	}
}
