#pragma once
class I4C3DSocketCommunication
{
public:
	I4C3DSocketCommunication(HWND hMainWnd);
	~I4C3DSocketCommunication(void);

	SOCKET m_socketHandler;	// ���b�Z�[�W���[�v�Ŏ󂯎���悤�ɂ��邽��public
private:
	SOCKADDR_IN m_addr;
};

