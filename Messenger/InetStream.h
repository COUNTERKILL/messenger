#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

class InetStream
{
public:
	InetStream(const char *host, u_short port);
	virtual ~InetStream();

	// Функция Получает IP по имени домена и пытается подключиться
	virtual int Connect();
	virtual void Disconnect();
	// Функция, гарантирующая доставку всего массива
	virtual bool Send(const char *data, const size_t dataLength);
protected:
	int GetIP();
	bool IsConnected();
	SOCKET Sock;
private:
	char *host;
	u_short port;
	sockaddr_in dstAdress;
	bool connected;
};