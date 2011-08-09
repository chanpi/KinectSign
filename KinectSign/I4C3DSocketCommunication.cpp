#include "StdAfx.h"
#include "I4C3DSocketCommunication.h"
#include "KinectSign.h"

I4C3DSocketCommunication::I4C3DSocketCommunication(HWND hMainWnd)
{
	m_socketHandler = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(22202);
	m_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(m_socketHandler, (const SOCKADDR*)&m_addr, sizeof(m_addr));

	if (WSAAsyncSelect(m_socketHandler, hMainWnd, WSOCK_SELECT, FD_READ) == SOCKET_ERROR) {
		MessageBox(hMainWnd, _T("[ERROR] WSAAsyncSelect()"), _T("KinectSign"), MB_ICONERROR | MB_OK);
	}
}


I4C3DSocketCommunication::~I4C3DSocketCommunication(void)
{
	closesocket(m_socketHandler);
}
