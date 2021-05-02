#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "cJSON.h"


void printUser(cJSON * user, char * message);

void sendMessage(int connfd, char * message){
	write(connfd, message, strlen(message));
}

void removeQuotes(char * string){
	memmove(string, string + 1, strlen(string));
	string[strlen(string) - 1] = '\0';
}

void receiveMessage(int connfd, char * message){
	char recvBuff[1024];
	int n;
	
	n = read(connfd, recvBuff, sizeof(recvBuff) - 1);
	recvBuff[n] = 0;
	strcpy(message, recvBuff);
	return;
}

void insertUser(int connfd){
	char recvBuff[1024];
	char json[5000];
	int n;
    FILE *filePointer;
	cJSON *userList;
	cJSON *user;

	if (access("users.txt", F_OK) != 0) {
		userList = cJSON_CreateArray();
	}else{
		filePointer = fopen("users.txt", "r");
		fscanf(filePointer, "%[^\0]", json);
		userList = cJSON_Parse(json);
		fclose(filePointer);
	}
	
	cJSON *abilities = cJSON_CreateArray();
	cJSON *experiences = cJSON_CreateArray();
	char *pt;
	
	sendMessage(connfd, "Insira o email:\n");
	receiveMessage(connfd, recvBuff);

	

	cJSON_ArrayForEach(user, userList){
		char * toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "email"));
		removeQuotes(toString);
		if (strcmp(recvBuff, toString) == 0){
			sendMessage(connfd, "Email já utilizado em um perfil.\n\n");
			return;
		}
	}

	user = cJSON_CreateObject();
	cJSON_AddStringToObject(user, "email", recvBuff);


	sendMessage(connfd, "Insira o nome:\n");
	receiveMessage(connfd, recvBuff);
	cJSON_AddStringToObject(user, "name", recvBuff);
	
	sendMessage(connfd, "Insira o sobrenome:\n");
	receiveMessage(connfd, recvBuff);
	cJSON_AddStringToObject(user, "lastName", recvBuff);

	sendMessage(connfd, "Insira o endereço:\n");
	receiveMessage(connfd, recvBuff);
	cJSON_AddStringToObject(user, "address", recvBuff);

	sendMessage(connfd, "Insira a formação:\n");
	receiveMessage(connfd, recvBuff);
	cJSON_AddStringToObject(user, "graduate", recvBuff);

	sendMessage(connfd, "Insira o ano de formatura:\n");
	receiveMessage(connfd, recvBuff);
	cJSON_AddStringToObject(user, "year", recvBuff);

	cJSON_AddItemToObject(user, "abilities", abilities);
	sendMessage(connfd, "Insira suas habilidades:\n");
	receiveMessage(connfd, recvBuff);
	pt = strtok(recvBuff,",");
    while (pt != NULL) {
		if (pt[0] == ' ') 
    		memmove(pt, pt+1, strlen(pt));
		cJSON_AddItemToArray(abilities, cJSON_CreateString(pt));
        pt = strtok (NULL, ",");
    }

	cJSON_AddItemToObject(user, "experience", experiences);
	sendMessage(connfd, "Insira a quantidade de experiências anteriores que deseja registrar:\n");
	receiveMessage(connfd, recvBuff);
	n = atoi(recvBuff);
	
	for (int i = 0; i < n; i++){
		sendMessage(connfd, "Insira uma das suas Experiência:\n");
		receiveMessage(connfd, recvBuff);
		cJSON_AddItemToArray(experiences, cJSON_CreateString(recvBuff));
	}

	if (access("users.txt", F_OK) != 0) {
		cJSON_AddItemToArray(userList, user);
		filePointer = fopen("users.txt", "w");
		fprintf(filePointer, "%s", cJSON_Print(userList));
		fclose(filePointer);
	}
	else{
		filePointer = fopen("users.txt", "r");
		fscanf(filePointer, "%[^\0]", json);
		userList = cJSON_Parse(json);
		fclose(filePointer);
		cJSON_AddItemToArray(userList, user);

		filePointer = fopen("users.txt", "w");
		fprintf(filePointer, "%s", cJSON_Print(userList));
		fclose(filePointer);
	}
	sendMessage(connfd, "Registro Inserido\n\n");
}

void printAllUsers(int connfd){
	FILE *filePointer;
	char sendBuff[1025];
	char json[5000];
	memset(sendBuff, 0, sizeof(sendBuff));
	if (access("users.txt", F_OK) != 0) {
		sendMessage(connfd, "Nenhum perfil cadastrado\n");
		return;
	}
    filePointer = fopen("users.txt", "r");
	fscanf(filePointer, "%[^\0]", json);
	cJSON *userList = cJSON_Parse(json);
	fclose(filePointer);
	cJSON *user = NULL;
	cJSON_ArrayForEach(user, userList){
		printUser(user, sendBuff);
	}
	sendMessage(connfd, sendBuff);
}

void printUser(cJSON * user, char *message){
	cJSON *jsonPointer = NULL;
	cJSON *jsonArray = NULL;
	char * toString;

	toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "email"));
	removeQuotes(toString);
	strcat(message, "Email: ");
	strcat(message, toString);
	strcat(message, "\n");

	toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "name"));
	removeQuotes(toString);
	strcat(message, "Nome: ");
	strcat(message, toString);
	strcat(message, " ");

	toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "lastName"));
	removeQuotes(toString);
	strcat(message, "Sobrenome: ");
	strcat(message, toString);
	strcat(message, "\n");

	toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "address"));
	removeQuotes(toString);
	strcat(message, "Residência: ");
	strcat(message, toString);
	strcat(message, "\n");
	
	toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "graduate"));
	removeQuotes(toString);
	strcat(message, "Formação Acadêmica: ");
	strcat(message, toString);
	strcat(message, "\n");

	toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "year"));
	removeQuotes(toString);
	strcat(message, "Ano de Formatura: ");
	strcat(message, toString);
	strcat(message, "\n");


	strcat(message, "Habilidades: ");	
	jsonArray = cJSON_GetObjectItemCaseSensitive(user, "abilities");
	cJSON_ArrayForEach(jsonPointer, jsonArray){
		toString = cJSON_Print(jsonPointer);
		removeQuotes(toString);
		strcat(message, toString);
		strcat(message, ", ");
	}
	message[strlen(message) - 2] = '\0';
	strcat(message, "\n");

	strcat(message, "Experiências: ");
	int i = 1;	
	jsonArray = cJSON_GetObjectItemCaseSensitive(user, "experience");
	cJSON_ArrayForEach(jsonPointer, jsonArray){
		toString = cJSON_Print(jsonPointer);
		removeQuotes(toString);
		strcat(message, "(");
		char number = i + '0';
		strcat(message, &number);
		strcat(message, ") ");
		strcat(message, toString);
		strcat(message, "\n");
		i++;
	}
	strcat(message, "\n");
}

void removerUser(int connfd){
	FILE *filePointer;
	char sendBuff[1025];
	char recvBuff[1024];
	char json[5000];
	memset(sendBuff, 0, sizeof(sendBuff));

	if (access("users.txt", F_OK) != 0) {
		sendMessage(connfd, "Nenhum perfil cadastrado\n");
		return;
	}

	filePointer = fopen("users.txt", "r");
	fscanf(filePointer, "%[^\0]", json);
	cJSON *userList = cJSON_Parse(json);
	fclose(filePointer);

	cJSON *user = NULL;

	sendMessage(connfd, "Insira o email do perfil que deseja remover:\n");
	receiveMessage(connfd, recvBuff);
	int i = 0;
	cJSON_ArrayForEach(user, userList){
		char * toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "email"));
		removeQuotes(toString);
		if (strcmp(recvBuff, toString) == 0){
			cJSON_DeleteItemFromArray(userList, i);
			sendMessage(connfd, "Perfil deletado\n\n");
			filePointer = fopen("users.txt", "w");
			fprintf(filePointer, "%s", cJSON_Print(userList));
			fclose(filePointer);
			return;
		}
		i++;
	}

	

	sendMessage(connfd, "Perfil não encontrado\n\n");

}

void findUser(int connfd){
	FILE *filePointer;
	char sendBuff[1025];
	char recvBuff[1024];
	char json[5000];
	memset(sendBuff, 0, sizeof(sendBuff));

	if (access("users.txt", F_OK) != 0) {
		sendMessage(connfd, "Nenhum perfil cadastrado\n");
		return;
	}

	filePointer = fopen("users.txt", "r");
	fscanf(filePointer, "%[^\0]", json);
	cJSON *userList = cJSON_Parse(json);
	fclose(filePointer);

	cJSON *user = NULL;

	sendMessage(connfd, "Insira o email do perfil:\n");
	receiveMessage(connfd, recvBuff);

	cJSON_ArrayForEach(user, userList){
		char * toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "email"));
		removeQuotes(toString);
		if (strcmp(recvBuff, toString) == 0){
			printUser(user, sendBuff);
			sendMessage(connfd, sendBuff);
			return;
		}
	}
	sendMessage(connfd, "Perfil não encontrado\n\n");
	
}

void printFiltered(int connfd, int filter){
	FILE *filePointer;
	char sendBuff[1025];
	char json[5000];
	char recvBuff[1024];
	memset(sendBuff, 0, sizeof(sendBuff));
	if (access("users.txt", F_OK) != 0) {
		sendMessage(connfd, "Nenhum perfil cadastrado\n");
		return;
	}
    filePointer = fopen("users.txt", "r");
	fscanf(filePointer, "%[^\0]", json);
	cJSON *userList = cJSON_Parse(json);
	fclose(filePointer);
	cJSON *user = NULL;

	//print all profiles of given graduate
	if (filter == 1){
		sendMessage(connfd, "Insira a formação acadêmica que deseja buscar:\n");
		receiveMessage(connfd, recvBuff);
		strcat(sendBuff, "Perfis com formação em ");
		strcat(sendBuff, recvBuff);
		strcat(sendBuff, ":\n");
		
		cJSON_ArrayForEach(user, userList){
			char * toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "graduate"));
			removeQuotes(toString);
			if (strcmp(recvBuff, toString) == 0){
				toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "email"));
				removeQuotes(toString);
				strcat(sendBuff, "Email: ");
				strcat(sendBuff, toString);
				strcat(sendBuff, "\n");

				toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "name"));
				removeQuotes(toString);
				strcat(sendBuff, "Nome: ");
				strcat(sendBuff, toString);

				toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "lastName"));
				removeQuotes(toString);
				strcat(sendBuff, " Sobrenome: ");
				strcat(sendBuff, toString);
				strcat(sendBuff, "\n\n");
			}
		}
	}
	//print all profiles of given ability
	else if (filter == 2){
		sendMessage(connfd, "Insira a habilidade que deseja buscar:\n");
		receiveMessage(connfd, recvBuff);
		strcat(sendBuff, "Perfis com habilidade em ");
		strcat(sendBuff, recvBuff);
		strcat(sendBuff, ":\n");
		cJSON * abilities;
		cJSON * ability;
		cJSON_ArrayForEach(user, userList){
			abilities = cJSON_GetObjectItemCaseSensitive(user, "abilities");
			cJSON_ArrayForEach(ability, abilities){
				char * toString = cJSON_Print(ability);
				removeQuotes(toString);
				if(strstr(toString, recvBuff) != NULL){
					toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "email"));
					removeQuotes(toString);
					strcat(sendBuff, "Email: ");
					strcat(sendBuff, toString);
					strcat(sendBuff, "\n");

					toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "name"));
					removeQuotes(toString);
					strcat(sendBuff, "Nome: ");
					strcat(sendBuff, toString);

					toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "lastName"));
					removeQuotes(toString);
					strcat(sendBuff, " Sobrenome: ");
					strcat(sendBuff, toString);
					strcat(sendBuff, "\n\n");
				}
			}
		}
	}
	//print all profiles of given graduate year
	else if(filter == 3){
		sendMessage(connfd, "Insira o ano de formação acadêmica que deseja buscar:\n");
		receiveMessage(connfd, recvBuff);
		strcat(sendBuff, "Perfis com formação no ano ");
		strcat(sendBuff, recvBuff);
		strcat(sendBuff, ":\n");
		
		cJSON_ArrayForEach(user, userList){
			char * toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "year"));
			removeQuotes(toString);
			if (strcmp(recvBuff, toString) == 0){
				toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "email"));
				removeQuotes(toString);
				strcat(sendBuff, "Email: ");
				strcat(sendBuff, toString);
				strcat(sendBuff, "\n");

				toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "name"));
				removeQuotes(toString);
				strcat(sendBuff, "Nome: ");
				strcat(sendBuff, toString);

				toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "lastName"));
				removeQuotes(toString);
				strcat(sendBuff, " Sobrenome: ");
				strcat(sendBuff, toString);
				strcat(sendBuff, "\n\n");
			}
		}
	}
	strcat(sendBuff, "\n");
	sendMessage(connfd, sendBuff);
}

void addAbility(int connfd){
	FILE *filePointer;
	char sendBuff[1025];
	char recvBuff[1024];
	char json[5000];
	memset(sendBuff, 0, sizeof(sendBuff));

	if (access("users.txt", F_OK) != 0) {
		sendMessage(connfd, "Nenhum perfil cadastrado\n");
		return;
	}

	filePointer = fopen("users.txt", "r");
	fscanf(filePointer, "%[^\0]", json);
	cJSON *userList = cJSON_Parse(json);
	fclose(filePointer);

	cJSON *user = NULL;

	sendMessage(connfd, "Insira o email do perfil:\n");
	receiveMessage(connfd, recvBuff);

	cJSON_ArrayForEach(user, userList){
		char * toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "email"));
		removeQuotes(toString);
		if (strcmp(recvBuff, toString) == 0){
			sendMessage(connfd, "Insira a habilidade que deseja inserir:\n");
			receiveMessage(connfd, recvBuff);
			cJSON * abilities = cJSON_GetObjectItemCaseSensitive(user, "abilities");
			cJSON * ability = NULL;
			cJSON_ArrayForEach(ability, abilities){
				char * toString = cJSON_Print(ability);
				removeQuotes(toString);
				if (strcmp(toString, recvBuff) == 0){
					sendMessage(connfd, "Habilidade já inserida neste perfil!\n");
					return;
				}				
			}

			cJSON_AddItemToArray(abilities, cJSON_CreateString(recvBuff));
			filePointer = fopen("users.txt", "w");
			fprintf(filePointer, "%s", cJSON_Print(userList));
			fclose(filePointer);

			sendMessage(connfd, "Habilidade inserida no perfil!\n");

			return;
		}
	}
	sendMessage(connfd, "Perfil não encontrado\n\n");
	
}

void receiveCommands(int connfd){
	char recvBuff[1024];
	while (1){
		receiveMessage(connfd, recvBuff);
		printf("%s\n", recvBuff);
		if (strcmp(recvBuff, "1") == 0){
			insertUser(connfd);
		}
		else if (strcmp(recvBuff, "2") == 0){
			addAbility(connfd);
		}
		else if (strcmp(recvBuff, "3") == 0){
			printFiltered(connfd, 1);	
		}
		else if (strcmp(recvBuff, "4") == 0){
			printFiltered(connfd, 2);	
		}
		else if (strcmp(recvBuff, "5") == 0){
			printFiltered(connfd, 3);
		}
		else if (strcmp(recvBuff, "6") == 0){
			printAllUsers(connfd);
		}
		else if (strcmp(recvBuff, "7") == 0){
			findUser(connfd);
		}
		else if (strcmp(recvBuff, "8") == 0){
			removerUser(connfd);
		}
		else if (strcmp(recvBuff, "0") == 0){
			return;
		}
		else{
			sendMessage(connfd, "Operacao invalida\n");
		}
	}
}

int main() {

	pid_t pid;
	int connfd = 0;
	int listenfd = 0;
	struct sockaddr_in serv_addr;
	int n;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);

	bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	listen(listenfd, 10);

	while (1){
		connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
		if ( (pid = fork()) == 0 ) {
			close(listenfd);
			receiveCommands(connfd);
			close(connfd);
			exit(0);
		}
	}
	return 0;
}

