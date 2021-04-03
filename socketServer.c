#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

int main() {
	int listenfd = 0, connfd = 0;

	struct sockaddr_in serv_addr;

	char sendBuff[1025];
	char recvBuff[1024];
	int numrv;
	int n;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("Socket Criado!\n");

	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(sendBuff, '0', sizeof(sendBuff));
 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);

	bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	if (listen(listenfd, 10) == -1) {
		printf("Failed to listen\n");
		return -1;
	}

	while (1){
		connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
		while ((n = read(connfd, recvBuff, sizeof(recvBuff) - 1)) > 0){
			recvBuff[n] = 0;
			printf("%s \n", recvBuff);
			if (strcmp(recvBuff, "1") == 0){
				strcpy(sendBuff, "Voce mandou 1");
			}else{
				strcpy(sendBuff, "Voce nao mandou 1");
			}
			printf("%s \n", sendBuff);
			write(connfd, sendBuff, strlen(sendBuff));
		}
	}
	return 0;
}