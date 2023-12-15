#include "Json.hpp"

#define RAPIDJSON_HAS_STDSTRING 1
#define RAPIDJSON_NO_SIZETYPEDEFINE
#define RAPIDJSON_SSE2
namespace rapidjson { typedef size_t SizeType; }

#include "ThirdParty/rapidjson/include/rapidjson/document.h"
#include "ThirdParty/rapidjson/include/rapidjson/prettywriter.h"
#include "ThirdParty/rapidjson/include/rapidjson/stringbuffer.h"
#include "ThirdParty/rapidjson/include/rapidjson/allocators.h"
#include "ThirdParty/rapidjson/include/rapidjson/encodings.h"
#include "ThirdParty/rapidjson/include/rapidjson/error/en.h"

namespace Json
{


////////////////////////////////////////////////////////////////////////////////
// PRIVATE IMPLEMENTATION

using ValueType = rapidjson::GenericValue< rapidjson::UTF16<> >;
using StringRefType = rapidjson::GenericStringRef<wchar_t>;
static rapidjson::GenericDocument< rapidjson::UTF16<> > g_Document{rapidjson::kObjectType};
static std::vector<ValueType*> g_ValueStack;
static wstring g_CurrentKey;

static void PushValue(ValueType&& val)
{
	using namespace rapidjson;
	assert(!g_ValueStack.empty());
	ValueType* const currVal = g_ValueStack.back();
	const bool newValNeedsPush = val.GetType() == kObjectType || val.GetType() == kArrayType;
	switch(currVal->GetType())
	{
	case kArrayType:
	{
		currVal->PushBack(val, g_Document.GetAllocator());
		if(newValNeedsPush)
			g_ValueStack.push_back(&currVal->GetArray()[currVal->GetArray().Size() - 1]);
		break;
	}
	case kObjectType:
	{
		assert(!g_CurrentKey.empty());
		currVal->AddMember(
			ValueType{g_CurrentKey.data(), g_CurrentKey.length(), g_Document.GetAllocator()},
			val, g_Document.GetAllocator());
		if(newValNeedsPush)
			g_ValueStack.push_back(&currVal->GetObj()[g_CurrentKey]);
		g_CurrentKey.clear();
		break;
	}
	default:
		assert(0);
	}
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE

void Begin()
{
	assert(g_ValueStack.empty() && g_CurrentKey.empty());
	g_ValueStack.push_back(&g_Document);
}

wstring End()
{
	using namespace rapidjson;
	using StringBufferType = GenericStringBuffer< UTF16LE<> >;
	StringBufferType stringBuffer;
	{
		PrettyWriter< StringBufferType, UTF16<>, UTF16LE<> > writer{stringBuffer};
		g_Document.Accept(writer);
	}
	
	assert(g_ValueStack.size() == 1 && g_ValueStack[0] == &g_Document && g_CurrentKey.empty());
	g_ValueStack.clear();
	
	return wstring{stringBuffer.GetString(), stringBuffer.GetLength()};
}

void WriteString(const wchar_t* str, size_t length)
{
	using namespace rapidjson;
	assert(!g_ValueStack.empty());
	ValueType* const currVal = g_ValueStack.back();
	// Writing key
	if(currVal->GetType() == kObjectType && g_CurrentKey.empty())
		g_CurrentKey = wstring{str, length};
	// Writing value
	else
		PushValue(ValueType{str, length, g_Document.GetAllocator()});
}

void WriteString(const wchar_t* str)
{
	WriteString(str, wcslen(str));
}

void WriteString(wstring&& str)
{
	using namespace rapidjson;
	assert(!g_ValueStack.empty());
	ValueType* const currVal = g_ValueStack.back();
	// Writing key
	if(currVal->GetType() == kObjectType && g_CurrentKey.empty())
		g_CurrentKey = std::move(str);
	// Writing value
	else
		PushValue(ValueType{str.data(), str.length(), g_Document.GetAllocator()});
}

void WriteNull()
{
	PushValue(ValueType{rapidjson::kNullType});
}

void WriteBool(bool value)
{
	PushValue(ValueType{value});

}

void WriteNumber(uint32_t value)
{
	PushValue(ValueType{value});
}

void WriteNumber(uint64_t value)
{
	PushValue(ValueType{value});
}

void WriteNumber(int32_t value)
{
	PushValue(ValueType{value});
}

void WriteNumber(float value)
{
	PushValue(ValueType{value});
}

void BeginObject()
{
	PushValue(ValueType{rapidjson::kObjectType});
}

void EndObject()
{
	assert(!g_ValueStack.empty() && g_ValueStack.back()->GetType() == rapidjson::kObjectType);
	g_ValueStack.pop_back();
}

void BeginArray()
{
	PushValue(ValueType{rapidjson::kArrayType});
}

void EndArray()
{
	assert(!g_ValueStack.empty() && g_ValueStack.back()->GetType() == rapidjson::kArrayType);
	g_ValueStack.pop_back();
}

void WriteNameAndString(const wchar_t* name, const wchar_t* str, size_t length)
{
	WriteString(name);
	WriteString(str, length);
}

void WriteNameAndString(const wchar_t* name, const wchar_t* str)
{
	WriteString(name);
	WriteString(str);
}

void WriteNameAndString(const wchar_t* name, wstring&& str)
{
	WriteString(name);
	WriteString(std::move(str));
}

void WriteNameAndBool(const wchar_t* name, bool value)
{
	WriteString(name);
	WriteBool(value);
}

void WriteNameAndNumber(const wchar_t* name, uint32_t value)
{
	WriteString(name);
	WriteNumber(value);
}

void WriteNameAndNumber(const wchar_t* name, uint64_t value)
{
	WriteString(name);
	WriteNumber(value);
}

void WriteNameAndNumber(const wchar_t* name, int32_t value)
{
	WriteString(name);
	WriteNumber(value);
}

void WriteNameAndNumber(const wchar_t* name, float value)
{
	WriteString(name);
	WriteNumber(value);
}

} // namespace Json
