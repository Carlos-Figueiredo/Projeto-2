
all:
	rm -rf Client
	rm -rf Server
	rm -rf users.txt
	gcc socketServer.c cJSON.c -o Server
	gcc socketClient.c -o Client

