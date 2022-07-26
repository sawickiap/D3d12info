#include "pch.hpp"
#include "Utils.hpp"

const wchar_t* const PROGRAM_NAME = L"D3d12info";
const char* const PROGRAM_NAME_ANSI = "D3d12info";
const wchar_t* const PROGRAM_VERSION = L"1.0.0";
const uint32_t PROGRAM_VERSION_NUMBER = (1u << 20) | (0u << 10) | (0u);

wstring SizeToStr(uint64_t size)
{
	if(size == 0)
		return L"0";
	wchar_t s[16];
	if (size < 1024llu)
		swprintf_s(s, L"%llu B", size);
	else if(size < 1024llu * 1024)
		swprintf_s(s, L"%.2f KB", size / 1024.);
	else if(size < 1024llu * 1024 * 1024)
		swprintf_s(s, L"%.2f MB", size / (1024. * 1024.));
	else if(size < 1024llu * 1024 * 1024 * 1024)
		swprintf_s(s, L"%.2f GB", size / (1024. * 1024. * 1024.));
	else if(size < 1024llu * 1024 * 1024 * 1024 * 1024)
		swprintf_s(s, L"%.2f TB", size / (1024. * 1024. * 1024. * 1024.));
	return s;
}

wstring StrToWstr(const char* str, uint32_t codePage)
{
    if(!str || !*str)
        return wstring{};
    const int size = MultiByteToWideChar(codePage, 0, str, (int)strlen(str), NULL, 0);
    if(size == 0)
        return wstring{};
    std::vector<wchar_t> buf((size_t)size);
    const int result = MultiByteToWideChar(codePage, 0, str, (int)strlen(str), buf.data(), size);
    if(result == 0)
        return wstring{};
    return wstring{buf.data(), buf.size()};
}

////////////////////////////////////////////////////////////////////////////////
// class CmdLineParser

bool CmdLineParser::ReadNextArg(std::wstring *OutArg)
{
	if (m_argv != NULL)
	{
		if (m_ArgIndex >= (size_t)m_argc) return false;

		*OutArg = m_argv[m_ArgIndex];
		m_ArgIndex++;
		return true;
	}
	else
	{
		if (m_ArgIndex >= m_CmdLineLength) return false;
		
		OutArg->clear();
		bool InsideQuotes = false;
		while (m_ArgIndex < m_CmdLineLength)
		{
			wchar_t Ch = m_CmdLine[m_ArgIndex];
			if (Ch == L'\\')
			{
				bool FollowedByQuote = false;
				size_t BackslashCount = 1;
				size_t TmpIndex = m_ArgIndex + 1;
				while (TmpIndex < m_CmdLineLength)
				{
					wchar_t TmpCh = m_CmdLine[TmpIndex];
					if (TmpCh == L'\\')
					{
						BackslashCount++;
						TmpIndex++;
					}
					else if (TmpCh == L'"')
					{
						FollowedByQuote = true;
						break;
					}
					else
						break;
				}

				if (FollowedByQuote)
				{
					if (BackslashCount % 2 == 0)
					{
						for (size_t i = 0; i < BackslashCount / 2; i++)
							*OutArg += L'\\';
						m_ArgIndex += BackslashCount + 1;
						InsideQuotes = !InsideQuotes;
					}
					else
					{
						for (size_t i = 0; i < BackslashCount / 2; i++)
							*OutArg += L'\\';
						*OutArg += L'"';
						m_ArgIndex += BackslashCount + 1;
					}
				}
				else
				{
					for (size_t i = 0; i < BackslashCount; i++)
						*OutArg += L'\\';
					m_ArgIndex += BackslashCount;
				}
			}
			else if (Ch == L'"')
			{
				InsideQuotes = !InsideQuotes;
				m_ArgIndex++;
			}
			else if (isspace(Ch))
			{
				if (InsideQuotes)
				{
					*OutArg += Ch;
					m_ArgIndex++;
				}
				else
				{
					m_ArgIndex++;
					break;
				}
			}
			else
			{
				*OutArg += Ch;
				m_ArgIndex++;
			}
		}

		while (m_ArgIndex < m_CmdLineLength && isspace(m_CmdLine[m_ArgIndex]))
			m_ArgIndex++;

		return true;
	}
}

CmdLineParser::SHORT_OPT * CmdLineParser::FindShortOpt(wchar_t Opt)
{
	for (size_t i = 0; i < m_ShortOpts.size(); i++)
		if (m_ShortOpts[i].Opt == Opt)
			return &m_ShortOpts[i];
	return NULL;
}

CmdLineParser::LONG_OPT * CmdLineParser::FindLongOpt(const std::wstring &Opt)
{
	for (size_t i = 0; i < m_LongOpts.size(); i++)
		if (m_LongOpts[i].Opt == Opt)
			return &m_LongOpts[i];
	return NULL;
}

CmdLineParser::CmdLineParser(int argc, wchar_t **argv) :
	m_argv(argv),
	m_CmdLine(NULL),
	m_argc(argc),
	m_CmdLineLength(0),
	m_ArgIndex(1),
	m_InsideMultioption(false),
	m_LastArgIndex(0),
	m_LastOptId(0)
{
	assert(argc > 0);
	assert(argv != NULL);
}

CmdLineParser::CmdLineParser(const wchar_t *CmdLine) :
	m_argv(NULL),
	m_CmdLine(CmdLine),
	m_argc(0),
	m_ArgIndex(0),
	m_InsideMultioption(false),
	m_LastArgIndex(0),
	m_LastOptId(0)
{
	assert(CmdLine != NULL);

	m_CmdLineLength = wcslen(m_CmdLine);

	while (m_ArgIndex < m_CmdLineLength && isspace(m_CmdLine[m_ArgIndex]))
		m_ArgIndex++;
}

void CmdLineParser::RegisterOpt(uint32_t Id, wchar_t Opt, bool Parameter)
{
	assert(Opt != L'\0');

	m_ShortOpts.push_back(SHORT_OPT(Id, Opt, Parameter));
}

void CmdLineParser::RegisterOpt(uint32_t Id, const std::wstring &Opt, bool Parameter)
{
	assert(!Opt.empty());
	
	m_LongOpts.push_back(LONG_OPT(Id, Opt, Parameter));
}

CmdLineParser::RESULT CmdLineParser::ReadNext()
{
	if (m_InsideMultioption)
	{
		assert(m_LastArgIndex < m_LastArg.length());
		SHORT_OPT *so = FindShortOpt(m_LastArg[m_LastArgIndex]);
		if (so == NULL)
		{
			m_LastOptId = 0;
			m_LastParameter.clear();
			return CmdLineParser::RESULT_ERROR;
		}
		if (so->Parameter)
		{
			if (m_LastArg.length() == m_LastArgIndex+1)
			{
				if (!ReadNextArg(&m_LastParameter))
				{
					m_LastOptId = 0;
					m_LastParameter.clear();
					return CmdLineParser::RESULT_ERROR;
				}
				m_InsideMultioption = false;
				m_LastOptId = so->Id;
				return CmdLineParser::RESULT_OPT;
			}
			else if (m_LastArg[m_LastArgIndex+1] == L'=')
			{
				m_InsideMultioption = false;
				m_LastParameter = m_LastArg.substr(m_LastArgIndex+2);
				m_LastOptId = so->Id;
				return CmdLineParser::RESULT_OPT;
			}
			else
			{
				m_InsideMultioption = false;
				m_LastParameter = m_LastArg.substr(m_LastArgIndex+1);
				m_LastOptId = so->Id;
				return CmdLineParser::RESULT_OPT;
			}
		}
		else
		{
			if (m_LastArg.length() == m_LastArgIndex+1)
			{
				m_InsideMultioption = false;
				m_LastParameter.clear();
				m_LastOptId = so->Id;
				return CmdLineParser::RESULT_OPT;
			}
			else
			{
				m_LastArgIndex++;

				m_LastParameter.clear();
				m_LastOptId = so->Id;
				return CmdLineParser::RESULT_OPT;
			}
		}
	}
	else
	{
		if (!ReadNextArg(&m_LastArg))
		{
			m_LastParameter.clear();
			m_LastOptId = 0;
			return CmdLineParser::RESULT_END;
		}
		
		if (!m_LastArg.empty() && m_LastArg[0] == L'-')
		{
			if (m_LastArg.length() > 1 && m_LastArg[1] == L'-')
			{
				size_t EqualIndex = m_LastArg.find(L'=', 2);
				if (EqualIndex != std::wstring::npos)
				{
					LONG_OPT *lo = FindLongOpt(m_LastArg.substr(2, EqualIndex-2));
					if (lo == NULL || lo->Parameter == false)
					{
						m_LastOptId = 0;
						m_LastParameter.clear();
						return CmdLineParser::RESULT_ERROR;
					}
					m_LastParameter = m_LastArg.substr(EqualIndex+1);
					m_LastOptId = lo->Id;
					return CmdLineParser::RESULT_OPT;
				}
				else
				{
					LONG_OPT *lo = FindLongOpt(m_LastArg.substr(2));
					if (lo == NULL)
					{
						m_LastOptId = 0;
						m_LastParameter.clear();
						return CmdLineParser::RESULT_ERROR;
					}
					if (lo->Parameter)
					{
						if (!ReadNextArg(&m_LastParameter))
						{
							m_LastOptId = 0;
							m_LastParameter.clear();
							return CmdLineParser::RESULT_ERROR;
						}
					}
					else
						m_LastParameter.clear();
					m_LastOptId = lo->Id;
					return CmdLineParser::RESULT_OPT;
				}
			}
			else
			{
				if (m_LastArg.length() < 2)
				{
					m_LastOptId = 0;
					m_LastParameter.clear();
					return CmdLineParser::RESULT_ERROR;
				}
				SHORT_OPT *so = FindShortOpt(m_LastArg[1]);
				if (so == NULL)
				{
					m_LastOptId = 0;
					m_LastParameter.clear();
					return CmdLineParser::RESULT_ERROR;
				}
				if (so->Parameter)
				{
					if (m_LastArg.length() == 2)
					{
						if (!ReadNextArg(&m_LastParameter))
						{
							m_LastOptId = 0;
							m_LastParameter.clear();
							return CmdLineParser::RESULT_ERROR;
						}
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
					else if (m_LastArg[2] == L'=')
					{
						m_LastParameter = m_LastArg.substr(3);
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
					else
					{
						m_LastParameter = m_LastArg.substr(2);
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
				}
				else
				{
					if (m_LastArg.length() == 2)
					{
						m_LastParameter.clear();
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
					else
					{
						m_InsideMultioption = true;
						m_LastArgIndex = 2;

						m_LastParameter.clear();
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
				}
			}
		}
		else if (!m_LastArg.empty() && m_LastArg[0] == L'/')
		{
			size_t EqualIndex = m_LastArg.find('=', 1);
			if (EqualIndex != std::wstring::npos)
			{
				if (EqualIndex == 2)
				{
					SHORT_OPT *so = FindShortOpt(m_LastArg[1]);
					if (so != NULL)
					{
						if (so->Parameter == false)	
						{
							m_LastOptId = 0;
							m_LastParameter.clear();
							return CmdLineParser::RESULT_ERROR;
						}
						m_LastParameter = m_LastArg.substr(EqualIndex+1);
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
				}
				LONG_OPT *lo = FindLongOpt(m_LastArg.substr(1, EqualIndex-1));
				if (lo == NULL || lo->Parameter == false)
				{
					m_LastOptId = 0;
					m_LastParameter.clear();
					return CmdLineParser::RESULT_ERROR;
				}
				m_LastParameter = m_LastArg.substr(EqualIndex+1);
				m_LastOptId = lo->Id;
				return CmdLineParser::RESULT_OPT;
			}
			else
			{
				if (m_LastArg.length() == 2)
				{
					SHORT_OPT *so = FindShortOpt(m_LastArg[1]);
					if (so != NULL)
					{
						if (so->Parameter)
						{
							if (!ReadNextArg(&m_LastParameter))
							{
								m_LastOptId = 0;
								m_LastParameter.clear();
								return CmdLineParser::RESULT_ERROR;
							}
						}
						else
							m_LastParameter.clear();
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
				}
				LONG_OPT *lo = FindLongOpt(m_LastArg.substr(1));
				if (lo == NULL)
				{
					m_LastOptId = 0;
					m_LastParameter.clear();
					return CmdLineParser::RESULT_ERROR;
				}
				if (lo->Parameter)
				{
					if (!ReadNextArg(&m_LastParameter))
					{
						m_LastOptId = 0;
						m_LastParameter.clear();
						return CmdLineParser::RESULT_ERROR;
					}
				}
				else
					m_LastParameter.clear();
				m_LastOptId = lo->Id;
				return CmdLineParser::RESULT_OPT;
			}
		}
		else
		{
			m_LastOptId = 0;
			m_LastParameter = m_LastArg;
			return CmdLineParser::RESULT_PARAMETER;
		}
	}
}

uint32_t CmdLineParser::GetOptId()
{
	return m_LastOptId;
}

const std::wstring & CmdLineParser::GetParameter()
{
	return m_LastParameter;
}
