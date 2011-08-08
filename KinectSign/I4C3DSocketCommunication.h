#pragma once
class I4C3DSocketCommunication
{
public:
	I4C3DSocketCommunication(void);
	~I4C3DSocketCommunication(void);

private:
	SOCKET m_socketHandler;
	SOCKADDR_IN m_addr;
};

