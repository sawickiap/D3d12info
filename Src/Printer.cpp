/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#include "Printer.hpp"

bool Printer::m_IsInitialized = false;
bool Printer::m_WritingToFile = false;
std::wostream* Printer::m_Output = nullptr;

bool Printer::Initialize(bool writeToFile, std::wstring_view name)
{
    assert(!m_IsInitialized);
    if(writeToFile)
    {
        m_Output = new std::wofstream(name.data(), std::ios_base::out);
        if(!m_Output->good())
        {
            delete m_Output;
            m_Output = nullptr;
            return false;
        }
    }
    else
    {
        m_Output = &std::wcout;
    }
    m_IsInitialized = true;
    return true;
}

void Printer::Release()
{
    assert(m_IsInitialized);
    if(m_WritingToFile)
    {
        delete m_Output;
    }
    m_Output = nullptr;
    m_WritingToFile = false;
    m_IsInitialized = false;
}

void Printer::PrintNewLine()
{
    assert(m_IsInitialized);
    *m_Output << std::endl;
}

void Printer::PrintString(const std::string& line)
{
    assert(m_IsInitialized);

    *m_Output << line.c_str();
}

void Printer::PrintString(std::wstring_view line)
{
    assert(m_IsInitialized);

    *m_Output << line;
}

void Printer::PrintFormat(std::string_view format, std::format_args&& args)
{
    std::string formatted = std::vformat(format, args);
    PrintString(formatted);
}

void Printer::PrintFormat(std::wstring_view format, std::wformat_args&& args)
{
    std::wstring formatted = std::vformat(format, args);
    PrintString(formatted);
}

PrinterScope::PrinterScope(bool writeToFile, std::wstring_view name)
{
    if(!Printer::Initialize(writeToFile, name))
    {
        if(writeToFile)
        {
            std::string narrowName(name.begin(), name.end());
            throw std::runtime_error(std::format("Could not open {} for writing.", narrowName));
        }
        else
        {
            throw std::runtime_error("Unexpected error during output initialization.");
        }
    }
}

PrinterScope::~PrinterScope()
{
    Printer::Release();
}

void ErrorPrinter::PrintFormat(std::string_view format, std::format_args&& args)
{
    std::string formatted = std::vformat(format, args);
    PrintString(formatted);
}

void ErrorPrinter::PrintFormat(std::wstring_view format, std::wformat_args&& args)
{
    std::wstring formatted = std::vformat(format, args);
    PrintString(formatted);
}

void ErrorPrinter::PrintString(const std::string& line)
{
    std::cerr << line;
}

void ErrorPrinter::PrintString(std::wstring_view line)
{
    std::wcerr << line;
}