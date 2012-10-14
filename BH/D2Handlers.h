#pragma once
#include <Windows.h>

void GameDraw();
void GameAutomapDraw();
void OOGDraw();

void GameLoop();
DWORD WINAPI GameThread(VOID* lpvoid);
LONG WINAPI GameWindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL ChatPacketRecv(DWORD dwSize,BYTE* pPacket);
BOOL __fastcall RealmPacketRecv(BYTE* pPacket);
DWORD __fastcall GamePacketRecv(BYTE* pPacket, DWORD dwSize);

DWORD __fastcall GameInput(wchar_t* wMsg);
DWORD __fastcall ChannelInput(wchar_t* wMsg);
BOOL __fastcall ChatHandler(char* user, char* msg);