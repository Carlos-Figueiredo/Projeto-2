#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int main(){

    int sockfd = 0, n = 0;
    char recvBuff[8000];
    char sendBuff[8000];
    struct sockaddr_in serv_addr;
    memset(recvBuff, '0', sizeof(recvBuff));

    // Checa se é possível abrir o socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        return 1;
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    //Porta 5000, 127.0.0.1 localhost
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //Loop infinito até que o usuário interrompa
    socklen_t len = sizeof(serv_addr);
    while (1) { 

        //Leitura de entrada do cliente e envio
        scanf(" %[^\n]s", sendBuff);
        sendto(sockfd, (const char *)sendBuff, strlen(sendBuff), 0, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
        
        //Recebimento da mensagem
        n = recvfrom(sockfd, (char *)recvBuff, 8000, 0, (struct sockaddr *) &serv_addr, &len);
        recvBuff[n] = '\0';
        printf("%s", recvBuff);
    }
    return 0;
}

