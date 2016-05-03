#include "MeshServer.h"

MeshServer::MeshServer(PCSTR portNum)
{
	PORT_NUM = portNum;
	vertex = nullptr;
	normal = nullptr;
	texture = nullptr;
	indexArray = nullptr;
	vertexSize = 0;
	normalSize = 0;
	textureSize = 0;
	indexSize = 0;
}

void MeshServer::setVertexBuffer(glm::vec3* vArray, int aSize){
	vertex = vArray;
	vertexSize = aSize;
}

void MeshServer::setNormalBuffer(glm::vec3* nArray, int aSize){
	normal = nArray;
	normalSize = aSize;
}

void MeshServer::setTextrueBuffer(glm::vec2* tArray, int aSize){
	texture = tArray;
	textureSize = aSize;
}

void MeshServer::setIndexBuffer(glm::vec3* iArray, int aSize){
	indexArray = iArray;
	indexSize = aSize;
}

bool MeshServer::sendFrame(){

	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	char sendbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	int index;
	//char filename[DEFAULT_BUFLEN];
	//memset(filename, 0, sizeof(filename));

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	
	// Resolve the server address and port
	iResult = getaddrinfo(NULL, PORT_NUM, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return false;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	// No longer need server socket
	closesocket(ListenSocket);
	bool init = false;
	int vI = 0; //vertext counter
	int tI = 0; //texture counter
	int nI = 0; //normal counter
	int iI = 0; //index counter

	// Receive until the peer shuts down the connection
	do {
		// Cleanup buffer char[]
		memset(sendbuf, 0, sizeof(sendbuf));
		memset(recvbuf, 0, sizeof(recvbuf));
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

		if (iResult > 0) {

			// Send total #line in .obj file
			if (!init){

				//printf("Receive string = %s\n", recvbuf);
				sprintf_s(sendbuf, "%d;%d;%d;%d\n", vertexSize,normalSize,textureSize,indexSize);
				printf("Send string = %s\n", sendbuf);
				iSendResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return false;
				}
				//printf("Send Bytes = %d\n", iSendResult);
				init = true;
			}
			else{

				int reqNum = atoi(recvbuf);
				//printf("Request Line Number = %d\n", linenum);
				string data;
				if (reqNum < vertexSize){
					data = to_string(vertex[reqNum].x) + " " + to_string(vertex[reqNum].y) + " " + to_string(vertex[reqNum].z);
				}
				else if (reqNum >= vertexSize && reqNum < vertexSize + normalSize){
					int index = reqNum - vertexSize;
					data = to_string(normal[index].x) + " " + to_string(normal[index].y) + " " + to_string(normal[index].z);
				}
				else if (reqNum >= vertexSize + normalSize && reqNum < vertexSize + normalSize + textureSize){
					int index = reqNum - vertexSize - normalSize;
					data = to_string(texture[index].x) + " " + to_string(texture[index].y);
				}
				else{
					int j = reqNum - vertexSize - normalSize - indexSize;
					data = to_string(indexArray[j].x) + " " + to_string(indexArray[j].y) + " " + to_string(indexArray[j].z);
				}

				strncpy_s(sendbuf, data.c_str(), sizeof(sendbuf));
				//printf("Send String = %s\n", sendbuf);
				iSendResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return false;
				}

				//printf("Send Bytes = %d\n", iSendResult);
			}


		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else  {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return false;
		}

	} while (iResult > 0);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return false;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
	return true;
}