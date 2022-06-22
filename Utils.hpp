#pragma once

extern const wchar_t* const PROGRAM_NAME;
extern const char* const PROGRAM_NAME_ANSI;
extern const wchar_t* const PROGRAM_VERSION;
extern const uint32_t PROGRAM_VERSION_NUMBER;

wstring SizeToStr(uint64_t size);
// As codePage use e.g. CP_ACP (native Windows), CP_UTF8.
wstring StrToWstr(const char* str, uint32_t codePage);
string WstrToStr(const wchar_t* str, uint32_t codePage);

class CmdLineParser
{
public:
	enum RESULT
	{
		RESULT_OPT,
		RESULT_PARAMETER,
		RESULT_END,
		RESULT_ERROR,
	};

	CmdLineParser(int argc, wchar_t **argv);
	CmdLineParser(const wchar_t *CmdLine);
	
    void RegisterOpt(uint32_t Id, wchar_t Opt, bool Parameter);
	void RegisterOpt(uint32_t Id, const std::wstring &Opt, bool Parameter);
	
    RESULT ReadNext();
	uint32_t GetOptId();
	const std::wstring & GetParameter();

private:
	struct SHORT_OPT
	{
		uint32_t Id;
		wchar_t Opt;
		bool Parameter;

		SHORT_OPT(uint32_t Id, wchar_t Opt, bool Parameter) : Id(Id), Opt(Opt), Parameter(Parameter) { }
	};

	struct LONG_OPT
	{
		uint32_t Id;
		std::wstring Opt;
		bool Parameter;

		LONG_OPT(uint32_t Id, std::wstring Opt, bool Parameter) : Id(Id), Opt(Opt), Parameter(Parameter) { }
	};

	wchar_t **m_argv;
	const wchar_t *m_CmdLine;
	int m_argc;
	size_t m_CmdLineLength;
	size_t m_ArgIndex;

	bool ReadNextArg(std::wstring *OutArg);

	std::vector<SHORT_OPT> m_ShortOpts;
	std::vector<LONG_OPT> m_LongOpts;

	SHORT_OPT * FindShortOpt(wchar_t Opt);
	LONG_OPT * FindLongOpt(const std::wstring &Opt);

	bool m_InsideMultioption;
	std::wstring m_LastArg;
	size_t m_LastArgIndex;
	uint32_t m_LastOptId;
	std::wstring m_LastParameter;
};
