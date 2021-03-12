#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define MaxNum		20
typedef struct Bank{
	int money[MaxNum];
	int accountNum;
	pthread_rwlock_t rwlock[MaxNum];
	pthread_t pid[MaxNum];
	int clientNum;
	pthread_mutex_t lock;
}sBank,*pBank;

 
sBank gBank; 


void WithdrawsMoney(char* accout,int id,int money)
{
	pthread_rwlock_wrlock(&gBank.rwlock[id-1]);
	sleep(5);
	if(gBank.money[id-1] - money >= 0)
	{
		gBank.money[id-1] -= money;
		printf("%s-w-a%d-%d\na%d:%d\n",accout,id,money,id,gBank.money[id-1]);		
	}
	pthread_rwlock_unlock(&gBank.rwlock[id-1]);
} 


void DepositsMoney(char* accout,int id,int money)
{
	pthread_rwlock_wrlock(&gBank.rwlock[id-1]);
	sleep(5);
	gBank.money[id-1] += money;
	printf("%s-d-a%d-%d\na%d:%d\n",accout,id,money,id,gBank.money[id-1]);	
	pthread_rwlock_unlock(&gBank.rwlock[id-1]);
} 

void *Thread(void *arg)
{
	char cmd[200];
	strcpy(cmd,arg);
	int len = strlen(cmd);
	int pos = 0;
	char str1[10],str2[10],str3[10],str4[10];
	char accout[10];
	//printf("Thread:%s - %d\n",cmd,len);
	while(pos < len)
	{
		//printf("%d\n",pos);
		sscanf(cmd+pos,"%s",str1);
		if(str1[0] == 'c')
		{
		//	printf("c:%s\n",str1);
			sscanf(cmd+pos,"%s",accout);
			pos += strlen(str1) + 1;
		}
		else if(str1[0] == 'd')
		{
			sscanf(cmd+pos,"%s %s %s",str1,str2,str3);
			//printf("d:%s %s %s\n",str1,str2,str3);
			pos += strlen(str1) + 1;
			pos += strlen(str2) + 1;
			pos += strlen(str3) + 1;
			DepositsMoney(accout,atoi(str2+1),atoi(str3));
		}
		else if(str1[0] == 'w')
		{
			sscanf(cmd+pos,"%s %s %s",str1,str2,str3);
			//printf("w:%s %s %s\n",str1,str2,str3);
			pos += strlen(str1) + 1;
			pos += strlen(str2) + 1;
			pos += strlen(str3) + 1;
			WithdrawsMoney(accout,atoi(str2+1),atoi(str3));			
		}
		else if(str1[0] == 't')
		{
			sscanf(cmd+pos,"%s %s %s %s",str1,str2,str3,str4);
			//printf("t:%s %s %s %s\n",str1,str2,str3,str4);
			pos += strlen(str1) + 1;
			pos += strlen(str2) + 1;
			pos += strlen(str3) + 1;
			pos += strlen(str4) + 1;
			WithdrawsMoney(accout,atoi(str2+1),atoi(str4));
			DepositsMoney(accout,atoi(str3+1),atoi(str4));			
		}		
	}
}



void Parse(char* cmd)
{
	char name[10];
	char operate;
	int money;
	if(cmd[0] == 'a')
	{
		sscanf(cmd,"%s %c %d",name,&operate,&money);
		gBank.money[gBank.accountNum] = money;
		//gBank.rwlock[gBank.accountNum] = PTHREAD_RWLOCK_INITIALIZER;
		pthread_rwlock_init(&gBank.rwlock[gBank.accountNum],NULL);
		gBank.accountNum++;
	}
	else if(cmd[0] == 'c')
	{
		printf("Parse:%s\n",cmd);
		if(pthread_create(&gBank.pid[gBank.clientNum], NULL, Thread, (void*)cmd) != 0)
		{
			printf("Failed to create pthread.\n");
		}
		gBank.clientNum++;
		sleep(1);
	}
}


int ReadFile(char *filename)
{
	FILE *file;
	char buff[1024];
	file = fopen(filename,"r");
	if(NULL == file)
	{
		printf("Failed to open %s.",filename);
		return 1; 
	}
	while(!feof(file))
	{
		fgets(buff,1024,file);
		Parse(buff);
		memset(buff,0,1024);
	}
	fclose(file);
	return 0;
}

int WriteFile(char *filename)
{
	int i;
	FILE *file;
	file = fopen(filename,"w");	
	for(i = 0 ; i < gBank.accountNum ; i++)
	{
		printf("a%i b %d\n",i+1,gBank.money[i]);
		fprintf(file,"a%i b %d\n",i+1,gBank.money[i]);
	}
	fclose(file);
	return 0;	
}


int main(int argc, char *argv[]) {
	int i;
	if(2 != argc)
	{
		return 1;	
	}
	
	memset(&gBank,0,sizeof(sBank));
 
	if(1 == ReadFile(argv[1]))
	{
		return 1;
	}
	

	for(i = 0 ; i < gBank.clientNum ; i++)
	{
		pthread_join(gBank.pid[i], NULL);	
	} 

	
	WriteFile("expected_output.txt");	
	return 0;
}
