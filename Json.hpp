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
void BeginObject();
void EndObject();
void BeginArray();
void EndArray();

} // namespace Json
