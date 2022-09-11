#define WIN32_LEAN_AND_MEAN

#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#pragma comment(lib, "ws2_32.lib") // required for 'WSAStartup()' workig
	#define GET_SOCK_ERROR() WSAGetLastError()
	#define ERR_BUFF_SIZE	(128)
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#define GET_SOCK_ERROR() (errno)
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define IP_ADDR_SIZE (20)


static char* get_ip(const char* asciiName);
static void print_socket_error_message(const char* usrDescription, int errCode);
static bool is_ip_address(const char* string);


int main()
{
	const char* host_name = "www.google.com";
	if (is_ip_address(host_name))
	{
		printf("IP is: %s \n", host_name);
	}
	else
	{
		char* ip = get_ip(host_name);
		printf("IP is: %s \n", ip ? ip : "FAILURE");

		if (ip != NULL)
		{
			free(ip);
		}
	}

	return 0;
}


char* get_ip(const char* asciiName)
{
	char* result = NULL;
	struct addrinfo* hostInfo = { 0 };
	bool isInfoGot = false;
	int errCode = 0;

#ifdef _WIN32
	bool isWsaStarted = false;
#endif

	do
	{

#ifdef _WIN32

		// required for 'getaddrinfo()' working
		WSADATA wsaData;
		errCode = WSAStartup(MAKEWORD(2, 2), &wsaData);    // actual win socket version is 2 (see '#pragma comment()' library version)
		if (errCode != 0)
		{
			errCode = GET_SOCK_ERROR();
			print_socket_error_message("WSAStartup()", errCode);
			break;
		}
		isWsaStarted = true;

#endif
		
		// not necessary
		struct addrinfo hints = { 0 };
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;

		// main work = get all info about given name
		errCode = getaddrinfo(asciiName, NULL, &hints, &hostInfo);
		if (errCode != 0)
		{

#ifdef _WIN32
			errCode = GET_SOCK_ERROR();
			print_socket_error_message("getaddrinfo()", errCode);
#else
			WARN("getaddrinfo() failed with code %d. %s", errCode, gai_strerror(errCode));
#endif 

			break;
		}
		isInfoGot = true;

		// retrieve ip from info was got 
		char* ip = (char*)calloc(IP_ADDR_SIZE, sizeof(char));    // have to free allocated memory in caller
		struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)hostInfo->ai_addr;
		result = (char*)inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ip, IP_ADDR_SIZE);
		if (!result)
		{
			errCode = GET_SOCK_ERROR();
			print_socket_error_message("inet_ntop()", errCode);
			free(ip);
		}

	} while (0);

#ifdef _WIN32

	if (isWsaStarted)
	{
		errCode = WSACleanup();
		if (errCode != 0)
		{
			errCode = WSAGetLastError();
			print_socket_error_message("Warning! WSACleanup", errCode);
		}
	}

#endif

	if (isInfoGot)
	{
		freeaddrinfo(hostInfo);
	}

	return result;
}


void print_socket_error_message(const char* usrDescription, int errCode)
{
#ifdef _WIN32

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

#else

	char* errMessage = strerror(errCode);

#endif

	printf("%s failed with code: %d. %s \n", usrDescription, errCode, errMessage); 
}


bool is_ip_address(const char* string)
{
	struct in_addr result = { 0 };
	int errCode = inet_pton(AF_INET, string, &result);

	if (errCode == -1)
	{
		errCode = GET_SOCK_ERROR();
		print_socket_error_message("inet_pton()", errCode);
	}
	
	return (errCode > 0) ? true : false;
}
