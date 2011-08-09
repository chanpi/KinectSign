#pragma once
class I4C3DSocketCommunication
{
public:
	I4C3DSocketCommunication(HWND hMainWnd);
	~I4C3DSocketCommunication(void);

	SOCKET m_socketHandler;	// メッセージループで受け取れるようにするためpublic
private:
	SOCKADDR_IN m_addr;
};

