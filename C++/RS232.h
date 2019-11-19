//---------------------------------------------------------------------------

#ifndef RS232H
#define RS232H
#include <windows.h>
#include <queue>
class PortRS
{

	HANDLE hCom;
	DWORD written;
	DWORD read;
	char recvChar;
	bool Vcontinue;
	int isRead;
	bool ignoreData;

	std::deque<char> recvBuf;

	public:
	PortRS();
	~PortRS();
	int ouvrirport();
	void lireport();
	void ecrireport(char *, int);
	void fermerport();
	int connectbdd();

	std::deque<char> & getRecvBuf();
};
//---------------------------------------------------------------------------
#endif
