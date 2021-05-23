#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define SIZE 1024
 
int createClientSocket(struct sockaddr_in *address, struct sockaddr_in *serv_addr);
char *pathing(char path1[], char path2[], char result[]);
 
int main(int argc, char const *argv[])
{
    struct sockaddr_in address, serv_addr;
    char command[1024];

    char source[100] = "/home/alvancho/Documents/IO5/Shift_3/Question_1/Client";
    char result[100];
	char *c_register = "register";
	char *c_login = "login";
	char *c_add = "add";
	char *c_download = "download";
	char *c_deletes = "delete";
	char *c_see = "see";
	char *c_find = "find";
	char *c_exit = "exit";
 
    int client_fd = createClientSocket(&address, &serv_addr);
 
    while (1) {
    	printf("Please input the command\n");
        scanf("%s", command);
        
        if(strcmp(command,c_add)==0){
            send(client_fd, command, strlen(command), 0); //add
            memset(command, 0, sizeof(command));
            read(client_fd, command, 1024);
            printf("%s\n", command);
            scanf("%s", command);
            send(client_fd, command, strlen(command), 0); //publisher
            
            memset(command, 0, sizeof(command));
            read(client_fd, command, 1024);
            printf("%s\n", command);
            scanf("%s", command);
            send(client_fd, command, strlen(command), 0); //tahun
            
            memset(command, 0, sizeof(command));
            read(client_fd, command, 1024);
            printf("%s\n", command);
            scanf("%s", command);
            send(client_fd, command, strlen(command), 0); //filepath
            
            strcpy(result,command);
            memset(command, 0, sizeof(command));
            read(client_fd, command, 1024);
            printf("%s\n", command);
            memset(command, 0, sizeof(command));
            
            // sleep(5);
            FILE *fp;
            fp = fopen(result,"rb");
            if (fp == NULL) {
                perror("[-]Error in reading file.");
                exit(1);
            }
            char data[SIZE] = {0};
            int n;
            do{
                n = fread(data, 1,1024, fp);
                send(client_fd, data, sizeof(data), 0);
                // puts(",,,");
            } while(n == sizeof(data));
            memset(command, 0, sizeof(command));
            fclose(fp);
            send(client_fd, "END", sizeof("END"), 0);
            continue;
        }
        else if(strcmp(command,c_download)==0){
            send(client_fd, command, strlen(command), 0); //download
            memset(command, 0, sizeof(command));
            read(client_fd, command, 1024);
            printf("%s\n", command);
            scanf("%s", command);
            
            send(client_fd, command, strlen(command), 0); //file
            char data[1024];
            FILE *rcv;
            sprintf(result,"%s/%s",source,command);//warning
            rcv = fopen(result,"wb");
            int n;
            
            while (1) {
                n = recv(client_fd, data, 1024, 0);
                if (strcmp(data,"END")==0){
                break;
                }
                fwrite(data,1,sizeof(data),rcv);
                // puts("...");
                bzero(data, 1024);
            }
            fclose(rcv);
            continue;
        } else if(strcmp(command,c_exit)==0){
            send(client_fd, command, strlen(command), 0);
            break;
        }
        send(client_fd, command, strlen(command), 0);
        memset(command, 0, sizeof(command));
        read(client_fd, command, 1024);
        printf("%s\n", command);
    }
    return 0;
}


char *pathing(char path1[], char path2[], char result[]){
    strcpy(result, path1);
    strcat(result, path2);
    return result;
}
 
int createClientSocket(struct sockaddr_in *address, struct sockaddr_in *serv_addr)
{
    int fd;
 
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    memset(serv_addr, '0', sizeof(*serv_addr));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(PORT);
 
    if (inet_pton(AF_INET, "127.0.0.1", &(serv_addr->sin_addr)) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(fd, (struct sockaddr *)serv_addr, sizeof(*serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    return fd;
}
