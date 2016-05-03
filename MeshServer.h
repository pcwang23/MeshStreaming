#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Include GLM
#include <glm/vec3.hpp> // glm::vec3
#include "glm/ext.hpp"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512

class MeshServer
{
public:
	MeshServer(PCSTR portNum);
	void setVertexBuffer(glm::vec3* vertexArray, int arraySize);
	void setNormalBuffer(glm::vec3* normalArray, int arraySize);
	void setTextrueBuffer(glm::vec2* textureArray, int arraySize);
	void setIndexBuffer(glm::vec3* indexArray, int arraySize);
	bool sendFrame();


private:
	PCSTR PORT_NUM;
	glm::vec3* vertex;
	glm::vec3* normal;
	glm::vec2* texture;
	glm::vec3* indexArray;
	int vertexSize;
	int normalSize;
	int textureSize;
	int indexSize;
};



