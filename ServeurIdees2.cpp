// ServeurIdees2.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#ifndef UNICODE
#define UNICODE
#endif
#define WIN32_LEAN_AND_MEAN
#include "pch.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <stdio.h>
#include "partie.h"

//code symbole => reponse cr / ro
//sendCase => scxy repondre o si OK n sinon
//receiveCase => envoyer rcxy ou rcxyf si partie finie
//


#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#pragma comment(lib, "Ws2_32.lib")

#define PORT 5150

#define DATA_BUFSIZE 8192

using namespace std;




typedef struct _SOCKET_INFORMATION {
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	SOCKET Socket;
	WSAOVERLAPPED Overlapped;
	DWORD BytesSEND;
	DWORD BytesRECV;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;



DWORD WINAPI ProcessIO(LPVOID lpParameter);
DWORD EventTotal = 0;
WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
LPSOCKET_INFORMATION SocketArray[WSA_MAXIMUM_WAIT_EVENTS];
CRITICAL_SECTION CriticalSection;



int main(int argc, char **argv)

{

	Partie p;
	int l_i = 0;
	WSADATA wsaData;
	SOCKET ListenSocket, AcceptSocket;
	SOCKADDR_IN InternetAddr;
	DWORD Flags;
	DWORD ThreadId;
	DWORD RecvBytes;


	InitializeCriticalSection(&CriticalSection);



	if (WSAStartup((2, 2), &wsaData) != 0)
	{
		printf("WSAStartup() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else
		printf("WSAStartup() fonctionne!\n");



	if ((ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		printf("Failed to get a socket %d\n", WSAGetLastError());
		return 1;
	}
	else
		printf("WSASocket() fonctionne!\n");



	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(PORT);



	if (bind(ListenSocket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		printf("bind() failed with error %d\n", WSAGetLastError());
		return 1;
	}
	else
		printf("bind() fonctionne!\n");



	if (listen(ListenSocket, 5))
	{
		printf("listen() failed with error %d\n", WSAGetLastError());
		return 1;
	}
	else
		printf("listen() fonctionne\n");



	// Setup the listening socket for connections

	if ((AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		printf("Failed to get a socket %d\n", WSAGetLastError());
		return 1;
	}
	else
		printf("WSASocket() fonctionne!\n");


	if ((EventArray[0] = WSACreateEvent()) == WSA_INVALID_EVENT)
	{
		printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
		return 1;
	}
	else
		printf("WSACreateEvent() fonctionne!\n");



	// Create a thread to service overlapped requests

	if (CreateThread(NULL, 0, ProcessIO, NULL, 0, &ThreadId) == NULL)
	{
		printf("CreateThread() failed with error %d\n", GetLastError());
		return 1;
	}
	else
		printf("CreateThread est ok!\n");



	EventTotal = 1;

	int itsize = sizeof(InternetAddr);

	while (TRUE)
	{
		// Accept inbound connections
		if ((AcceptSocket = accept(ListenSocket, (sockaddr*)&InternetAddr, &itsize)) == INVALID_SOCKET)
		{
			printf("accept() failed with error %d\n", WSAGetLastError());
			return 1;
		}
		else
		{
			printf("accept() fonctionne!\n");


			char host[NI_MAXHOST];
			char service[NI_MAXHOST];

			char* liste[NI_MAXHOST];


			ZeroMemory(host, NI_MAXHOST);
			ZeroMemory(service, NI_MAXHOST);

			getnameinfo((sockaddr*)&InternetAddr, itsize, host, NI_MAXHOST, service, NI_MAXSERV, 0);

			std::cout << host << " est connecte sur le port " << service << std::endl;

			//send(AcceptSocket, "salut le client ", 20, 0);

			liste[l_i] = service;

			std::cout << liste[l_i] << std::endl;

			l_i++;

			


		/*	if (l_i == 2)
			{
				
			}
			else
			{
				send(AcceptSocket, "En a, 30, 0);
			}
			*/

		}
			

		char s[] = "";
		recv(AcceptSocket, s, 6, 0);
		cout << s << endl;
		
	/*	if (s == "first\0") {
			send(AcceptSocket, p.tirage(), 10, 0);
		}*/


		send(AcceptSocket, "1", 10, 0);
		EnterCriticalSection(&CriticalSection);

		// Create a socket information structure to associate with the accepted socket

		if ((SocketArray[EventTotal] = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL)
		{
			printf("GlobalAlloc() failed with error %d\n", GetLastError());
			return 1;
		}
		else
			printf("GlobalAlloc() est ok!\n");



		// Fill in the details of our accepted socket

		SocketArray[EventTotal]->Socket = AcceptSocket;
		ZeroMemory(&(SocketArray[EventTotal]->Overlapped), sizeof(OVERLAPPED));
		SocketArray[EventTotal]->BytesSEND = 0;
		SocketArray[EventTotal]->BytesRECV = 0;
		SocketArray[EventTotal]->DataBuf.len = DATA_BUFSIZE;
		SocketArray[EventTotal]->DataBuf.buf = SocketArray[EventTotal]->Buffer;



		if ((SocketArray[EventTotal]->Overlapped.hEvent = EventArray[EventTotal] = WSACreateEvent()) == WSA_INVALID_EVENT)
		{
			printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
			return 1;
		}
		else
			printf("WSACreateEvent() is OK!\n");



		// Post a WSARecv() request to to begin receiving data on the socket

		Flags = 0;

		if (WSARecv(SocketArray[EventTotal]->Socket,
			&(SocketArray[EventTotal]->DataBuf), 1, &RecvBytes, &Flags, &(SocketArray[EventTotal]->Overlapped), NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				printf("WSARecv() failed with error %d\n", WSAGetLastError());
				return 1;
			}
		}
		else
			printf("WSARecv() est ok!\n");




		EventTotal++;
		LeaveCriticalSection(&CriticalSection);



		// Signal the first event in the event array to tell the worker thread to

		// service an additional event in the event array

		if (WSASetEvent(EventArray[0]) == FALSE)
		{
			printf("WSASetEvent() failed with error %d\n", WSAGetLastError());
			return 1;
		}
		else
			printf("WSASetEvent() est OK!\n");
	}



}



DWORD WINAPI ProcessIO(LPVOID lpParameter)
{

	DWORD Index;
	DWORD Flags;
	LPSOCKET_INFORMATION SI;
	DWORD BytesTransferred;
	DWORD i;
	DWORD RecvBytes, SendBytes;



	// Process asynchronous WSASend, WSARecv requests

	while (TRUE)
	{
		if ((Index = WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, WSA_INFINITE, FALSE)) == WSA_WAIT_FAILED)
		{
			printf("WSAWaitForMultipleEvents() failed %d\n", WSAGetLastError());
			return 0;
		}
		else
			printf("WSAWaitForMultipleEvents() est OK!\n");



		// If the event triggered was zero then a connection attempt was made

		// on our listening socket.

		if ((Index - WSA_WAIT_EVENT_0) == 0)
		{
			WSAResetEvent(EventArray[0]);
			continue;
		}



		SI = SocketArray[Index - WSA_WAIT_EVENT_0];
		WSAResetEvent(EventArray[Index - WSA_WAIT_EVENT_0]);



		if (WSAGetOverlappedResult(SI->Socket, &(SI->Overlapped), &BytesTransferred, FALSE, &Flags) == FALSE || BytesTransferred == 0)
		{
			printf("Closing socket %d\n", SI->Socket);
			if (closesocket(SI->Socket) == SOCKET_ERROR)
			{
				printf("closesocket() failed with error %d\n", WSAGetLastError());
			}
			else
				printf("closesocket() est OK!\n");



			GlobalFree(SI);
			WSACloseEvent(EventArray[Index - WSA_WAIT_EVENT_0]);

			// Cleanup SocketArray and EventArray by removing the socket event handle

			// and socket information structure if they are not at the end of the arrays

			EnterCriticalSection(&CriticalSection);



			if ((Index - WSA_WAIT_EVENT_0) + 1 != EventTotal)
				for (i = Index - WSA_WAIT_EVENT_0; i < EventTotal; i++)
				{
					EventArray[i] = EventArray[i + 1];
					SocketArray[i] = SocketArray[i + 1];
				}
			EventTotal--;
			LeaveCriticalSection(&CriticalSection);
			continue;
		}

		// Check to see if the BytesRECV field equals zero. If this is so, then

		// this means a WSARecv call just completed so update the BytesRECV field

		// with the BytesTransferred value from the completed WSARecv() call.

		if (SI->BytesRECV == 0)
		{
			SI->BytesRECV = BytesTransferred;
			SI->BytesSEND = 0;
		}
		else
		{
			SI->BytesSEND += BytesTransferred;
		}



		if (SI->BytesRECV > SI->BytesSEND)
		{

			// Post another WSASend() request.

			// Since WSASend() is not guaranteed to send all of the bytes requested,

			// continue posting WSASend() calls until all received bytes are sent

			ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
			SI->Overlapped.hEvent = EventArray[Index - WSA_WAIT_EVENT_0];
			SI->DataBuf.buf = SI->Buffer + SI->BytesSEND;
			SI->DataBuf.len = SI->BytesRECV - SI->BytesSEND;



			if (WSASend(SI->Socket, &(SI->DataBuf), 1, &SendBytes, 0, &(SI->Overlapped), NULL) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					printf("WSASend() failed with error %d\n", WSAGetLastError());
					return 0;
				}
			}
			else
				printf("WSASend() est OK!\n");
		}
		else
		{
			SI->BytesRECV = 0;

			// Now that there are no more bytes to send post another WSARecv() request

			Flags = 0;
			ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
			SI->Overlapped.hEvent = EventArray[Index - WSA_WAIT_EVENT_0];
			SI->DataBuf.len = DATA_BUFSIZE;
			SI->DataBuf.buf = SI->Buffer;

			if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags, &(SI->Overlapped), NULL) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					printf("WSARecv() failed with error %d\n", WSAGetLastError());
					return 0;
				}
			}
			else
				printf("WSARecv() est OK!\n");
		}
	}

}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Conseils pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.


