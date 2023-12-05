#ifndef WIN_HTTP_SESSION_H_
#define WIN_HTTP_SESSION_H_

#include <windows.h>
#include <winhttp.h>

#include <string>

struct ResponseData
{
	std::wstring header;
	std::string content;
	DWORD ulErrorCode = 0;
	std::string error;
};


class CWinHttpSession
{
public:
	CWinHttpSession();
	~CWinHttpSession();
	bool Open();
	bool RequestGet(const wchar_t* server_url, const wchar_t* header, ResponseData& response);
	bool RequestPost(const wchar_t* server_url, const wchar_t* header, const char *option, DWORD option_length, ResponseData& response);
private:
	HINTERNET m_hSession = nullptr;

	void Close();

	std::wstring PickHostName(URL_COMPONENTS& urlComp);
	std::wstring PickUrlPath(URL_COMPONENTS& urlComp);

	bool ReceiveResponse(HINTERNET hRequest, ResponseData& response);
	std::wstring ReadResponseHeader(HINTERNET hRequest);
	std::string ReadResponseData(HINTERNET hRequest);
};


#endif //WIN_HTTP_SESSION_H_