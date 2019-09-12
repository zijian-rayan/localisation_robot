#include "CcomPort.h"
#include <stdio.h>
#include "RecieveData.h"
#include "tchar.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <thread>
#include "Interface.h"
#pragma comment( linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"") 
CWind * CWind::pBase = 0;
int WINAPI WinMain(HINSTANCE hinstance,HINSTANCE hPrevinstance,LPSTR lpCmdLine,int nCmdShow)
{
	COM com;
	char bufferPort[5] = "";	
	int id = 0, cmd = 0, x = 0, y = 0;
	bool newDataEnable = false;
	bool WindowAlive = true;
	bool writeRead = false;
	int formatChaine = 0;
	size_t CommandePosCible = 0;
  
	std::thread receptData(ReceptionData, std::ref(newDataEnable),std::ref(id), std::ref(x), std::ref(y) );
	receptData.hardware_concurrency();

	std::thread threadInterface(Interface, &id, &cmd, &x, &y, &WindowAlive, &writeRead);
	threadInterface.hardware_concurrency();

	system("start MotionDetection.exe");	


	/* **********open port ************/
	bool connected = false;
	printf("Entrer le nom du port série (COM3):");
	fgets(bufferPort, sizeof(bufferPort), stdin);
	printf("%s", bufferPort);

	while (!connected)
	{
	  if (com.open_port(_T(bufferPort)))
	  {
	    connected = true;
	    printf("open port %s success\n", bufferPort);
	  }
	  else
	  {
	    printf("Echec d'ouverture de port\n");
	    fgets(bufferPort, sizeof(bufferPort), stdin);
	    printf("%s", bufferPort);
	  }
	}

	//if (com.open_port(_T(bufferPort)))
	//{
	//	printf("open port %s success\n", bufferPort);
	//}
	//else
	//{
	//	printf("Echec d'ouverture de port\n");
	//	printf("%s", bufferPort);
	//}	

	/* **********set DCB *********** */
	if(com.setupdcb(9600))printf("setupDCB success\n");
	if(com.setuptimeout(1000,500,5000,500,2000))printf("setuptimeout success\n");
	SetCommMask(com.hCom, EV_RXCHAR);

	/**********Definir/nettoyer les buffers***************/
	com.setbuffer();
	com.clearbuffer();




	
	unsigned char nbrobots = 0;

	while (x == 0 && y == 0)
	{
		if (newDataEnable)
		{
			if (id == nbrobots)
			{
				nbrobots++;

				formatChaine = FormatTrame(id, cmd, x, y);
				SendTrame(com, formatChaine);
				printf("id : %d, cmd : %d, x : %d, y : %d\n", id, cmd, x, y);
			
				if (id >= 1)
				{
					cmd = 1;
					id = id - 1;
					formatChaine = FormatTrame(id, cmd, x, y);
					SendTrame(com, formatChaine);
					printf("id : %d, cmd : %d, x : %d, y : %d\n", id, cmd, x, y);
				}
			}
			newDataEnable = false;
		}
	}

	newDataEnable = true;

	for (int i = 0; i < nbrobots; i++)
	{
		id = i;
		cmd = 1;
		formatChaine = FormatTrame(id, cmd, x, y);
		SendTrame(com, formatChaine);
		printf("id : %d, cmd : %d, x : %d, y : %d\n", id, cmd, x, y);
	}
	newDataEnable = false;
	


	cmd = 2;

	//id = 0; x = 300, y = 300;

	while (WindowAlive)
	{
		if (writeRead || newDataEnable)
		{
			if (writeRead)
			{
				newDataEnable = true;
				CommandePosCible++;
				formatChaine = FormatTrame(id, cmd, x, y);
				SendTrame(com, formatChaine);
				printf("id : %d, cmd : %d, x : %d, y : %d\n", id, cmd, x, y);
				if (CommandePosCible == 5)
				{
					CommandePosCible = 0;
					cmd = 2;
					writeRead = false;
				}
				newDataEnable = false;

				//printf("\n\n\n\n\nHELLO WORLD\n\n\n\n\n");
			}
			else
			{
				formatChaine = FormatTrame(id, cmd, x, y);
				SendTrame(com, formatChaine);
				printf("id : %d, cmd : %d, x : %d, y : %d\n", id, cmd, x, y);
				newDataEnable = false;
			}
		}
	}	

	com.close();
	system("pause");
}