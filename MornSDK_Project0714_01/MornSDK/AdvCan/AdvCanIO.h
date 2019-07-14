#pragma once

#include "AdvCan.h"

#define SUCCESS                           0                                       //Status definition : success
#define OPERATION_ERROR                   -1                                      //Status definition : device error or parameter error
#define TIME_OUT                          -2                                      //Status definition : time out

//
HANDLE acCanOpen(TCHAR *PortName, BOOL synchronization=FALSE);

//
int acCanClose(HANDLE hDevice);

//
int acEnterResetMode(HANDLE hDevice);

//
int acEnterWorkMode(HANDLE hDevice);

//
int acClearRxFifo(HANDLE hDevice);

//
int acSetBaud(HANDLE hDevice, unsigned int nBaud);

//
int acSetBaudRegister(HANDLE hDevice, unsigned char Btr0, unsigned char Btr1);

//
int acSetTimeOut(HANDLE hDevice, ULONG ulReadTimeOut, ULONG ulWriteTimeOut);

//
int acSetSelfReception(HANDLE hDevice, BOOL bSelfFlag);

//
int acSetListenOnlyMode(HANDLE hDevice, BOOL bListerOnlyFlag);

//
int acSetAcceptanceFilterMode(HANDLE hDevice, int nFilterMode);

//
int acSetAcceptanceFilterCode(HANDLE hDevice, ULONG ulAcceptanceCode);

//
int acSetAcceptanceFilterMask(HANDLE hDevice, ULONG ulAcceptanceMask);

//
int acSetAcceptanceFilter(HANDLE hDevice, ULONG ulAcceptanceCode, ULONG ulAcceptanceMask);

//
int acCanWrite(HANDLE hDevice, canmsg_t *pbyData, ULONG ulWriteCount, ULONG *pulNumberofWritten, OVERLAPPED *ov);

//
int acCanRead(HANDLE hDevice, canmsg_t *pbyData, ULONG ulReadCount, ULONG *pulNumberofRead, OVERLAPPED *ov);

//
int acGetStatus(HANDLE hDevice, CanStatusPar_t* CanStatus);

//
BOOL acClearCommError(HANDLE hDevice, ULONG *ulErrorCode);

//
BOOL acSetCommMask(HANDLE hDevice, ULONG ulEvtMask);

//
BOOL acGetCommMask(HANDLE hDevice, ULONG *ulEvtMask);

//
int acWaitEvent(HANDLE hDevice, canmsg_t *pbyData, ULONG ulReadCount, ULONG *pulNumberofRead, ULONG *ErrorCode, OVERLAPPED *ov);