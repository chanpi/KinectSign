#include "StdAfx.h"
#include "I4C3DSocketCommunication.h"
#include "KinectSign.h"

I4C3DSocketCommunication::I4C3DSocketCommunication(void)
{
	m_socketHandler = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(22202);
	m_addr.sin_addr.S_un.S_addr = INADDR_ANY;
}


I4C3DSocketCommunication::~I4C3DSocketCommunication(void)
{
	closesocket(m_socketHandler);
}
