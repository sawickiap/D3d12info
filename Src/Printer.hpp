/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/

#pragma once

#include <format>

class Printer
{
public:
    static bool Initialize(bool writeToFile, std::wstring_view name);
    static void Release();

    static void PrintNewLine();

    static void PrintString(const std::string& line);
    static void PrintString(std::wstring_view line);

    static void PrintFormat(std::string_view format, std::format_args&& args);
    static void PrintFormat(std::wstring_view format, std::wformat_args&& args);

private:
    static bool m_IsInitialized;
    static bool m_WritingToFile;
    static std::wostream* m_Output;
};

class PrinterScope
{
public:
    PrinterScope(bool writeToFile, std::wstring_view name);
    ~PrinterScope();
};

class ErrorPrinter
{
public:
    static void PrintString(const std::string& line);
    static void PrintString(std::wstring_view line);

    static void PrintFormat(std::string_view format, std::format_args&& args);
    static void PrintFormat(std::wstring_view format, std::wformat_args&& args);
};
