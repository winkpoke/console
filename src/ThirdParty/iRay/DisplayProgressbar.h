#ifndef _DISPLAYPROGRESSBAR_H
#define _DISPLAYPROGRESSBAR_H

#include <string>
using namespace::std;


class DisplayProgressbar
{
public:
	void SetProgress(int nValue, int total)
	{
		if (nValue <= 0)
			return;
		int signalCurNum = nValue;
		int singalTotal = total;
		if (total > TotalValue)
		{
			signalCurNum = nValue * TotalValue / total;
			singalTotal = TotalValue;
		}
		m_progress.assign(signalCurNum, '#');
		m_output.assign(singalTotal - signalCurNum + 2, ' ');
		printf("%s %d/%d\r", (m_progress + m_output).c_str(), nValue, total);
	}
private:
	string m_progress;
	string m_output;
	static const int TotalValue = 35;
};


#endif