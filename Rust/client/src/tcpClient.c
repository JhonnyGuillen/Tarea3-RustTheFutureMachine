#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SIZE 5024

//#define PORT 4444

int main(int argc, char *argv[]){
    //const int PORT = atoi(argv[1]);
    #define PORT atoi(argv[1])

	int clientSocket, ret, new_sock;
	struct sockaddr_in serverAddr, new_addr;
	socklen_t addr_size;
	char buffer[5024];

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("10.0.2.15");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");

	while(1){
		printf("Client: \t");
		scanf("%s", &buffer[0]);
		send(clientSocket, buffer, strlen(buffer), 0);

		if(strcmp(buffer, ":exit") == 0){
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}else if (strcmp(buffer, ":file1.txt") == 0){
            if(recv(clientSocket, buffer, 5024, 0) < 0){
                printf("[-]Error in receiving data.\n");
            }else{
                FILE *fp;
                char *filename = "file1.txt";
                //char buffer[SIZE];
                char bufferF[5024];
                char aux[]="";
                strcpy(bufferF, aux);
                strcat(bufferF,buffer);
                fp = fopen(filename, "w");
                fputs(bufferF, fp);
                fclose ( fp );
                printf("Archivo recibido: file1.txt\n");
                //bzero(buffer, SIZE);
            }//else
		}else if (strcmp(buffer, ":file1.txt") != 0){
            if(recv(clientSocket, buffer, 5024, 0) < 0){
                printf("[-]Error in receiving data.\n");
            }else{
                char aux2[]="F";
                //printf("buffer[1]: \t%c\n", buffer[1]);
                if(aux2[0] == buffer[1]){
                    printf("Server: \t%s\n", buffer);
                    close(clientSocket);
                    printf("[-]Disconnected from server.\n");
                    exit(1);
                } else{
                    printf("Server: \t%s\n", buffer);
                }
            }
		}

	}

	return 0;
}
