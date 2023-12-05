# WinHttpSession
WinHTTPの簡易クラス。Get/Post要求ができます。

## 使い方

``` cpp

#include "win_http_session.h"


/*HTTP POST要求*/
bool RequestHttpPost(const std::wstring& wstrUrl, const std::wstring& wstrHeader, const std::string& strPayload, ResponseData& r)
{
	bool bRet = false;

	CWinHttpSession* pSession = new(std::nothrow) CWinHttpSession();
	if (pSession != nullptr)
	{
		bRet = pSession->Open();
		if (bRet)
		{
			bRet = pSession->RequestPost(wstrUrl.c_str(), wstrHeader.c_str(), strPayload.c_str(), static_cast<DWORD>(strPayload.size()), r);
		}
		delete pSession;
	}

	return bRet;
}
/*HTTP GET要求*/
bool RequestHttpGet(const std::wstring &wstrUrl, const std::wstring &wstrHeader, ResponseData &r)
{
	bool bRet = false;

	CWinHttpSession* pSession = new(std::nothrow) CWinHttpSession();
	if (pSession != nullptr)
	{
		bRet = pSession->Open();
		if (bRet)
		{
			bRet = pSession->RequestGet(wstrUrl.c_str(), wstrHeader.c_str(), r);
		}
		delete pSession;
	}

	return bRet;
}

```
