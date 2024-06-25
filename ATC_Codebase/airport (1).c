#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <mqueue.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>

struct Plane {
	int Arriv;
	int dept;
	int pid;
	int tweight;
	int type;
	int numofpass;
	int stage;
    int terminate;
};
pthread_mutex_t lock[11];
int lock1[11]={1,1,1,1,1,1,1,1,1,1,1};

int Runway[11]={0};
int numorunway;

key_t key;
int msgid;


struct MyMessage {
    long mtype;
    struct Plane plane;
};

int bestfit(int runway[], int value, int numorunway);
void depature(struct Plane p,int msgid,int Runnway[],int numorunway);
void arrival(struct Plane p,int msgid,int Runnway[],int numorunway);
void* deptthread(void* arg);
void* arrvthread(void* arg);

int main(){
    int Airportid;
    printf("Enter Airport Number:\n");
    scanf("%d",&Airportid);
    int add=Airportid+2000;
    printf("Enter number of Runways:\n");
    
    scanf("%d",&numorunway);
    printf("%d",numorunway);
    struct MyMessage message;

    key = ftok("AirTrafficController.txt", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }




    printf("Enter loadCapacity of Runways (give as a space separated list in a single line end with x):\n");
    printf("debug1\n");
    for (int i = 0; i < numorunway; i++) {
        scanf("%d", &Runway[i]);
    }   
    int args[2]={100,300};
    printf("Best fit %d",bestfit(args,200,2));

    printf("debug2\n");
    int n=0;
    pthread_t tid[100];
    while(1){
        if ((msgrcv(msgid, &message, sizeof(struct MyMessage) - sizeof(long), add, 0) == -1)) {
           exit(1);
        }
        struct Plane p=message.plane;
        
        printf("debug1\n");
        if(p.terminate==1){
            break;
        }
        printf("debug2 %d\n",p.stage);
        if(p.stage==2){
            printf("debug3\n");
            pthread_create(&tid[n], NULL, deptthread, (void*)&p);
            //depature(p,msgid,Runway,numorunway);
            printf("debug4\n");
            n++;
            
        }
        printf("debug5\n");
        if(p.stage==4){
            pthread_create(&tid[n], NULL, arrvthread, (void*)&p);
            //depature(p,msgid,Runway,numorunway);
            n++;
        }
        
        
    }
    for(int i=0;i<n;i++){
          pthread_join(tid[i], NULL);  
    }
    return 0;

}
int bestfit(int runway[], int value, int numorunway) {
    int minDiff = value + 1; 
    int bestIndex = -1; 

    for (int i = 0; i < numorunway; i++) {
        if (lock1[i] && runway[i] >= value) {
            int diff = runway[i] - value; 
            if (diff < minDiff) {
                minDiff = diff;
                bestIndex = i;
            }
        }
    }

    return bestIndex; 
}


void depature(struct Plane p,int msgid,int Runnway[],int numorunway){
    int index=bestfit(Runnway,p.tweight,numorunway);
    printf("%d ,%d,%d,%d",Runnway[0],Runnway[1],p.tweight,numorunway);
    printf("Index=%d \n",index);
    if(index==-1){
        pthread_mutex_lock(&lock[10]);
        lock1[10]=0;
        sleep(3);
        printf("Plane %d has completed boarding/loading and taken off from Runway No. %d of Airport No. %d.\n",p.pid,numorunway+1,p.dept);
        sleep(2);
        struct MyMessage m;
        m.mtype=1008;
        p.stage++;
        m.plane=p;
        if (msgsnd(msgid, &m, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	perror("msgsnd");
        	exit(1);
    	}
        lock1[10]=1;
        pthread_mutex_unlock(&lock[10]);	
    }else{
        pthread_mutex_lock(&lock[index]);
        lock1[index]=0;
        sleep(3);
        
        printf("Plane %d has completed boarding/loading and taken off from Runway No. %d of Airport No. %d.\n",p.pid,index+1,p.dept);
        sleep(2);
        struct MyMessage m;
        m.mtype=1008;
        p.stage++;
        m.plane=p;
        if (msgsnd(msgid, &m, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	perror("msgsnd");
        	exit(1);
    	}
        lock1[index]=1;
        pthread_mutex_unlock(&lock[index]);
    }
}

void* deptthread(void* arg){
    struct Plane* p = (struct Plane*)arg;
    depature(*p,msgid,Runway,numorunway);
    pthread_exit(NULL);
}



void arrival(struct Plane p,int msgid,int Runnway[],int numorunway){
    int index=bestfit(Runnway,p.tweight,numorunway);
    printf("Index=%d \n",index);
    if(index==-1){
        pthread_mutex_lock(&lock[10]);
        printf("Inside me");
        lock1[10]=0;
        sleep(3);
        printf("Plane %d has landed on Runway No. %d of Airport No. %d and has completed deboarding/unloading.\n",p.pid,numorunway+1,p.dept);
        sleep(2);
        struct MyMessage m;
        m.mtype=1008;
        p.stage++;
        m.plane=p;
        if (msgsnd(msgid, &m, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	perror("msgsnd");
        	exit(1);
    	}
        lock1[10]=1;
        pthread_mutex_unlock(&lock[10]);	
    }else{
        pthread_mutex_lock(&lock[index]);
        lock1[index]=0;
        sleep(3);
        printf("Plane %d has landed on Runway No. %d of Airport No. %d and has completed deboarding/unloading.\n",p.pid,index+1,p.dept);
        sleep(2);
        struct MyMessage m;
        m.mtype=1008;
        p.stage++;
        m.plane=p;
        if (msgsnd(msgid, &m, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	perror("msgsnd");
        	exit(1);
    	}
        lock1[index]=1;
        pthread_mutex_unlock(&lock[index]);
    }
}
void* arrvthread(void* arg){
    struct Plane* p = (struct Plane*)arg;
    arrival(*p,msgid,Runway,numorunway);
    pthread_exit(NULL);
}
