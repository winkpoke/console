#ifndef _UTIL_H
#define _UTIL_H

std::string _wtoa(const std::wstring& w)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, w.c_str(), -1, NULL, 0, NULL, NULL);
	if (nLen <= 0) return "";
	char* pszDst = new char[nLen];
	if (NULL == pszDst) return "";
	WideCharToMultiByte(CP_ACP, 0, w.c_str(), -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;
	std::string strTemp(pszDst);
	delete[] pszDst;
	return strTemp;
}

std::wstring _atow(const std::string& a)
{
	int nSize = MultiByteToWideChar(CP_ACP, 0, a.c_str(), (int)a.size(), 0, 0);
	if (nSize <= 0) return L"";
	wchar_t *pwszDst = new wchar_t[nSize + 1];
	if (NULL == pwszDst) return L"";
	MultiByteToWideChar(CP_ACP, 0, a.c_str(), (int)a.size(), pwszDst, nSize);
	pwszDst[nSize] = 0;
	if (pwszDst[0] == 0xFEFF) // skip Oxfeff
	for (int i = 0; i < nSize; i++)
	{
		pwszDst[i] = pwszDst[i + 1];
	}
	std::wstring wcharString(pwszDst);
	delete[]pwszDst;
	return wcharString;
}

#endif