#include<stdio.h>
#include<stdlib.h> 
#include<string.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>
#include<errno.h>
#include<ctype.h>

//pthread_mutex_t signal;

char *checkName(char *dir){
 char *name = strrchr(dir, '/');
 if(name == dir) return "";
 return name + 1;
}

char *lowercase(char *str){
 unsigned char *temp = (unsigned char *)str;
 while(*temp){
  *temp = tolower(*temp);
  temp++;
 }
 return str;
}

char *checkExt(char *dir){
 char *unk = {"Unknown"};
 char *hid = {"Hidden"};
 char *tmp = strrchr(dir, '/');
 if(tmp[1] == '.') return hid;
 
 int i = 0;
 while(i < strlen(tmp) && tmp[i] != '.') i++;
 if(i == strlen(tmp)) return unk;
 
 char ext[400];
 int j = i;
 while(i < strlen(tmp)) ext[i-j] = tmp[i], i++;
 return lowercase(ext + 1);
}

void* categorize(void *arg){
 char *src = (char *)arg;
 char srcP[150];
 memcpy(srcP, (char*)arg, 400);
 char *srcExt = checkExt(src);
 char ext[400];
 strcpy(ext, srcExt);
 
 DIR *dir = opendir(srcExt);
 if(dir) closedir(dir);
 else if(ENOENT == errno) mkdir(srcExt, 0755);
 
 char *srcName = checkName(srcP);
 char *curr = getenv("PWD");
 
 char destP[512];
 sprintf(destP, "%s/%s/%s", curr, ext, srcName);
 //printf("dest: %s (%s) %s\n", destP, ext, srcP);
 
 //pthread_mutex_lock(&signal);
 //if(rename(srcP, destP) != 0) fprintf(stderr,"error: %s\n\n",strerror(errno));
 //pthread_mutex_unlock(&signal);
 
 rename(srcP, destP);
}

void categorizeFolder(char *folderPath, int threadSize){
 DIR *fd = opendir(folderPath);
 struct dirent *dp;
 pthread_t tid[threadSize];
 int count = 0;
 char fileName[400][400];
 
 while((dp = readdir(fd)) != NULL){
  if(dp->d_type == DT_REG){
   sprintf(fileName[count], "%s/%s", folderPath, dp->d_name);
   pthread_create(&tid[count], NULL, categorize, (void *)fileName[count]);
   count++;
  }
  else if((dp->d_type == DT_DIR) && strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            char folderPath2[400];
            sprintf(folderPath2, "%s/%s", folderPath, dp->d_name);
            DIR *fd2 = opendir(folderPath2);
            struct dirent *dp2;
   int threadSize2 = 0;
   while((dp2 = readdir(fd2)) != NULL){
    if(dp2->d_type == DT_REG){
     threadSize2++;
    }
   }
   categorizeFolder(folderPath2, threadSize2);
   closedir(fd2);
        }
 }
 
 for(int i=0; i<threadSize; i++) pthread_join(tid[i], NULL);
    closedir(fd);
}

int main(int argc, char *argv[]){
 if(argc == 0) exit(EXIT_FAILURE);
 //soal a
 if(argc > 2 && strcmp(argv[1], "-f") == 0){
  pthread_t tid[argc-2];
  int count = 0;
  for(int i=2; i<argc; i++){
   if(access(argv[i], F_OK) == 0){
    pthread_create(&tid[count], NULL, categorize, (void *)argv[i]);
    count++;
    printf("File %d : Berhasil Dikategorikan\n", i-1);
   }
   else printf("File %d : Sad, gagal :(\n", i-1);
  }
  for(int i=0; i<count; i++) pthread_join(tid[i], NULL);
  return 0;
 }
 //soal b
 else if(argc == 3 && strcmp(argv[1], "-d") == 0){
  DIR *fd = opendir(argv[2]);
  if(fd){
   struct dirent *dp;
   int threadSize = 0;
   while((dp = readdir(fd)) != NULL){
    if(dp->d_type == DT_REG){
     threadSize++;
    }
   }
   categorizeFolder(argv[2], threadSize);
   closedir(fd);
   printf("Direktori sukses disimpan!\n");
  }
  else if(ENOENT == errno) printf("Yah, gagal disimpan :(\n");
 }
 //soal c
 else if(argc == 2 && strcmp(argv[1], "*") == 0){
  char *curr = getenv("PWD");
  DIR *dir = opendir(curr);
  struct dirent *dp;
  int threadSize = 0;
  while((dp = readdir(dir)) != NULL){
   if(dp->d_type == DT_REG){
    threadSize++;
   }
  }
  categorizeFolder(curr, threadSize);
  closedir(dir);
 }
 else{
  printf("Format input salah\n");
  return 0;
 }
}
