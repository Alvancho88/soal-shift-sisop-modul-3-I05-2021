# soal-shift-sisop-modul-3-I05-2021
Repository for Shift 3 Lab Work

# SOAL 1
Keverk is a famous ambitious person in his colleagues. Before he became the head of
department in HTMC, he has done a project which tells him to make a book server
database. This project is required, so that it could be used by the app owner and is
expected from the users.
In this project, Keverk is asked to:

# A.)
When a client is connected to the server, there are two main options. They are
register and login. If the user has chosen the register option, the client will prompt
an id and password to be sent to the server. Users can also login to the server.
Login is successful if the id and the password sent to the server matched with
one of the accounts listed on the server application. This system can accepts
multiple connections at once. A connection is counted when a client application is
connected to the server. If there are 2 connections or more then we must wait for
the first client to exit the application to be able to login and use the application.
Keverk wanted the file that stores the id and password named account.txt with
the following format:

# account.txt
```
id:password
id2:password2
```

# B.)
The system has a database on a file named files.tsv. The content of the file is
the file path on the server, publisher, and the year of publication. For every
insertion and removal of a file in the folder FILES on the server, the file files.tsv
will be affected. Folder FILES are made, automatically when the server is started

# C.)
Not only that, Keverk is asked to make a feature such that clients are able to
insert new files to the server. FILES directory have a structure as follows :

Direktori FILES

```
File1.extension
File2.extension
```

First, the client send input to the server with the structure as follows:
Client command example :

```
add
```

Client Console Output:

```
Publisher:
Publication year:
Filepath:
```

Then, we can fill the book’s data (please note that Filepath is the path to the file
that will be sent to the server). Then the client will send the file to the server via
socket. When a file is received at the server, a new row containing new data of
the book is then added to the files.tsv.

# D.)
Clients can also download files that exist in the FILES folder in the server, then
the system must send the following file to the client. Server must check from
files.tsv to check if the file requested is valid. If it’s not valid, the system sends an
error message back to the client. If valid, the following file is then sent to the
client in the client folder.

Client command example

```
download TEMPfile.pdf
```

# E.)
After that, the client can also delete file that is stored on the server. But keverk is
afraid that the file that is going to be deleted is an important file, so instead of
deleting the file, the program just changes its name to ‘old-FileName.extension’.
When the filename is changed, the row of the file in files.tsv will be deleted.

Client command example:

```
delete TEMPfile.pdf
```

# F.)
Clients can see all the contents of files.tsv by calling the see command. Output
of the command will follow this format.

Client command example :

```
see
```

Client Console Output:

```
Name:
Publisher:
Publication year:
File extension :
Filepath :
Name:
Publisher:
Publication Year:
File extension:
Filepath :
```

# G.)
The client application can also do search by using the find command and a
string. It will output the same format as number 6.

Client command example:

```
find TEMP
```

# H.)
Keverk is cautious on insertion and removal of files on the server, so he made a
log file for the server named running.log. The content of the log file is following
this format

running.log
```
Tambah : File1.extension (id:pass)
Hapus : File2.extension (id:pass)
```

But keverk has not yet understood the client and server system. Help Keverk to finish
this project!

Note:
- It is not allowed to use system() dan execv(). Please do the task fully using
thread and socket programming.
- To download and upload, please use any text file with extension. The content of
the file can be anything. (Please don’t use an empty file) and the accepted
requirement is trial with the minimum of 5 data.

Directory Structure:
```
├── Client
│ ├── client.c
│ ├── File2.extension
│ └── File1.extension
└── Server
  ├── account.txt
  ├── files.tsv
  ├── server.c
  ├── running.log
  └── FILES
    ├── File2.extension
    └── File1.extension
```

# SOLUTION

Here are the libraries used to solve this problem:

#include <stdio.h> = for standard input-output

#include <sys/socket.h> = to run socket

#include <stdlib.h> = for general functions

#include <netinet/in.h> = for internet domain addresses

#include <string.h> = to perform string manipulation, for example strcmp ()

#include <errno.h> = to give additional errors to the system

#include <unistd.h> = to fork a system call ()

#include <arpa/inet.h> = for internet operations

#include <sys/types.h> = for data types

#include <sys/stat.h> = for data returned by the stat() function

```
#define PORT 8080
#define SIZE 1024
```
Define server port and SIZE for later use


Server
Server Function Initialization
```
int createServerSocket(struct sockaddr_in *address, int *addrlen);
int command(char buffer[], int *flag);
char *pathing(char path1[], char path2[], char result[]);
int lr(char buffer[], int *flag);
void addtsv(FILE *files, char nama[], char publisher[], char tahun[], char ekstensi[], char path[]);
int deletefile(FILE *files, char name[], char source[], char result[]);
void see(FILE *files, char check[], char display[]);
void find(FILE *files, char check[], char display[], char buffer[]);
int locate(char fullpath[], char sy);
void logging(FILE* log,int tipe, char nama[]);
```


Server Socket Creator Function
```
int createServerSocket(struct sockaddr_in *address, int *addrlen)
{
    int fd, opt = 1;
 
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
 
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);
    *addrlen = sizeof(*address);
 
    if (bind(fd, (struct sockaddr *)address, *addrlen) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    return fd;
}
```

```
    int server_fd = createServerSocket(&address, &addrlen);
```


Server Path Helper Function
```
char *pathing(char path1[], char path2[], char result[]){
    strcpy(result, path1);
    strcat(result, path2);
    return result;
}
```


Server Global Iniitialization
```
    struct sockaddr_in address;
    int new_socket, addrlen, flag = 1, info, auth = 0;
    char delim[] = ":";
    char buffer[1024];
    char check[1024];
    char source[100] = "/home/alvancho/Documents/IO5/Shift_3/Question_1/Server";
    char result[100];
    char display[10000];
    
    char *c_registers = "register";
	  char *c_login = "login";
	  char *c_add = "add";
	  char *c_download = "download";
	  char *c_delete = "delete";
	  char *c_see = "see";
	  char *c_find = "find";


    FILE* akun;
    FILE* files;
    FILE* log;
    akun = fopen(pathing(source, "/account.txt", result), "a+");
    files = fopen(pathing(source, "/files.tsv", result), "a+");
    log = fopen(pathing(source, "/log.tsv", result), "a+");
    int status = mkdir(pathing(source, "/FILES", result),0777); //folder FILES
```


Server Command Function
```
int command(char buffer[], int *flag)
{
    if (strcmp(buffer, "add") == 0) {
        sprintf(buffer, "Publisher:");
        return 1;
    } else if(strcmp(buffer, "delete") == 0){
        return 2;
    } else if(strcmp(buffer, "see") == 0){
        return 3;
    } else if(strcmp(buffer, "find") == 0){
        return 4;
    } else if(strcmp(buffer, "download") == 0){
        return 5;
    } else if(strcmp(buffer, "exit") == 0){
        (*flag) = 0;
        sprintf(buffer, "Exiting..");
        return 0;
    }
    return 0;
}
```


Server Status Function
```
int lr(char buffer[], int *flag)
{
    if (strcmp(buffer, "login") == 0) {
        sprintf(buffer, "Expecting id...");
        return 1;
    }
    else if (strcmp(buffer, "register") == 0) {
        sprintf(buffer, "Expecting id...");
        return 2;
    } else if(strcmp(buffer, "exit") == 0){
        (*flag) = 0;
        sprintf(buffer, "Exiting..");
        return 3;
    }
    return 0;
}
```


Server Locate Function
```
int locate(char fullpath[], char sy){
    int len = strlen(fullpath);
    while(--len){
        if(fullpath[len] == sy) break;
    }
    return len;
}
```


Client
Client Function Initialization
```
int createClientSocket(struct sockaddr_in *address, struct sockaddr_in *serv_addr);
char *pathing(char path1[], char path2[], char result[]);
```


Client socket creator function
```
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
```

```
    int client_fd = createClientSocket(&address, &serv_addr);
```


Client Path helper function
```
char *pathing(char path1[], char path2[], char result[]){
    strcpy(result, path1);
    strcat(result, path2);
    return result;
}
```


Client Global Initialization
```
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
```

Client Input (User Input)
```
    	printf("Please input the command\n");
        scanf("%s", command);
```

# A.)

Source Code

*Server
```
        while(flag == 1 && auth == 0){
        	
            fclose(akun);
            akun = fopen(pathing(source, "/account.txt", result), "a+");
            
            memset(buffer, 0, sizeof(buffer));
            read(new_socket, buffer, 1024);
            
            info = lr(buffer, &flag);
            send(new_socket, buffer, strlen(buffer), 0);
            
            if(info == 1) { //login
                char login[1024];
                char pass[1024];
                
                memset(buffer, 0, sizeof(buffer)); //id
                read(new_socket, buffer, 1024);
                
                strcpy(login, buffer);
                send(new_socket, "Expecting pass...", strlen("Expecting pass..."), 0);
                
                memset(buffer, 0, sizeof(buffer)); // pass
                read(new_socket, buffer, 1024);
                strcpy(pass, buffer);
                
                while(fgets(check, sizeof(check), akun))
                {
                    char *ptr = strtok(check,delim);
                    if (strcmp(ptr, login) == 0) 
                    {
                        ptr = strtok(NULL,delim);
                        ptr[strcspn(ptr,"\n")]='\0';
                        // printf("%d\n",strcmp(ptr, pass));
                        if (strcmp(ptr, pass) == 0) 
                        {
                            sprintf(user,"%s:%s", login, pass);//warning
                            auth=1;
                            break;
                        }
                    }
                }
                if(auth == 1)
                	send(new_socket, "Login berhasil", strlen("Login berhasil"), 0);
                else{
                	send(new_socket, "Login gagal", strlen("Login gagal"), 0);
				}
            } else if(info == 2){ //register
                int pesan = 0;
                char login[1024];
                char pass[1024];
                
                memset(buffer, 0, sizeof(buffer)); //id
                read(new_socket, buffer, 1024);
                
                strcpy(login, buffer);
                send(new_socket, "Expecting pass...", strlen("Expecting pass..."), 0);
                
                memset(buffer, 0, sizeof(buffer)); // pass
                read(new_socket, buffer, 1024);
                strcpy(pass, buffer);
                
                while(fgets(check, sizeof(check), akun))
                {
                    char *ptr = strtok(check,delim);
                    puts(ptr);
                    puts(login);
                    if (strcmp(ptr, login) == 0) 
                    {
                        pesan = 1;
                        break;
                    }
                }
                if(pesan == 0){
                    fputs(login, akun);
                    fputs(":", akun);
                    fputs(pass, akun);
                    fputs("\n", akun);
                }

                if(pesan == 0)
                send(new_socket, "Berhasil mendaftar.", strlen("Berhasil mendaftar."), 0);
                else send(new_socket, "Gagal mendaftar.", strlen("Gagal mendaftar."), 0);
            }
        }
```

*Helper Function (Server)
```
int lr(char buffer[], int *flag)
{
    if (strcmp(buffer, "login") == 0) {
        sprintf(buffer, "Expecting id...");
        return 1;
    }
    else if (strcmp(buffer, "register") == 0) {
        sprintf(buffer, "Expecting id...");
        return 2;
    } else if(strcmp(buffer, "exit") == 0){
        (*flag) = 0;
        sprintf(buffer, "Exiting..");
        return 3;
    }
    return 0;
}
```

*Client
```
    	printf("Please input the command\n");
        scanf("%s", command);
```

```
        send(client_fd, command, strlen(command), 0);
        memset(command, 0, sizeof(command));
        read(client_fd, command, 1024);
        printf("%s\n", command);
```

Explanation
Auth is used to classify client accounts. Accounts that are logged in at this time will be stored in the user for easy logging.
If successfully logged in, it will escape the while loop

Screenshot

# B.)

Source Code

*Server
```
        fclose(files);
        fclose(log);
        
        files = fopen(pathing(source, "/files.tsv", result), "a+");
        log = fopen(pathing(source, "/log.tsv", result), "a+");
        
        memset(display, 0, strlen(display));
        memset(buffer, 0, sizeof(buffer));
        
        read(new_socket, buffer, 1024);
        info = command(buffer, &flag);
```

*Helper Function (Server)
```
void addtsv(FILE *files, char nama[], char publisher[], char tahun[], char ekstensi[], char path[]){
    fputs(nama, files);
    fputc("\t", files);
    fputs(publisher, files);
    fputc("\t", files);
    fputs(tahun, files);
    fputc("\t", files);
    fputs(ekstensi, files);
    fputc("\t", files);
    fputs(path, files);
    fputc("\t", files);
}
```

Explanation
Organized by variable files. then create a procedure for adding to files.tsv using addtsv ()

Screenshot

# C.)

Source Code

*Server
```
        if(info == 1){
        	
            char comp[1024];
            char nama[1024];
            char publish[1024];
            char tahun[1024];
            char eks[1024];
            char pathnya[1024];
            
            send(new_socket, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            
            read(new_socket, buffer, 1024);
            strcpy(publish,buffer);
            send(new_socket, "Tahun publikasi:", strlen("Tahun publikasi:"), 0);
            memset(buffer, 0, sizeof(buffer));
            
            read(new_socket, buffer, 1024);
            strcpy(tahun,buffer);
            send(new_socket, "Filepath:", strlen("Filepath:"), 0);
            memset(buffer, 0, sizeof(buffer));
            
            read(new_socket, buffer, 1024);
            send(new_socket, "Transferring..", strlen("Transferring.."), 0);
            strcpy(comp,buffer);
            
            char *ptr;
            
            ptr = comp + locate(comp,'/') + 1;
            strcpy(nama,ptr);
            
            logging(log,1,nama);
            strcpy(comp,nama);
            
            ptr = comp + locate(comp,'.') + 1;
            strcpy(eks,ptr);
            
            char *p = strtok(comp,".");
            strcpy(nama,p);
            
            sprintf(display,"%s/FILES/%s.%s",source,nama,eks);
            
            char data[1024];
            FILE *rcv;
            rcv = fopen(display,"wb");
            int n;
            while (1) {
                n = recv(new_socket, data, 1024, 0);
                if (strcmp(data,"END")==0){
                break;
                }
                fwrite(data,1,sizeof(data),rcv);
                // puts("...");
                bzero(data, 1024);
            }
            fclose(rcv);
            addtsv(files,nama,publish,tahun,eks,display);
            continue;
```

*Client
```
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
```

Explanation
The add command will produce info = 1. the procedure is held by the code section below.

Screenshot

# D.)

Source Code

*Server
```
else if(info == 5){
            int muncul =0;
            sprintf(buffer, "Downloading..");
            send(new_socket, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            read(new_socket, buffer, 1024);
            sprintf(display,"%s/FILES/%s",source,buffer);
            char *p;
            char na[1024],nana[1024], eks[1024];
            strcpy(na,buffer);
            p = buffer + locate(buffer,'.');
            strcpy(eks,p);
            char *pna = strtok(na,".");
            strcpy(nana,pna);
            while(fgets(check, 1024 , files)) //check validity
            {
                int i = 0;
                char comp[1024];
                
                strcpy(comp,check);
                
                char iter[5][1024];
                char *ptr = strtok(comp,"\t");

                while( ptr != NULL){
                    strcpy(iter[i++],ptr);
                    ptr = strtok(NULL,"\t");
                }
                
                if (strcmp(iter[0], nana) == 0 && strcmp(iter[3], eks) == 0) 
                {
                    muncul++;
                }
            }
            fclose(files);
            files = fopen(pathing(source, "/files.tsv", result), "a+");
            if(muncul == 0) send(new_socket, "END", sizeof("END"), 0);
            if(muncul > 0){
                FILE *fp;
                fp = fopen(display,"rb");
                if (fp == NULL) {
                    perror("[-]Error in reading file.");
                    exit(1);
                }
                char data[SIZE] = {0};
                int n;
                do{
                    n = fread(data, 1,1024, fp);
                    send(new_socket, data, sizeof(data), 0);
                    // puts(",,,");
                } while(n == sizeof(data));
                memset(data, 0, sizeof(data));
                fclose(fp);
                send(new_socket, "END", sizeof("END"), 0);
            } else {
                sprintf(buffer, "File tidak valid, mohon dicek kembali");
            }
            continue;
        }
```

*Client
```
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
```

Explanation
Here is the download procedure. use fopen mode which rb and wb

Screenshot

# E.)

Source Code

*Server
```
 else if(info == 2){
            sprintf(buffer, "Processing..");
            send(new_socket, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            read(new_socket, buffer, 1024);
            if(deletefile(files,buffer,source,result) == 1)
            logging(log,2,buffer);
            fclose(files);
            remove(pathing(source, "/files.tsv", result));

            char result2[100];
            strcpy(result2,pathing(source, "/files.tsv", result));
            rename(pathing(source, "/temp.tsv", result), result2);

            files = fopen(pathing(source, "/files.tsv", result), "a+");
            sprintf(buffer, "Processed..");
        }
```

*Helper Function (Server)
```
int deletefile(FILE *files, char name[], char source[], char result[]){
    int flag=0;
    char check[1024];
    FILE *temp;
    temp = fopen(pathing(source, "/temp.tsv", result), "a+");
    char *p;
    char na[1024],nana[1024], eks[1024];
    strcpy(na,name);
    p = name + locate(name,'.') +1;
    strcpy(eks,p);
    char *pna = strtok(na,".");
    strcpy(nana,pna);
    // puts(nana);
    //     puts(eks);
    while(fgets(check, sizeof(check), files) != NULL)
    {
        int i = 0;
        char comp[1024];
        
        strcpy(comp,check);
        
        char iter[5][1024];
        char *ptr = strtok(comp,"\t");

        while( ptr != NULL){
            strcpy(iter[i++],ptr);
            ptr = strtok(NULL,"\t");
        }
        
        if (strcmp(iter[0], nana) == 0 && strcmp(iter[3], eks) == 0) 
        {
            flag=1;
            continue;
        }
        fputs(check, temp);
    }
    fclose(temp);

    char result3[100];
    strcpy(result3,pathing(pathing(pathing(source, "/FILES/", result), "old-", result),name,result));
    rename( pathing(pathing(source, "/FILES/", result), name, result), result3);

    return flag;
}
```

Explanation
It is handled by the deleteFile () function and on main it is also available.

Screenshot

# F.)

Source Code

*Server
```
else if(info == 3){
            see(files, check, display);
            send(new_socket, display, strlen(display), 0);
            continue;
        }
```

*Helper Function (Server)
```
void see(FILE *files, char check[], char display[]){
    int muncul = 0;
    while(fgets(check, 1024 , files) != NULL)
    {
        int i = 0;
        char comp[1024];
        strcpy(comp,check);
        char iter[5][1024];
        char *ptr = strtok(comp,"\t");

        while( ptr != NULL){
            strcpy(iter[i++],ptr);
            ptr = strtok(NULL,"\t");
        }
        muncul++;
        sprintf(display + strlen(display),"\nNama:%s\n",iter[0]);
        sprintf(display + strlen(display),"Publisher:%s\n",iter[1]);
        sprintf(display + strlen(display),"Tahun publishing:%s\n",iter[2]);
        sprintf(display + strlen(display),"Ekstensi File:%s\n",iter[3]);
        sprintf(display + strlen(display),"Filepath:%s",iter[4]);
    }
    if(muncul == 0) sprintf(display + strlen(display),"Nama file tidak ditemukan");
}
```

Explanation
Handled by the see () function and played there too.

Screenshot

# G.)

Source Code

*Server
```
else if(info == 4){
            sprintf(buffer, "Finding..");
            send(new_socket, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            read(new_socket, buffer, 1024);
            find(files, check, display, buffer);
            send(new_socket, display, strlen(display), 0);
            continue;
        }
```

*Helper Function (Server)
```
void find(FILE *files, char check[], char display[], char buffer[]){
    int muncul = 0;
    while(fgets(check, 1024 , files)!= NULL)
    {
        if(feof(files)){
            sprintf(display + strlen(display),"files.tsv kosong");
            return;
        }
        int i = 0;
        char comp[1024];
        strcpy(comp,check);
        char iter[5][1024];
        char *ptr = strtok(comp,"\t");
        if(strcmp(buffer, ptr) == 0){
            muncul++;
        }
    }
    fclose(files);
    files = fopen(pathing("/home/alvancho/Documents/IO5/Shift_3/Soal_1/Server", "/files.tsv", check), "a+");
    if(muncul > 0)
    while(fgets(check, 1024 , files))
    {
        int i = 0;
        char comp[1024];
        strcpy(comp,check);
        char iter[5][1024];
        char *ptr = strtok(comp,"\t");
        if(strcmp(buffer, ptr) != 0) continue;

        while( ptr != NULL){
            strcpy(iter[i++],ptr);
            ptr = strtok(NULL,"\t");
        }
        
        sprintf(display + strlen(display),"\nNama:%s\n",iter[0]);
        sprintf(display + strlen(display),"Publisher:%s\n",iter[1]);
        sprintf(display + strlen(display),"Tahun publishing:%s\n",iter[2]);
        sprintf(display + strlen(display),"Ekstensi File:%s\n",iter[3]);
        sprintf(display + strlen(display),"Filepath:%s",iter[4]);
    }
    else sprintf(display + strlen(display),"Nama file tidak ditemukan");
}
```

Explanation
Handled by the find () function and on main are also available. first, you must first check whether it is in files.tsv.

Screenshot

# H.)

Source Code

*Server
```
void logging(FILE* log,int tipe, char nama[]){
    char result[1024];
    if(tipe == 1){
        sprintf(result,"Tambah : %s (%s)\n",nama,user);//warning
        fputs(result,log);
    } else if(tipe == 2){
        sprintf(result,"Hapus : %s (%s)\n",nama,user);//warning
        fputs(result,log);
    }
}
```

*Initialization
```
    FILE* log;
    log = fopen(pathing(source, "/log.tsv", result), "a+");
```

Explanation
running.log is created at the beginning of the code. And we insert the logging () function when the add and delete commands are called.

Screenshot

Problems


