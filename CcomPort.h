#ifndef _CCOMPORT_H
#define _CCOMPORT_H

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

class COM{
public:
	HANDLE hCom;
	DCB dcb;
	OVERLAPPED m_ov;
	COMSTAT comstat;
	DWORD  m_dwCommEvents;

	bool open_port(LPCTSTR portname)
	{
		hCom = CreateFile(portname, GENERIC_WRITE|GENERIC_READ,	0, 0, OPEN_EXISTING, 0, 0);

		if(hCom == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hCom);
			return false;
		}
		else
			return true;
	}

	bool setupdcb(int rate_arg){

		int rate = rate_arg;
		memset(&dcb,0,sizeof(dcb));
		if(!GetCommState(hCom,&dcb))
		{
			return false;
		}
		dcb.BaudRate = rate;
		dcb.Parity   = NOPARITY;
		dcb.fParity  = 0;
		dcb.StopBits = 0;
		dcb.ByteSize = 8;
		dcb.fOutxCtsFlow = 0;
		dcb.fOutxDsrFlow = 0;
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		dcb.fDsrSensitivity = 0;
		dcb.fRtsControl = RTS_CONTROL_DISABLE;
		dcb.fOutX = 0;
		dcb.fInX  = 0;

		dcb.fErrorChar = 0;
		dcb.fBinary    = 1;
		dcb.fNull      = 0;
		dcb.fAbortOnError = 0;
		dcb.wReserved = 0;
		dcb.EvtChar = 0;

		if (!SetCommState(hCom, &dcb))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	void setbuffer()
	{
		SetupComm(hCom,4,4);	
	}

	bool setuptimeout(DWORD ReadInterval, 
					  DWORD ReadTotalMultiplier,
					  DWORD ReadTotalconstant,
					  DWORD WriteTotalMultiplier,
					  DWORD WriteTotalconstant)
	{
		COMMTIMEOUTS timeouts;
		timeouts.ReadIntervalTimeout=ReadInterval;
		timeouts.ReadTotalTimeoutConstant=ReadTotalconstant;
		timeouts.ReadTotalTimeoutMultiplier=ReadTotalMultiplier;
		if(!SetCommTimeouts(hCom, &timeouts))return false;
		else return true;
	}

	void clearbuffer()
	{
		PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);
	}

	bool write(char c)
	{ 
		DWORD dwBytesWrite=1;  
		COMSTAT ComStat;  
		DWORD dwErrorFlags;  
		BOOL bWriteStat;  
		ClearCommError(hCom,&dwErrorFlags,&ComStat);  
		//bWriteStat=WriteFile(hCom,&buff_write,4,&dwBytesWrite,NULL);
		bWriteStat=WriteFile(hCom,&c,1,&dwBytesWrite,NULL);  
		if(!bWriteStat)  
		{  
			printf("fail to write\n"); 
			return FALSE;
		}  
		else printf("success to write\n");
		PurgeComm(hCom, PURGE_TXABORT|  
			PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);  
		return TRUE;  
	}

	void close(){
		CloseHandle(hCom);
	}
};

#endif // !_CCOMPORT_H