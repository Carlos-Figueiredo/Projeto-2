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
    char recvBuff[1024];
    char sendBuff[1025];
    struct sockaddr_in serv_addr;
    memset(recvBuff, '0', sizeof(recvBuff));

    // Checa se é possível abrir o socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        return 1;
    }


    //Porta 5000, 127.0.0.1 localhost
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // // Caso o cliente nao conecte ao server
    // if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
    //     return 1;
    // }

    //Loop infinito até que o usuário interrompa

    while (1) { 
        //Leitura de entrada do cliente e envio
        scanf(" %[^\n]s", sendBuff);
        //sendto(int sockfd, const void *buf, int len, Unsigned int flags, const struct sockaddr *to, socklen_t *tolen);
        sendto(sockfd, sendBuff, strlen(sendBuff), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        //write(sockfd, sendBuff, strlen(sendBuff));

        //Recebimento da mensagem
        //recvfrom(int sockfd, void *buf, int len, unsigned int flags, struct sockaddr *from, int *fromlen);
        n = recvfrom(sockfd, recvBuff, sizeof(recvBuff) - 1, 0, NULL, NULL);
        // n = read(sockfd, recvBuff, sizeof(recvBuff) - 1);
        recvBuff[n] = 0;
        printf("%s", recvBuff);
        
    }
    return 0;
}