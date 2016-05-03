#include "MeshClient.h"

MeshClient::MeshClient(PCSTR num, PCSTR add){
	PORT_NUM = num;
	address = add;
	vertex = nullptr;
	normal = nullptr;
	texture = nullptr;
	index = nullptr;
	success = false;
}

glm::vec3* MeshClient::getVertexBuffer(){
	if (success){
		return vertex;
	}
	else{
		printf("Transmit Error\n");
		return nullptr;
	}
}

glm::vec3* MeshClient::getNormalBuffer(){
	if (success){
		return normal;
	}
	else{
		printf("Transmit Error\n");
		return nullptr;
	}
}

glm::vec2* MeshClient::getTextrueBuffer(){
	if (success){
		return texture;
	}
	else{
		printf("Transmit Error\n");
		return nullptr;
	}
}

int MeshClient::getVertexNum(){
	if (success){
		return vertexSize;
	}
	else{
		printf("Transmit Error\n");
		return 0;
	}
}
int MeshClient::getNormalNum(){
	if (success){
		return normalSize;
	}
	else{
		printf("Transmit Error\n");
		return 0;
	}
}
int MeshClient::getTextrueNum(){
	if (success){
		return textureSize;
	}
	else{
		printf("Transmit Error\n");
		return 0;
	}
}

int MeshClient::getIndexNum(){
	if (success){
		return indexSize;
	}
	else{
		printf("Transmit Error\n");
		return 0;
	}
}

glm::vec3* MeshClient::getIndexBuffer(){
	if (success){
		return index;
	}
	else{
		printf("Transmit Error\n");
		return nullptr;
	}
}

bool MeshClient::loadFrame(){

	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char sendbuf[DEFAULT_BUFLEN];
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(address, PORT_NUM, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return false;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return false;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return false;
	}

	// Send an initial request for obj file
	string inputBuf ="Model Request";
	strncpy_s(sendbuf, inputBuf.c_str(), sizeof(sendbuf));
	sendbuf[sizeof(sendbuf)-1] = 0;

	// Send request
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return false;
	}

	// Receive total #line in .obj file
	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0)
		printf("Bytes received: %d\n", iResult);
	else if (iResult == 0)
		printf("Connection closed\n");
	else
		printf("recv failed with error: %d\n", WSAGetLastError());

	//int line = atoi(recvbuf);
	//printf("line number = %d\n", line);
	sscanf_s(recvbuf, "%d;%d;%d;%d\n", &vertexSize,&normalSize, &textureSize,&indexSize);
	
	vertex = (glm::vec3*)malloc(sizeof(glm::vec3)*vertexSize);
	normal = (glm::vec3*)malloc(sizeof(glm::vec3)*normalSize);
	texture = (glm::vec2*)malloc(sizeof(glm::vec2)*textureSize);
	index = (glm::vec3*)malloc(sizeof(glm::vec3)*indexSize);
	
	// Receive until the peer closes the connection
	for (int i = 0; i < vertexSize+normalSize+textureSize+indexSize; i++){

		// Cleanup buffer char[]
		memset(sendbuf, 0, sizeof(sendbuf));
		memset(recvbuf, 0, sizeof(recvbuf));

		// Request #line of data
		sprintf_s(sendbuf, "%3d", i);
		iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);

		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return false;
		}

		//printf("#line %d\n", i);

		// Receive data from server
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0){
			//printf("String received: %s\n", recvbuf);
			//printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

		// Write to file
		//printf("%s\n",recvbuf);
		if (i < vertexSize){
			sscanf_s(recvbuf, "%f %f %f\n", &vertex[i].x, &vertex[i].y, &vertex[i].z);
		}
		else if (i >= vertexSize && i < vertexSize + normalSize){
			int index = i - vertexSize;
			sscanf_s(recvbuf, "%f %f %f\n", &normal[index].x, &normal[index].y, &normal[index].z);
		}
		else if (i >= vertexSize + normalSize && i < vertexSize + normalSize + textureSize){
			int index = i - vertexSize - normalSize;
			sscanf_s(recvbuf, "%f %f\n", &texture[index].x, &texture[index].y);
		}
		else{
			int j = i - vertexSize - normalSize - indexSize;
			sscanf_s(recvbuf, "%f %f %f\n", &index[j].x, &index[j].y, &index[j].z);
		}

	}

	

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return false;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	success = true;
	return true;


}