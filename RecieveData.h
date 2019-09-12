#ifndef _RECIEVEDATA_H
#define _RECIEVEDATA_H
#include "CcomPort.h"

int FormatTrame(int& id, int& cmd, int& x, int& y);
void SendTrame(COM& com, int formatChaine);
void ReceptionData(bool& newDataEnable, int& id, int& x, int& y);


#endif // !_RECIEVEDATA_H
