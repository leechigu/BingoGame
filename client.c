#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <errno.h>


#define SOCKET_NAME	"pro2"
#define PORTNUM 9000


void clearreadbuffer(void){
	while(getchar()!='\n');
}

//union semun{
//        int val;
//        struct semid_ds *buf;
//        unsigned short *array;
//};
//int initsem(key_t semkey){ //세마포어 생성 및 초기화 함수 
//        union semun semunarg;
//        int status = 0, semid;
//        semid =semget(semkey,1,0600);//세마포어 생성
//        if(semid ==-1){
//                if(errno ==EEXIST)
//                        semid =semget(semkey,1,0);
//        }
//        if(semid==1||status==-1){
//                perror("initsem");
//                return (-1);
//        }
//       return semid;
//}
//int semlock(int semid){//세마포어 잠금 함수
//        struct sembuf buf;
//        buf.sem_num = 0;
//        buf.sem_op =-1; //음수로 값을 설정하여 잠금기능 수행
//        buf.sem_flg =SEM_UNDO;
//        if(semop(semid,&buf,1)==-1){
//                perror("semlock failed");
//                exit(1);
//        }
//        return 0;
//}
//int semunlock(int semid){//세마포어 잠금 해제 함수
//        struct sembuf buf;
//        buf.sem_num =0;
//        buf.sem_op=1; //양수로 하여 잠금해제기능 수행 
//        buf.sem_flg = SEM_UNDO;
//        if(semop(semid,&buf,1)==-1){
//                perror("semunlock failed");
//                exit(1);
//        }
//        return 0;
//}




int main(void)
{
	key_t key;
	int semid;
	void *shmaddr;
	char temp[20];
	key =ftok("lee",1);
//	semid = initsem(key);
	

	int sock;
	int str_len;
	struct sockaddr_in server_addr;
	char bingo[25];
	char inputbuf[10];
	int a,b,c=0;
	int cho[20]={0, };
	//선택한 숫자의 index가 들어감

	sock =  socket(PF_INET,SOCK_STREAM ,0);
	//소캣 생성 
	if(sock==-1){
		perror("socket");
		exit(1);
	}

	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(PORTNUM);
	//변수 IP PORT 초기화
	

	srand(time(NULL));

	if(connect(sock,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1){
		perror("connection");
		exit(1);
	}
	//server에 연결 connect

	//빙고판 세팅
	for(a =0;a<25;a++){
		bingo[a] = 'O';
		
	}
	printf("\nBINGO GAME 시작!\n\n");

	for(c=0;c<25;c++){
		printf("%c\t",bingo[c]);
		if((c+1)%5==0)
			printf("\n");
		//5숫자 마다 엔터
	}



	for(a=0;a<10;a++){
		sleep(1);

		int clch;
	
		printf("원하는 위치를 골라주세요(1~25) : ");
		//client 측 좌표 선택 
		memset(inputbuf,'\0',9);	
		fgets(inputbuf,9,stdin);
		clch = atoi(inputbuf);	
		clch -=1;
		while(1){
			if(bingo[clch]!='O'||clch>25||clch<-1){
				
				printf("이미 고른 위치이거나 범위를 벗어났습니다. 다시 골라주세요 : ");
				
				//중복 확인 후 다시 입력
				fgets(inputbuf,9,stdin);
				clch = atoi(inputbuf);
				clch -=1;

			}
			else{
				break;
			}
		}

		bingo[clch] = 'C';
		cho[(a*2)] = clch;
		printf("\n%d. client의 선택은 : %d 번째 \n\n",(a*2)+1,clch+1);
		//몇번째 인지 출력 후 배열의 값은 0부터 시작함으로 +1해서 출력
		for(int t=0;t<25;t++){
			printf("%c\t",bingo[t]);
			if((t+1)%5==0){
				printf("\n");
			}

		}
		printf("\n");

		//가로 세로 빙고 확인 server.c와 동일  
		
		int garo;
		for(b=0;b<5;b++){
			garo = 0;
			if(bingo[b*5]=='C'){
				for(c=1;c<5;c++){
					if(bingo[b*5]==bingo[b*5+c])
						garo++;
					if(garo==4)
						break;
				}
			}
			if(garo ==4)
				break;
		}
		if(garo ==4){
			printf("\n---가로 빙고!---\n");
			cho[(a*2)]=99;
			write(sock,cho,BUFSIZ);
			break;
		}

		int sero;
		for(b=0;b<5;b++){
			sero = 0;
			for(c=1;c<5;c++){
				if(bingo[b]=='C'){
					if(bingo[b]==bingo[b+5*c])
						sero++;
					if(sero ==4)
						break;
				}
			}
			if(sero==4)
				break;
		}	
		if(sero ==4){
			printf("\n---세로 빙고!---\n");
			cho[(a*2)]=99;
			write(sock,cho,BUFSIZ);
			break;
		}

		sleep(1);
//		semlock(semid);
		write(sock,cho,BUFSIZ); //서버로 선택 값 전송
//		semunlock(semid);
		

//		semlock(semid);

		while(1){
			printf("상대방의 선택을 기다리는 중...\n");
			if(read(sock,cho,BUFSIZ)!=-1){
				break;
			}
		}

		//read(sock,cho,BUFSIZ); //서버 측 선택 값 수신
//		semunlock(semid);
		if(cho[(a*2)+1]==99)
			break;
		//빙고 일 경우 break

		printf("%d. 서버 측 선택 : %d 번째\n\n",(a+1)*2,cho[(a*2)+1]+1);

		int svch = cho[(a*2)+1];
		bingo[svch] ='S';
		for(int k=0; k<25;k++){
			printf("%c\t",bingo[k]);
                        if((k+1) % 5 ==0){
                                printf("\n");
                        }

		}
		for(b=0;b<5;b++){
			garo = 0;
			if(bingo[b*5]=='S'){
				for(c=1;c<5;c++){
					if(bingo[b*5]==bingo[b*5+c])
						garo++;
					if(garo ==4)
						break;
				}
			}
			if(garo==4)
				break;
		}
		if(garo ==4){
			printf("\n---가로 빙고!---\n");
			cho[(a*2)]=99;
			write(sock,cho,BUFSIZ);
			break;
		}
	
		for(b =0;b<5;b++){
			sero = 0;
			for(c=1;c<5;c++){
				if(bingo[b] =='S'){
					if(bingo[b] == bingo[b+5*c])
						sero++;
					if(sero==4)
						break;
				}
			}
			if(sero ==4)
				break;
		}
		if(sero==4){
			printf("\n---세로 빙고!---\n");
			cho[(a*2)]=99;
			write(sock,cho,BUFSIZ);
			break;
		}

	}
	//10회 입력 후 종료.
	printf("---Game Finish---");
	close(sock);
	return 0;
}



		








