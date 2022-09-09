#define WIN32_LEAN_AND_MEAN


#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


#define ERR_BUFF_SIZE (128)
#define IP_ADDR_SIZE (20)


#pragma comment(lib, "ws2_32.lib")    // required for 'WSAStartup()' workig


const char* get_ip(const char* asciiName);
void print_socket_error_message(const char* usrDescription, int errCode);


int main()
{
	const char* host_name = "www.google.com";

	const char* ip = get_ip(host_name);
	printf("IP is: %s \n", ip ? ip : "FAILURE");

	return 0;
}


const char* get_ip(const char* asciiName)
{
	const char* result = NULL;
	struct addrinfo* hostInfo = { 0 };
	bool isWsaStarted = false;
	bool isInfoGot = false;
	int errCode = 0;

	do
	{
		// required for 'getaddrinfo()' working
		WSADATA wsaData;
		errCode = WSAStartup(MAKEWORD(2, 2), &wsaData);    // actual win socket version is 2 (see '#pragma comment()' library version)
		if (errCode != 0)
		{
			errCode = WSAGetLastError();
			print_socket_error_message("WSAStartup()", errCode);
			break;
		}
		isWsaStarted = true;
		
		// not necessary
		struct addrinfo hints = { 0 };
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;

		// main work = get all info about given name
		errCode = getaddrinfo(asciiName, NULL, &hints, &hostInfo);
		if (errCode != 0)
		{
			errCode = WSAGetLastError();
			print_socket_error_message("getaddrinfo()", errCode);
			break;
		}
		isInfoGot = true;

		// retrieve ip from info was got 
		char* ip = (char*)calloc(IP_ADDR_SIZE, sizeof(char));    // have to free allocated memory in caller
		struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)hostInfo->ai_addr;
		result = inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ip, IP_ADDR_SIZE);
		if (!result)
		{
			errCode = WSAGetLastError();
			print_socket_error_message("inet_ntop()", errCode);
			free(ip);
		}

	} while (0);

	if (isWsaStarted)
	{
		errCode = WSACleanup();
		if (errCode != 0)
		{
			errCode = WSAGetLastError();
			print_socket_error_message("Warning! WSACleanup", errCode);
		}
	}

	if (isInfoGot)
	{
		freeaddrinfo(hostInfo);
	}

	return result;
}


void print_socket_error_message(const char* usrDescription, int errCode)
{
	char errMessage[ERR_BUFF_SIZE] = { 0 };
	
	DWORD isFormated = FormatMessage(
							FORMAT_MESSAGE_FROM_SYSTEM, 
							NULL, 
							errCode, 
							MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), 
							errMessage,
							ERR_BUFF_SIZE, 
							NULL
						);

	printf("%s failed with code: %d. %s \n", usrDescription, errCode, errMessage); 
}
