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
void send_file(FILE *fp, int newSocket){
    int n;
    char data[SIZE] = {0};

    while(fgets(data, SIZE, fp) != NULL) {
        if (send(newSocket, data, sizeof(data), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        bzero(data, SIZE);
    }
}

int main(int argc, char *argv[]){
    //const int PORT = atoi(argv[1]);
    #define PORT atoi(argv[1])
    #define FORKS atoi(argv[2])
    int sockfd, ret;
    struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;
    FILE *fp;
    char *filename = "file1.txt";

	socklen_t addr_size;

	char buffer[5024];
	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("10.0.2.15");

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", PORT);

	if(listen(sockfd, 10) == 0){
		printf("[+]Listening....\n");
	}else{
		printf("[-]Error in binding.\n");
	}

    int sockCount = 0;
	while(1){
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
		sockCount = sockCount + 1;

		if((childpid = fork()) == 0){
			close(sockfd);

			while(1){
				recv(newSocket, buffer, 5024, 0);
                if(sockCount > FORKS){
                    char msClose[]= "[F] El servidor esta en su maxima capacidad, intentelo de nuevo...";
                    send(newSocket, msClose, strlen(msClose), 0);
                    close(newSocket);
                    printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    sockCount = sockCount - 1;
                    break;
                }
				if(strcmp(buffer, ":exit") == 0){
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    sockCount = sockCount - 1;
					break;
				}else if(strcmp(buffer, ":file1.txt") == 0){
                    fp = fopen(filename, "r");
                    if (fp == NULL) {
                        perror("[-]Error in reading file.");
                        exit(1);
                    }
                    char bufferF[5024];
                    fgets(bufferF,5000,fp);
                    printf("%s\n",bufferF);
                    fclose ( fp );
                    send(newSocket, bufferF, strlen(bufferF), 0);
                    printf("[+]File data sent successfully.\n");
                    bzero(buffer, sizeof(buffer));
                    bzero(bufferF, sizeof(bufferF));
				}else if(strcmp(buffer, ":play-up") == 0){
                    //char fullMs[5024];
                    char fullMs[] = "\n\n| | |x| | |\n| |x|x|x| |\n|x|x|x|x|x|\n| |x|x|x| |\n| |x|x|x| |\n";
                    printf("Client %s:%d ---> %s\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port),buffer);
                    send(newSocket, fullMs, strlen(fullMs), 0);
                    bzero(buffer, sizeof(buffer));
                    bzero(fullMs, sizeof(fullMs));
                }else if(strcmp(buffer, ":play-down") == 0){
                    //char fullMs[5024];
                    char fullMs[] = "\n\n| |x|x|x| |\n| |x|x|x| |\n|x|x|x|x|x|\n| |x|x|x| |\n| | |x| | |\n";
                    printf("Client %s:%d ---> %s\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port),buffer);
                    send(newSocket, fullMs, strlen(fullMs), 0);
                    bzero(buffer, sizeof(buffer));
                    bzero(fullMs, sizeof(fullMs));
                }else{
				    char fullMs[5024];
				    char clientMs[]="Recibimos tu mensaje: |";
                    strcpy(fullMs, clientMs);
                    strcat(fullMs, buffer);
                    strcat(fullMs, "| - pero no te entiendo...");
					printf("Client: %s\n", buffer);
					send(newSocket, fullMs, strlen(fullMs), 0);
					bzero(buffer, sizeof(buffer));
                    bzero(fullMs, sizeof(fullMs));
				}//else
			}//while
		}//if

	}//while

	close(newSocket);


	return 0;
}