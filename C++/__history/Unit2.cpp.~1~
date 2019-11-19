//---------------------------------------------------------------------------

#pragma hdrstop
#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#include <tchar.h>
#include <stdio.h>
#include "Unit2.h"

#include <string>
//---------------------------------------------------------------------------
#pragma package(smart_init)
// initialisation
PortRS::PortRS()
{
	ignoreData = true;
}
//------------------------------------------------------------------------------------//
PortRS::~PortRS()
{
}

//------------------------------------------------------------------------------------//
void PortRS::fermerport()
{
	CloseHandle(hCom);
}

//------------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------//
// connexion au port COM1
int PortRS::ouvrirport()
{
	DCB dcb;
	BOOL fSuccess;
	//TCHAR *pcCommPort = TEXT("COM1"); //  Most systems have a COM1 port
	char * pcCommPort = "COM1";

   //  Open a handle to the specified com port.
   hCom = CreateFile( pcCommPort,
					  GENERIC_READ | GENERIC_WRITE,
					  0,      //  must be opened with exclusive-access
					  NULL,   //  default security attributes
					  OPEN_EXISTING, //  must use OPEN_EXISTING
					  0,      //  not overlapped I/O
					  NULL ); //  hTemplate must be NULL for comm devices

   if (hCom == INVALID_HANDLE_VALUE)
   {
	   //  Handle the error.
	   //printf ("CreateFile failed with error %d.", GetLastError());
	   return (1);
   }

   //  Initialize the DCB structure.
   SecureZeroMemory(&dcb, sizeof(DCB));
   dcb.DCBlength = sizeof(DCB);

   //  Build on the current configuration by first retrieving all current
   //  settings.
   fSuccess = GetCommState(hCom, &dcb);

   if (!fSuccess)
   {
      //  Handle the error.
	  printf ("GetCommState failed with error %d.", GetLastError());
      return (2);
   }

   //  Fill in some DCB values and set the com state:
   //  57,600 bps, 8 data bits, no parity, and 1 stop bit.
   dcb.BaudRate = CBR_4800;     //  baud rate
   dcb.ByteSize = 8;             //  data size, xmit and rcv
   dcb.Parity   = NOPARITY;      //  parity bit
   dcb.StopBits = ONESTOPBIT;    //  stop bit

   fSuccess = SetCommState(hCom, &dcb);

   if (!fSuccess)
   {
	  //  Handle the error.
	  printf ("SetCommState failed with error %d.", GetLastError());
      return (3);
   }

   //  Get the comm config again.
   fSuccess = GetCommState(hCom, &dcb);

   if (!fSuccess)
   {
      //  Handle the error.
	  printf ("GetCommState failed with error %d.", GetLastError());
	  return (2);
   }

   _tprintf (TEXT("Serial port %s successfully reconfigured."), pcCommPort);
   return (0);
}

//------------------------------------------------------------------------------------//
// test d'ecriture de donnée pour hercules
void PortRS::ecrireport(char * buffer, int len)
{
	WriteFile(hCom, buffer, len, &written, NULL);
}

//------------------------------------------------------------------------------------//
//récuperer une donnée de la trame
void PortRS::lireport()
{
	Vcontinue = false;
	isRead = false;

    do
	{
		isRead = ReadFile(hCom, &recvChar, 1, &read, NULL);

		if(ignoreData)
		{
			if(recvChar == '$')
			{
				ignoreData = false;
            }
        }

		if(!ignoreData)
		{
			recvBuf.push_back(recvChar);
			if (!isRead)
			{
				Vcontinue = false;
				break;
			}
		}
	} while (Vcontinue);
}
//------------------------------------------------------------------------------------//
// recuperer la donnée du buffer
std::deque<char> & PortRS::getRecvBuf()
{
	return recvBuf;
}

