#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

#define SIZE 1024

char inputFile[50];
char outputFile[50];

char inbuf[SIZE];
char outbuf[SIZE];

char intemp[SIZE];
char outtemp[SIZE];

void CopyFile(char* inputFile,char* outputFile){
	//파일 복사를 위한 함수 
	struct stat frstatbuf;
	FILE* fr =  fopen(inputFile,"r");

	if(fr==NULL){ 
		perror("file read");
		exit(1);
	}
	int frfd = fileno(fr);

	fstat(frfd,&frstatbuf);
	//현재 파일의 정보를 불러옴 
	FILE* fw =fopen(outputFile,"w+");

	if(fw ==NULL){
		perror("file write");
		exit(1);
	}
	int fwfd = fileno(fw);
	
	fchmod(fwfd,frstatbuf.st_mode);
	//새 파일의 mod를 기존의 파일과 같게 설정 
	char buf[1024];

	while(1){
		int n =fread(buf,sizeof(char),SIZE,fr);
		//file read save into buf
		if(n<SIZE){
			fwrite(buf,sizeof(char),n,fw);
			//file is finish
			return;
		}
		//파일 복사
		fwrite(buf,sizeof(char),n,fw);
	}
	fclose(fr);
	fclose(fw);
}


int folderCopy(DIR* inputdir,DIR* outputdir){
	while(1){

		struct dirent * rddir =readdir(inputdir);
		struct dirent * outddir = readdir(outputdir);

		if(rddir==NULL){
			//end of dir stream
			exit(1);
		}

		if(rddir->d_type==DT_DIR){
			//if it is dir
			if(strcmp(rddir->d_name,".")==0||strcmp(rddir->d_name,"..")==0){
			//if it is current or parent dir do nothing
			}
			else{
				
				sprintf(inbuf,"%s/%s",inputFile,rddir->d_name);
				sprintf(outbuf,"%s/%s",outputFile,rddir->d_name);
				struct stat buf;
				stat(inbuf,&buf);
				int mkdirFlag = mkdir(outbuf,buf.st_mode);
				if(mkdirFlag ==0){
					//if mkdir is success
					strcpy(intemp,inputFile);
					strcpy(outtemp,outputFile);
					strcpy(inputFile,inbuf);
					strcpy(outputFile,outbuf);
				}
				else{
					perror("mkdir");
					exit(1);
				}
				DIR* indir = opendir(inbuf);
				DIR* outdir = opendir(outbuf);
				//open dir for read and write
				folderCopy(indir,outdir);
				//copy dir from read to write
				strcpy(inputFile,intemp);
				strcpy(outputFile,outtemp);
			}
		}
		else if(rddir->d_type==DT_REG){
			//if regular file do file copy
			char ref[1024];
			sprintf(ref,"%s/%s",inputFile,rddir->d_name);
			char outref[1024];
			sprintf(outref,"%s/%s",outputFile,rddir->d_name);
			CopyFile(ref,outref);
		}
	}
	return 0;//if return 0 is success message
}



int main(int argc,char*argv[]){
	if(argc<3){
		perror("argument length less");
		exit(1);
		//입력 인자의 개수가 부족할 경우 
	}
	if(argc==3){
	
		struct stat buf;
		stat(argv[1],&buf);
		//get input dir info
		
		if(S_ISREG(buf.st_mode)){
                //인자가 만약 파일이라면 파일 copy 실행
                        CopyFile(argv[1],argv[2]);
                        return 0;
                }

		if(S_ISDIR(buf.st_mode)){

			DIR* inputdir;
        	        inputdir = opendir(argv[1]);
	
			//인자가 폴더라면 폴더 copy진행 
			int mkdirFlag=mkdir(argv[2],buf.st_mode);
			//make output folder 
			if(mkdirFlag!=0){
				perror("mkdir");
				exit(1);
			}
			DIR * outputdir;
			outputdir=opendir(argv[2]);
			inputFile[0] ='\0';
			strcat(inputFile,"./");
			strcat(inputFile,argv[1]);
			outputFile[0]='\0';
			strcat(outputFile,"./");
			strcat(outputFile,argv[2]);
			//set folder name
			int res =folderCopy(inputdir,outputdir);
			//do folder copy
			if(res==1){
				perror("folderCopy fail");
				exit(1);
			}
		}
	}
	
	return 0;
}







