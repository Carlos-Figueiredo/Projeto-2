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

void sendMessage(int connfd, struct sockaddr *serv_addr, socklen_t serv_len, char * message){
	sendto(connfd, message, strlen(message), 0, serv_addr, serv_len);
	//write(connfd, message, strlen(message));
}

void removeQuotes(char * string){
	memmove(string, string + 1, strlen(string));
	string[strlen(string) - 1] = '\0';
}

void receiveMessage(int connfd, char * message){
	char recvBuff[1024];
	int n;
	
	//n = read(connfd, recvBuff, sizeof(recvBuff) - 1);
	n = recvfrom(connfd, recvBuff, sizeof(recvBuff) - 1, 0, NULL, NULL);
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
	
	sendMessage(connfd, serv_addr, serv_len, "\nInsira o email:\n");
	receiveMessage(connfd, recvBuff);

	

	cJSON_ArrayForEach(user, userList){
		char * toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "email"));
		removeQuotes(toString);
		if (strcmp(recvBuff, toString) == 0){
			sendMessage(connfd, serv_addr, serv_len, "\nEmail já utilizado em um perfil.\n\n");
			return;
		}
	}

	user = cJSON_CreateObject();
	cJSON_AddStringToObject(user, "email", recvBuff);


	sendMessage(connfd, serv_addr, serv_len, "\nInsira o nome:\n");
	receiveMessage(connfd, recvBuff);
	cJSON_AddStringToObject(user, "name", recvBuff);
	
	sendMessage(connfd, serv_addr, serv_len, "\nInsira o sobrenome:\n");
	receiveMessage(connfd, recvBuff);
	cJSON_AddStringToObject(user, "lastName", recvBuff);

	sendMessage(connfd, serv_addr, serv_len, "\nInsira o endereço:\n");
	receiveMessage(connfd, recvBuff);
	cJSON_AddStringToObject(user, "address", recvBuff);

	sendMessage(connfd, serv_addr, serv_len, "\nInsira a formação:\n");
	receiveMessage(connfd, recvBuff);
	cJSON_AddStringToObject(user, "graduate", recvBuff);

	sendMessage(connfd, serv_addr, serv_len, "\nInsira o ano de formatura:\n");
	receiveMessage(connfd, recvBuff);
	cJSON_AddStringToObject(user, "year", recvBuff);

	cJSON_AddItemToObject(user, "abilities", abilities);
	sendMessage(connfd, serv_addr, serv_len, "\nInsira suas habilidades:\n");
	receiveMessage(connfd, recvBuff);
	pt = strtok(recvBuff,",");
    while (pt != NULL) {
		if (pt[0] == ' ') 
    		memmove(pt, pt+1, strlen(pt));
		cJSON_AddItemToArray(abilities, cJSON_CreateString(pt));
        pt = strtok (NULL, ",");
    }

	cJSON_AddItemToObject(user, "experience", experiences);
	sendMessage(connfd, serv_addr, serv_len, "\nInsira a quantidade de experiências anteriores que deseja registrar:\n");
	receiveMessage(connfd, recvBuff);
	n = atoi(recvBuff);
	
	for (int i = 0; i < n; i++){
		sendMessage(connfd, serv_addr, serv_len, "\nInsira Experiência:\n");
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
	sendMessage(connfd, serv_addr, serv_len, "\nRegistro Inserido\n\n");
}

void printAllUsers(int connfd){
	FILE *filePointer;
	char sendBuff[1025];
	char json[5000];
	memset(sendBuff, 0, sizeof(sendBuff));
	if (access("users.txt", F_OK) != 0) {
		sendMessage(connfd, serv_addr, serv_len, "\nNenhum perfil cadastrado\n");
		return;
	}
    filePointer = fopen("users.txt", "r");
	fscanf(filePointer, "%[^\0]", json);
	cJSON *userList = cJSON_Parse(json);
	fclose(filePointer);
	cJSON *user = NULL;
	strcat(sendBuff,"\nListando todos os perfis:\n\n");
	cJSON_ArrayForEach(user, userList){
		printUser(user, sendBuff);
	}
	sendMessage(connfd, serv_addr, serv_len, sendBuff);
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
		sendMessage(connfd, serv_addr, serv_len, "\nNenhum perfil cadastrado\n");
		return;
	}

	filePointer = fopen("users.txt", "r");
	fscanf(filePointer, "%[^\0]", json);
	cJSON *userList = cJSON_Parse(json);
	fclose(filePointer);

	cJSON *user = NULL;

	sendMessage(connfd, serv_addr, serv_len, "\nRemover um perfil\n\nInsira o email do perfil que deseja remover:\n");
	receiveMessage(connfd, recvBuff);
	int i = 0;
	cJSON_ArrayForEach(user, userList){
		char * toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "email"));
		removeQuotes(toString);
		if (strcmp(recvBuff, toString) == 0){
			cJSON_DeleteItemFromArray(userList, i);
			sendMessage(connfd, serv_addr, serv_len, "\nPerfil deletado\n\n");
			filePointer = fopen("users.txt", "w");
			fprintf(filePointer, "%s", cJSON_Print(userList));
			fclose(filePointer);
			return;
		}
		i++;
	}

	

	sendMessage(connfd, serv_addr, serv_len, "\nPerfil não encontrado\n\n");

}

void findUser(int connfd){
	FILE *filePointer;
	char sendBuff[1025];
	char recvBuff[1024];
	char json[5000];
	memset(sendBuff, 0, sizeof(sendBuff));

	if (access("users.txt", F_OK) != 0) {
		sendMessage(connfd, serv_addr, serv_len, "\nNenhum perfil cadastrado\n");
		return;
	}

	filePointer = fopen("users.txt", "r");
	fscanf(filePointer, "%[^\0]", json);
	cJSON *userList = cJSON_Parse(json);
	fclose(filePointer);

	cJSON *user = NULL;

	sendMessage(connfd, serv_addr, serv_len, "\nProcurar por perfil pelo email\n\nInsira o email do perfil:\n");
	receiveMessage(connfd, recvBuff);

	cJSON_ArrayForEach(user, userList){
		char * toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "email"));
		removeQuotes(toString);
		if (strcmp(recvBuff, toString) == 0){
			strcat(sendBuff, "\n");
			printUser(user, sendBuff);
			sendMessage(connfd, serv_addr, serv_len, sendBuff);
			return;
		}
	}
	sendMessage(connfd, serv_addr, serv_len, "\nPerfil não encontrado\n\n");
	
}

void printFiltered(int connfd, int filter){
	FILE *filePointer;
	char sendBuff[1025];
	char json[5000];
	char recvBuff[1024];
	memset(sendBuff, 0, sizeof(sendBuff));
	if (access("users.txt", F_OK) != 0) {
		sendMessage(connfd, serv_addr, serv_len, "\nNenhum perfil cadastrado\n");
		return;
	}
    filePointer = fopen("users.txt", "r");
	fscanf(filePointer, "%[^\0]", json);
	cJSON *userList = cJSON_Parse(json);
	fclose(filePointer);
	cJSON *user = NULL;

	//Raise flag if any profile found
	int flag = 0;

	//print all profiles of given graduate
	if (filter == 1){
		sendMessage(connfd, serv_addr, serv_len, "\nProcurar por todos os perfis com determinada formação\n\nInsira a formação acadêmica que deseja buscar:\n");
		receiveMessage(connfd, recvBuff);
		strcat(sendBuff, "\nPerfis com formação em ");
		strcat(sendBuff, recvBuff);
		strcat(sendBuff, ":\n\n");
		
		cJSON_ArrayForEach(user, userList){
			char * toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "graduate"));
			removeQuotes(toString);
			if (strcmp(recvBuff, toString) == 0){
				flag = 1;
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
		sendMessage(connfd, serv_addr, serv_len, "\nProcurar por todos os perfis com determinada habilidade\n\nInsira a habilidade que deseja buscar:\n");
		receiveMessage(connfd, recvBuff);
		strcat(sendBuff, "\nPerfis com habilidade em ");
		strcat(sendBuff, recvBuff);
		strcat(sendBuff, ":\n\n");
		cJSON * abilities;
		cJSON * ability;
		cJSON_ArrayForEach(user, userList){
			abilities = cJSON_GetObjectItemCaseSensitive(user, "abilities");
			cJSON_ArrayForEach(ability, abilities){
				char * toString = cJSON_Print(ability);
				removeQuotes(toString);
				if(strstr(toString, recvBuff) != NULL){
					flag = 1;
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
		sendMessage(connfd, serv_addr, serv_len, "\nProcurar por todos os perfis com curso completo no ano dado\n\nInsira o ano de formação acadêmica que deseja buscar:\n");
		receiveMessage(connfd, recvBuff);
		strcat(sendBuff, "\nPerfis com formação no ano ");
		strcat(sendBuff, recvBuff);
		strcat(sendBuff, ":\n\n");
		
		cJSON_ArrayForEach(user, userList){
			char * toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "year"));
			removeQuotes(toString);
			if (strcmp(recvBuff, toString) == 0){
				flag = 1;
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
	if (flag == 0){
		sendMessage(connfd, serv_addr, serv_len, "\nNenhum perfil encontrado!\n\n");
		return;
	}
	sendMessage(connfd, serv_addr, serv_len, sendBuff);
}

void addAbility(int connfd){
	FILE *filePointer;
	char sendBuff[1025];
	char recvBuff[1024];
	char json[5000];
	memset(sendBuff, 0, sizeof(sendBuff));

	if (access("users.txt", F_OK) != 0) {
		sendMessage(connfd, serv_addr, serv_len, "\nNenhum perfil cadastrado\n");
		return;
	}

	filePointer = fopen("users.txt", "r");
	fscanf(filePointer, "%[^\0]", json);
	cJSON *userList = cJSON_Parse(json);
	fclose(filePointer);

	cJSON *user = NULL;

	sendMessage(connfd, serv_addr, serv_len, "\nAdicionar habilidade a um perfil existente\n\nInsira o email do perfil:\n");
	receiveMessage(connfd, recvBuff);

	cJSON_ArrayForEach(user, userList){
		char * toString = cJSON_Print(cJSON_GetObjectItemCaseSensitive(user, "email"));
		removeQuotes(toString);
		if (strcmp(recvBuff, toString) == 0){
			sendMessage(connfd, serv_addr, serv_len, "\nInsira a habilidade que deseja inserir:\n");
			receiveMessage(connfd, recvBuff);
			cJSON * abilities = cJSON_GetObjectItemCaseSensitive(user, "abilities");
			cJSON * ability = NULL;
			cJSON_ArrayForEach(ability, abilities){
				char * toString = cJSON_Print(ability);
				removeQuotes(toString);
				if (strcmp(toString, recvBuff) == 0){
					sendMessage(connfd, serv_addr, serv_len, "\nHabilidade já inserida neste perfil!\n\n");
					return;
				}				
			}

			cJSON_AddItemToArray(abilities, cJSON_CreateString(recvBuff));
			filePointer = fopen("users.txt", "w");
			fprintf(filePointer, "%s", cJSON_Print(userList));
			fclose(filePointer);

			sendMessage(connfd, serv_addr, serv_len, "\nHabilidade inserida no perfil!\n\n");

			return;
		}
	}
	sendMessage(connfd, serv_addr, serv_len, "\nPerfil não encontrado\n\n");
	
}

void sendMenuMessage(int connfd){
	char sendBuff[1024];
	strcpy(sendBuff, "\nInsira o comando desejado digitando o número:\n");
	strcat(sendBuff,"1.Inserir perfil\n");
	strcat(sendBuff,"2.Adicionar habilidade a um perfil existente\n");
	strcat(sendBuff,"3.Procurar por todos os perfis com determinada formação\n");
	strcat(sendBuff,"4.Procurar por todos os perfis com determinada habilidade\n");
	strcat(sendBuff,"5.Procurar por todos os perfis com curso completo no ano dado\n");
	strcat(sendBuff,"6.Listar todos os perfis\n");
	strcat(sendBuff,"7.Procurar por perfil dado o email\n");
	strcat(sendBuff,"8.Remover um perfil\n\n");
	sendMessage(connfd, serv_addr, serv_len, sendBuff);
}

void receiveCommands(int connfd, struct sockaddr *serv_addr, socklen_t serv_len){
	char recvBuff[1024];
	while (1){
		receiveMessage(connfd, recvBuff);
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
			sendMenuMessage(connfd);
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

	while (1){
		receiveCommands(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	}
	return 0;
}

