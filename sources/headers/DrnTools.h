#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define bool unsigned
#define true 1
#define false 0
#define uchar unsigned char
#define uint unsigned int

#define writeFile(fp, wText) fprintf(fp, "%s", wText)

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#endif

struct sockaddr_in initAddr (char *ip, int port)
{
	struct sockaddr_in target_addr;
	target_addr.sin_family = AF_INET;
	target_addr.sin_addr.s_addr = inet_addr (ip);
	target_addr.sin_port = htons (port);

	return target_addr;
}

int initSocket ()
{
#ifdef win32
	WSADATA wsaData;
	if (WSAStartup (MAKEWORD (2, 2), &wsaData) != 0)
		return -1;
#endif
	int socket_fd;
	socket_fd = socket (AF_INET, SOCK_STREAM, 0);

	return socket_fd;
}

int sockConn (int s_fd, struct sockaddr_in target_addr)
{
	return connect (s_fd, (struct sockaddr *)&target_addr, sizeof (target_addr));
}

int listenSocket(int s_fd, int port)
{
	struct sockaddr_in srv_addr = initAddr ("0.0.0.0", port);
	if (bind (s_fd, (struct sockaddr *)&srv_addr, sizeof (srv_addr)) < 0)
		return -1;

	return listen (s_fd, 5);
}

void closeSocket (int s_fd)
{
	close (s_fd);
#ifdef _WIN32
	WSACleanup ();
#endif
}


#ifdef _WIN32
void onConn (DWORD WINAPI Callback(LPVOID sender))
{
	DWORD th_id;
	HANDLE th_hdl = CreateThread (NULL, 0, Callback, NULL, 0, &th_id);

	CloseHandle (th_hdl);
#else
void onConn (void *Callback (void *sender))
{
	pthread_t th_id;
	int r_code = pthread_create (&th_id, NULL, Callback, NULL);
	if (r_code < 0)
	{
		printf ("Something wrong!\n");
	}
#endif
}

uint find_str (char *source, char *target, uint sLen, uint tLen, uint sIndex)
{
	if (sLen == -1)
		sLen = strlen (source);
	if (tLen == -1)
		tLen = strlen (target);
		
	uint fIndex = 0;
	for (; sIndex <= sLen - tLen + fIndex && fIndex != tLen; fIndex = source[sIndex] == target[fIndex] ? fIndex + 1 : 0, sIndex++);
	return fIndex == tLen ? sIndex - tLen : -1;
}

char *afterLast (char *source, char s)
{
	uint fIndex = - 1, i = 0;
	for (; source [i] != '\0'; i++)
		if (source[i] == s)
			fIndex = i;

	return fIndex == -1 ? source : source + fIndex + 1;
}
/*
void writeFile (FILE *fp, char *wText)
{
	fprintf (fp, "%s", wText);
}
*/
void writeFileFrStr (char *path, char *wText)
{
	FILE *fp = fopen (path, "wb");
	writeFile (fp, wText);

	fclose (fp);
}

char *readFile (FILE *fp)
{
	uchar *result = (char*)malloc (40960);
	uint i = 0;

	while ((result[i++] = fgetc (fp)) != (uchar)EOF && i < 40960);

	result[i - 1] = '\0';

	return result;
}

char *readFileFrStr (char *path)
{
	FILE *fp = fopen (path, "r");
	char *result = readFile (fp);

	fclose (fp);
	return result;
}
