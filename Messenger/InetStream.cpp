#include "InetStream.h"

InetStream::InetStream(const char *host, u_short port)
{
	int HostLen = strlen(host);
	this->host = new char[HostLen+1];
	strcpy(this->host, host);
	this->port = port;

	this->dstAdress.sin_family = AF_INET;
	this->connected = false;
}

InetStream::~InetStream()
{
	delete[] this->host;
	if(this->connected)
		this->Disconnect();
}

int InetStream::Connect()
{
	int isError = 0;
	WSADATA wsaData;
	this->connected = true;
	isError = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (isError) 
	{
		throw "Error: Winsock can't startup";
	}
	this->Sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if(this->Sock==INVALID_SOCKET)
	{
		throw "Error: Winsock can't create socketz";
	}
	isError = GetIP();
	if(isError)
	{
		this->connected = false;
		return 1;
		//throw "Error: Winsock can't resolve IP";
	}
	isError=connect(Sock,(sockaddr*)&(this->dstAdress),sizeof(sockaddr));
	if(isError==SOCKET_ERROR)
	{
		this->connected = false;
		return 1;
		//throw "Error: Winsock can't connect to IP";
	}
	return 0;
}

void InetStream::Disconnect()
{
	this->connected = false;
	int isError = 0;
	isError = shutdown(this->Sock,SD_BOTH);
	if(isError)
	{
		return;
		//throw "Error: Winsock can't shutdown socket";
	}
	isError = closesocket(this->Sock);
	if(isError)
	{
		return;
		//throw "Error: Winsock can't close socket";
	}
	isError = WSACleanup();
	if (isError) 
	{
		return;
		//throw "Error: Winsock can't cleanup";
	}
	return;
}

int InetStream::GetIP()
{
	hostent* HIP;
	HIP=gethostbyname(this->host);
	if(HIP==NULL)
		return 1;
	this->dstAdress.sin_port=htons(this->port);
	ZeroMemory(&(this->dstAdress.sin_zero), sizeof(this->dstAdress.sin_zero));
	// копируем адрес, хранящийся в первых 4 байтах
	memcpy(&(this->dstAdress.sin_addr.s_addr),HIP->h_addr_list[0],4);
	return 0;
}

bool InetStream::IsConnected()
{
	return this->connected;
}

bool InetStream::Send(const char *data, const size_t dataLength)
{
	size_t posInData = 0;
	while(posInData<dataLength)
	{
		// если не подключены, подключаемся
		if(!this->connected)
		{
			Sleep(1000);
			this->Connect();
			if(!this->connected)
				throw "Connection error!";
		}
		size_t lenToSend = ((dataLength-posInData)>SO_MAX_MSG_SIZE)?SO_MAX_MSG_SIZE:(dataLength-posInData); // размер отправляемого пакета
		int sendedLen = send(this->Sock, data + posInData, lenToSend, 0);
		if(sendedLen==SOCKET_ERROR)
		{
			this->Disconnect();
		}
		else
		{
			posInData += sendedLen;
		}
	}
	return 0;
}
