/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2023 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#pragma once

namespace Json
{

void Begin();
wstring End();

void WriteString(const wchar_t* str, size_t length);
void WriteString(const wchar_t* str);
void WriteString(wstring&& str);
void WriteNull();
void WriteBool(bool value);
void WriteNumber(uint32_t value);
void WriteNumber(uint64_t value);
void WriteNumber(int32_t value);
void WriteNumber(float value);
void BeginObject();
void EndObject();
void BeginArray();
void EndArray();

void WriteNameAndString(const wchar_t* name, const wchar_t* str, size_t length);
void WriteNameAndString(const wchar_t* name, const wchar_t* str);
void WriteNameAndString(const wchar_t* name, wstring&& str);
void WriteNameAndBool(const wchar_t* name, bool value);
void WriteNameAndNumber(const wchar_t* name, uint32_t value);
void WriteNameAndNumber(const wchar_t* name, uint64_t value);
void WriteNameAndNumber(const wchar_t* name, int32_t value);
void WriteNameAndNumber(const wchar_t* name, float value);

} // namespace Json
