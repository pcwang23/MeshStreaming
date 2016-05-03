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
#include <string.h>
#include <vector>

using namespace std;

// Include GLM
#include <glm/vec3.hpp> // glm::vec3
#include "glm/ext.hpp"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512

class MeshClient{
public:
	MeshClient(PCSTR PORT_NUM, PCSTR add);
	glm::vec3* getVertexBuffer();
	glm::vec3* getNormalBuffer();
	glm::vec2* getTextrueBuffer();
	glm::vec3* getIndexBuffer();
	int getVertexNum();
	int getNormalNum();
	int getTextrueNum();
	int getIndexNum();
	bool loadFrame();


private:
	PCSTR PORT_NUM;
	PCSTR address;
	int vertexSize;
	int normalSize;
	int textureSize;
	int indexSize;
	glm::vec3* vertex;
	glm::vec3* normal;
	glm::vec2* texture;
	glm::vec3* index;
	bool success;
};



