#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

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
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#endif

#define httpRequest struct http_req
#define httpResponse struct http_res

struct http_req
{
	char *hbStr;
	void *resBox;
	struct sockaddr_in target_addr;
};

struct http_res
{
	char *hbStr;
	void *resBox;
};

struct sockaddr_in initAddr_shd (long unsigned int ip, int port)
{
	struct sockaddr_in target_addr;
	target_addr.sin_family = AF_INET;
	target_addr.sin_addr.s_addr = ip;
	target_addr.sin_port = htons (port);

	return target_addr;
}


struct sockaddr_in initAddr (const char *ip, int port)
{
	return initAddr_shd (inet_addr(ip), port);
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

int sockConn (int *s_fd, struct sockaddr_in *target_addr)
{
	return connect (*s_fd, (struct sockaddr *)target_addr, sizeof (*target_addr));
}

int listenSocket(int s_fd, int port)
{
	struct sockaddr_in srv_addr = initAddr ("0.0.0.0", port);
	int s_len = sizeof (srv_addr);
	if (bind (s_fd, (struct sockaddr *)&srv_addr, s_len) < 0)
	{
		printf ("bind error, ");
		goto error;
	}

	if (listen (s_fd, 5) < 0)
	{
		printf ("listen error, ");
		goto error;
	}
	return accept (s_fd, (struct sockaddr *)&srv_addr, &s_len);
error:
	printf ("%s:%d\n", strerror(errno),errno);
	return -1;
}

void closeSocket (int *s_fd)
{
	printf ("_close\n");
	close (*s_fd);
#ifdef _WIN32
	WSACleanup ();
#endif
}


#ifdef _WIN32
void onConn (DWORD WINAPI Callback(LPVOID sender), void *args)
{
	DWORD th_id;
	HANDLE th_hdl = CreateThread (NULL, 0, Callback, args, 0, &th_id);
	CloseHandle (th_hdl);
#else
void onConn (void *Callback (void *sender), void *args)
{
	pthread_t th_id;


	if (pthread_create (&th_id, NULL, Callback, args) < 0)
	{
		printf ("Something wrong!\n");
	}
	pthread_detach (th_id);
//	pthread_join (th_id, NULL);
#endif
}

uint find_str (const char *source, const char *target, uint sLen, uint tLen, uint sIndex)
{
	if (sLen == -1)
		sLen = strlen (source);
	if (tLen == -1)
		tLen = strlen (target);
		
	uint fIndex = 0;
	for (; sIndex <= sLen - tLen + fIndex && fIndex != tLen; fIndex = source[sIndex] == target[fIndex] ? fIndex + 1 : 0, sIndex++);
	return fIndex == tLen ? sIndex - tLen : -1;
}

char *sub_str (char *source, uint st, uint ed)
{
	char *result = (char *)malloc (ed - st + 1);
	for (uint index = st; index < ed; result [index - st] = source [index], index++);
	result [ed - st] = '\0';

	return result;
}

char *replace_str (char *source, const char *oldStr, const char *newStr, uint sIndex, bool isAll)
{
	uint sLen = strlen (source), oLen = strlen (oldStr), nLen = strlen (newStr), fIndex;

	if ((fIndex = find_str (source, oldStr, sLen, oLen, sIndex)) != -1 && sIndex < sLen)
	{
		char *tmp, *result = (char *)malloc (1024);
		do
		{
			strcat (result, tmp = sub_str (source, sIndex, fIndex));
			free (tmp);
			tmp = NULL;
			strcat (result, newStr);
			sIndex = fIndex + oLen;
		} while (isAll && sIndex + oLen && (fIndex = find_str (source, oldStr, sLen, oLen, sIndex)) != -1);
		if (sIndex + oLen < sLen)
		{
			strcat (result, tmp = sub_str (source, sIndex, sLen));
			free (tmp);
			tmp = NULL;
		}
		free (source);
		source = result;
	}

	return source;
}

char *afterLast (char *source, const char s)
{
	uint fIndex = - 1, i = 0;
	for (; source [i] != '\0'; i++)
		if (source[i] == s)
			fIndex = i;

	return fIndex == -1 ? source : source + fIndex + 1;
}

char *combine_str (char *s1, char *s2)
{
	char result[10240];
	sprintf (result, "%s%s", s1, s2);
	char *presult = result;
	return presult;
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


