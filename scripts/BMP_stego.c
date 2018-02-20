/*
  BMP file carver and stego analyzer

  must have openssl installed

  compile as gcc BMP_stego.c -lcrypto
  run /a.out [bmp file]

*/



#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <openssl/md5.h>

void eval_BMP(FILE *input,char *dir);
void eval_unknown(FILE *input,char *dir,int first);
void compute_MD5(char *name);

int main(int argc,char *argv[]){
  FILE *input=fopen(argv[1],"r");
  char *dir=malloc(sizeof(argv[1])+7);
  strcpy(dir,argv[1]);
  strcat(dir,"_Output");
  mkdir(dir,0700);
  char first,second;
  fread(&first,1,1,input);
  while(fread(&second,1,1,input)!=0){
    if(first==0x42 && second==0x4d){
      eval_BMP(input,dir);
      fread(&second,1,1,input);
    }else{
      eval_unknown(input,dir,first);
      second=0x42;
    }
    first=second;
  }
 free(dir);
 fclose(input);
 
}

void eval_unknown(FILE *input,char *dir,int first){
  long offset=ftell(input)-2;
  char *file=malloc(50);
  sprintf(file,"%s/%ld.unknown",dir,offset);
  FILE *newfile=fopen(file,"w");
  fseek(input,offset+1,SEEK_SET);
  int second;
  int i=1;
  while(fread(&second,1,1,input)!=0 && !(first==0x42 && second==0x4d)){
    fputc(first,newfile);
    first=second;
    i++;
  }
  if(first==0x42 && second==0x4d){
    fseek(input,-1,SEEK_CUR);
    i-=2;
  }else{
    fputc(first,newfile);
  }
  fclose(newfile);

  printf("offset of %ld\nsize of %d\nmd5 is ",offset,i);
  compute_MD5(file);
  free(file);
  printf("path is %s\n\n",file);
}

void write_file(FILE *input,FILE *output,long offset, char *file,char *dir){
  int data,bmpsize,i=0;
  fread(&bmpsize,4,1,input);
  fseek(input,offset+2,SEEK_SET);
  int first;
  unsigned char *result;
  while(i<bmpsize-2){
    fread(&data,1,1,input);
    if((char)first==0x42 && (char)data==0x4d){
      eval_BMP(input,dir);
      fseek(input,offset+i+3,SEEK_SET);
    }
    fputc(data,output);
    i++;
    first=data;
  }
  fclose(output);
  printf("offset of %ld\nsize of %d\nmd5 is ",offset,bmpsize);
  compute_MD5(file);
}

void eval_BMP(FILE *input,char *dir){
  char *file=malloc(50);
  long offset=ftell(input)-2;
  sprintf(file,"%s/%ld.bmp",dir,offset);
  FILE *newfile=fopen(file,"w");
  fputc(0x42,newfile);
  fputc(0x4d,newfile);
  write_file(input,newfile,offset,file,dir);
  printf("path is %s\n\n",file);
  fclose(newfile);
  free(file);
}

void compute_MD5(char *name){
  FILE *file=fopen(name,"r");
  fseek(file,0,SEEK_END);
  long filesize=ftell(file);
  fseek(file,0,SEEK_SET);
  unsigned char *buf;
  buf=malloc(filesize);
  fread(buf,filesize,1,file);
  unsigned char *md5_result =malloc(MD5_DIGEST_LENGTH);
  MD5(buf, filesize, md5_result);
  for (int i=0; i < MD5_DIGEST_LENGTH; i++)
      {
        printf("%02x",  md5_result[i]);
      }
  printf("\n");
  fclose(file);
  free(buf);
  free(md5_result);
}
