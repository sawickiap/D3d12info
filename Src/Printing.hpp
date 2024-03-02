/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2024 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#pragma once

struct EnumItem;

extern bool g_UseJson;
extern uint32_t g_Indent;

void PrintEmptyLine();
void PrintHeader(const wchar_t* s, uint8_t headerLevel);
void PrintIndent();
void PrintName(const wchar_t* name);
void Print_string(const wchar_t* name, const wchar_t* value);
void PrintStructBegin(const wchar_t* structName);
void PrintStructEnd();
void Print_BOOL(const wchar_t* name, BOOL value);
void Print_uint32(const wchar_t* name, uint32_t value, const wchar_t* unit = nullptr);
void Print_uint64(const wchar_t* name, uint64_t value, const wchar_t* unit = nullptr);
void Print_size(const wchar_t* name, uint64_t value);
void Print_sizeKilobytes(const wchar_t* name, uint64_t value);
void Print_hex32(const wchar_t* name, uint32_t value);
void Print_int32(const wchar_t* name, int32_t value, const wchar_t* unit = nullptr);
void Print_float(const wchar_t* name, float value, const wchar_t* unit = nullptr);
void PrintEnum(const wchar_t* name, uint32_t value, const EnumItem* enumItems, bool isSigned = false);
void PrintFlags(const wchar_t* name, uint32_t value, const EnumItem* enumItems);
void PrintHexBytes(const wchar_t* name, const void* data, size_t byteCount);
void PrintFormat(const wchar_t* name, const wchar_t* format, ...);
void PrintVendorId(const wchar_t* name, uint32_t value);
void PrintSubsystemId(const wchar_t* name, uint32_t value);

struct ScopedStructRegion
{
public:
    ScopedStructRegion(const wchar_t* name);
    ~ScopedStructRegion();
};
