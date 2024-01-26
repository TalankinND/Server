#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;


int main(void)
{

	const char IP_SERV[] = "127.0.0.1";			
	const int PORT_NUM = 12;				
	const short BUFF_SIZE = 1024;
	const char* txt = ".txt";

	int erStat;								

	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, IP_SERV, &ip_to_num);

	if (erStat <= 0) {
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;
	}



	WSADATA wsData;

	erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (erStat != 0) {
		cout << "Error WinSock version initializaion #";
		cout << WSAGetLastError();
		return 1;
	}
	else
		cout << "WinSock initialization is OK" << endl;


	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ServSock == INVALID_SOCKET) {
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Server socket initialization is OK" << endl;


	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));	

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(PORT_NUM);

	erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Binding socket to Server info is OK" << endl;


	erStat = listen(ServSock, SOMAXCONN);

	if (erStat != 0) {
		cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Listening..." << endl;
	}


	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));	

	int clientInfo_size = sizeof(clientInfo);

	SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

	if (ClientConn == INVALID_SOCKET) {
		cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Connection to a client established successfully" << endl;
		char clientIP[22];

		inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);	

		cout << "Client connected with IP address " << clientIP << endl;

	}

	vector <char> directory(BUFF_SIZE), clientBuff(BUFF_SIZE), fileContent(BUFF_SIZE);
	short packet_size = 0;											

	while (true) {
		packet_size = recv(ClientConn, directory.data(), directory.size(), 0);
		cout << "Client's message: " << directory.data() << endl;

		string str = directory.data() + (string)".txt";

		packet_size = recv(ClientConn, fileContent.data(), fileContent.size(), 0);
		cout << "Client's message: " << fileContent.data() << endl;

		ofstream out;
		out.open(str);
		if (out.is_open())
		{
			out << fileContent.data() << endl;
		}
		out.close();
		cout << "File has been written" << std::endl;

		packet_size = send(ClientConn, "Message recieved", size("Message recieved"), 0);

		cout << "Exit? (y/n): ";
		fgets(clientBuff.data(), clientBuff.size(), stdin);

		if (clientBuff[0] == 'y' ) {
			shutdown(ClientConn, SD_BOTH);
			closesocket(ServSock);
			closesocket(ClientConn);
			WSACleanup();
			return 0;
		}

	}

	closesocket(ServSock);
	closesocket(ClientConn);
	WSACleanup();

	return 0;

}
