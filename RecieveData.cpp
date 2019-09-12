#include "RecieveData.h"

void ReceptionData(bool& newDataEnable, int& id, int& x, int& y)
{
	HANDLE hPipe;
	int buffer[4];
	DWORD dwRead;

	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\DetectionRobot"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		1,
		4 * 32,
		4 * 32,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);


	while (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
		{
			while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
			{
				if (!newDataEnable)
				{
					/* add terminating zero */
					buffer[dwRead] = '\0';

					printf("\n");
					printf("id : %d\nx : %d\ny : %d\n", buffer[2], buffer[0], buffer[1]);

					newDataEnable = true;
					id = buffer[2];
					x = buffer[0];
					y = buffer[1];
				}
			}
		}

		DisconnectNamedPipe(hPipe);
	}
}

int FormatTrame(int& id, int& cmd, int& x, int& y)
{
	return (id << 28) | (cmd << 24) | (x << 12) | y;
}

void SendTrame(COM& com, int formatChaine)
{
	for (unsigned char i = 0; i < 4; i++)
	{
		com.write(formatChaine);
		formatChaine = formatChaine >> 8;
		printf("Trame envoyee\n");
	}
}

//       GESTION TRAME 24 bits 
//int FormatTrame(int& id, int& cmd, int& x, int& y)
//{
//	return (id << 20) | (cmd << 18) | (x << 9) | y;
//}
//
//void SendTrame(COM& com, int formatChaine)
//{
//	for (unsigned char i = 0; i < 3; i++)
//	{
//		com.write(formatChaine);
//		formatChaine = formatChaine >> 8;
//		printf("Trame envoyee\n");
//	}
//}

