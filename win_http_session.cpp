
#include "win_http_session.h"

#pragma comment(lib, "Winhttp.lib")


CWinHttpSession::CWinHttpSession()
{

}

CWinHttpSession::~CWinHttpSession()
{
	Close();
}
/*開始*/
bool CWinHttpSession::Open()
{
	if (m_hSession == nullptr)
	{
		m_hSession = ::WinHttpOpen(L"WinHttpSession", WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (m_hSession != nullptr)
		{
			return true;
		}
	}
	return false;
}
/*GET要求*/
bool CWinHttpSession::RequestGet(const wchar_t* server_url, const wchar_t* header, ResponseData& response)
{
	bool bResult = false;

	if (m_hSession != nullptr)
	{
		URL_COMPONENTS urlComp{};
		urlComp.dwStructSize = sizeof(URL_COMPONENTS);
		urlComp.dwHostNameLength = static_cast<DWORD>(-1L);
		urlComp.dwUrlPathLength = static_cast<DWORD>(-1L);
		BOOL iRet = ::WinHttpCrackUrl(server_url, 0, 0, &urlComp);
		if (iRet)
		{
			std::wstring wstrHost = PickHostName(urlComp);
			std::wstring wstrPath = PickUrlPath(urlComp);

			if (!wstrHost.empty() && !wstrPath.empty())
			{
				HINTERNET hConnect = ::WinHttpConnect(m_hSession, wstrHost.c_str(), urlComp.nPort, 0);
				if (hConnect != nullptr)
				{
					DWORD dwFlag = urlComp.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
					HINTERNET hRequest = ::WinHttpOpenRequest(hConnect, L"GET", wstrPath.c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwFlag);
					if (hRequest != nullptr)
					{
						iRet = ::WinHttpSendRequest(hRequest, header, -1L, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
						if (iRet)
						{
							bResult = ReceiveResponse(hRequest, response);
						}
						else
						{
							response.ulErrorCode = ::GetLastError();
							response.error = "WinHttpSendRequest";
						}

						::WinHttpCloseHandle(hRequest);
					}
					else
					{
						response.ulErrorCode = ::GetLastError();
						response.error = "WinHttpOpenRequest";
					}

					::WinHttpCloseHandle(hConnect);
				}
				else
				{
					response.ulErrorCode = ::GetLastError();
					response.error = "WinHttpConnect";
				}
			}
		}
		else
		{
			response.ulErrorCode = ::GetLastError();
			response.error = "WinHttpCrackUrl";
		}

	}

	return bResult;
}
/*POST要求*/
bool CWinHttpSession::RequestPost(const wchar_t* server_url, const wchar_t* header, const char* option, DWORD option_length, ResponseData& response)
{
	bool bResult = false;

	if (m_hSession != nullptr)
	{
		URL_COMPONENTS urlComp{};
		urlComp.dwStructSize = sizeof(URL_COMPONENTS);
		urlComp.dwHostNameLength = static_cast<DWORD>(-1L);
		urlComp.dwUrlPathLength = static_cast<DWORD>(-1L);
		BOOL iRet = ::WinHttpCrackUrl(server_url, 0, 0, &urlComp);
		if (iRet)
		{
			std::wstring wstrHost = PickHostName(urlComp);
			std::wstring wstrPath = PickUrlPath(urlComp);

			if (!wstrHost.empty() && !wstrPath.empty())
			{
				HINTERNET hConnect = ::WinHttpConnect(m_hSession, wstrHost.c_str(), urlComp.nPort, 0);
				if (hConnect != nullptr)
				{
					DWORD dwFlag = urlComp.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
					HINTERNET hRequest = ::WinHttpOpenRequest(hConnect, L"POST", wstrPath.c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwFlag);
					if (hRequest != nullptr)
					{
						iRet = ::WinHttpSendRequest(hRequest, header, -1L, const_cast<char*>(option), option_length, option_length, 0);
						if (iRet)
						{
							bResult = ReceiveResponse(hRequest, response);
						}
						else
						{
							response.ulErrorCode = ::GetLastError();
							response.error = "WinHttpSendRequest";
						}

						::WinHttpCloseHandle(hRequest);
					}
					else
					{
						response.ulErrorCode = ::GetLastError();
						response.error = "WinHttpOpenRequest";
					}

					::WinHttpCloseHandle(hConnect);
				}
				else
				{
					response.ulErrorCode = ::GetLastError();
					response.error = "WinHttpConnect";
				}
			}
		}
		else
		{
			response.ulErrorCode = ::GetLastError();
			response.error = "WinHttpCrackUrl";
		}

	}

	return bResult;
}
/*終了*/
void CWinHttpSession::Close()
{
	if (m_hSession != nullptr)
	{
		::WinHttpCloseHandle(m_hSession);
	}
}
/*ホスト名取り出し*/
std::wstring CWinHttpSession::PickHostName(URL_COMPONENTS& urlComp)
{
	std::wstring wstrHost;
	if (urlComp.lpszHostName != nullptr)
	{
		wchar_t* host = static_cast<wchar_t*>(malloc((urlComp.dwHostNameLength + 1ULL) * sizeof(wchar_t)));
		if (host != nullptr)
		{
			memcpy(host, urlComp.lpszHostName, urlComp.dwHostNameLength * sizeof(wchar_t));
			*(host + urlComp.dwHostNameLength) = L'\0';
			wstrHost = host;
			free(host);
		}
	}

	return wstrHost;
}
/*パス取り出し*/
std::wstring CWinHttpSession::PickUrlPath(URL_COMPONENTS& urlComp)
{
	std::wstring wstrPath;

	if (urlComp.lpszUrlPath != nullptr)
	{
		wchar_t* path = static_cast<wchar_t*>(malloc((urlComp.dwUrlPathLength + 1ULL) * sizeof(wchar_t)));
		if (path != nullptr)
		{
			memcpy(path, urlComp.lpszUrlPath, urlComp.dwUrlPathLength * sizeof(wchar_t));
			*(path + urlComp.dwUrlPathLength) = L'\0';
			wstrPath = path;
			free(path);
		}
	}

	return wstrPath;
}
/*応答読み取り*/
bool CWinHttpSession::ReceiveResponse(HINTERNET hRequest, ResponseData& response)
{
	if (hRequest != nullptr)
	{
		BOOL iRet = ::WinHttpReceiveResponse(hRequest, nullptr);
		if (iRet)
		{
			std::wstring wstrHeader = ReadResponseHeader(hRequest);
			if (!wstrHeader.empty())
			{
				response.header = wstrHeader;
			}
			else
			{
				response.ulErrorCode = ::GetLastError();
				response.error = "WinHttpQueryHeaders";
				return false;
			}

			std::string strData = ReadResponseData(hRequest);
			if (!strData.empty())
			{
				response.content = strData;
			}
			else
			{
				response.ulErrorCode = ::GetLastError();
				response.error = "WinHttpQueryDataAvailable";
				return false;
			}
		}
		else
		{
			response.ulErrorCode = ::GetLastError();
			response.error = "WinHttpReceiveResponse";
			return false;
		}
		
	}

	return true;
}
/*ヘッダ情報読み取り*/
std::wstring CWinHttpSession::ReadResponseHeader(HINTERNET hRequest)
{
	std::wstring wstrHeader;

	if (hRequest != nullptr)
	{
		DWORD dwSize = 0;
		/*ヘッダ長取得*/
		BOOL iRet = ::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, nullptr, &dwSize, WINHTTP_NO_HEADER_INDEX);
		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			wchar_t* buffer = static_cast<wchar_t*>(malloc((dwSize + 1ULL) * sizeof(wchar_t)));
			if (buffer != nullptr)
			{
				iRet = ::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, buffer, &dwSize, WINHTTP_NO_HEADER_INDEX);
				if (iRet)
				{
					*(buffer + dwSize) = L'\0';
					wstrHeader = buffer;
				}
				free(buffer);
			}

		}
	}

	return wstrHeader;
}
/*データ部読み取り*/
std::string CWinHttpSession::ReadResponseData(HINTERNET hRequest)
{
	std::string strData;

	if (hRequest != nullptr)
	{
		for (;;)
		{
			DWORD dwSize = 0;
			BOOL iRet = ::WinHttpQueryDataAvailable(hRequest, &dwSize);
			if (!dwSize)break;
			else if (strData.empty())strData.reserve(dwSize);//push_back用
			if (iRet)
			{
				char* buffer = static_cast<char*>(malloc(dwSize + 1ULL));
				if (buffer != nullptr)
				{
					DWORD dwRead = 0;
					iRet = ::WinHttpReadData(hRequest, buffer, dwSize, &dwRead);
					if (iRet)
					{
						*(buffer + dwRead) = '\0';
						/*NULL包含。string_literalsは扱いにくい。*/
						//strData.append(buffer);
						for (size_t i = 0; i < dwRead; ++i)
						{
							strData.push_back(buffer[i]);
						}
					}
					free(buffer);
				}
			}
			else
			{
				break;
			}
		}
	}
	return strData;
}