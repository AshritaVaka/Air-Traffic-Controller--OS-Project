#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <mqueue.h>

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

struct MyMessage {
    long mtype;
    struct Plane plane;
};

int main(){
    
    key_t key;
    int msgid;
    struct MyMessage message;

    // Generate the same key used to create the message queue
    key = ftok("AirTrafficController.txt", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    // Get the message queue ID
    msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    // Receive the message with message type 1008
    printf("Enter the number of airports to be handled/managed:\n");
    int numofplane;
    scanf("%d",&numofplane);
    struct Plane p=message.plane;
    //printf("%d %d %d %d",p.pid,p.stage,p.Arriv,p.dept);
    int endflag=1;
    int loop=1;
    int stats=0;
    printf("Debug 1\n");
    while(loop){
        if ((msgrcv(msgid, &message, sizeof(struct MyMessage) - sizeof(long), 1010, IPC_NOWAIT) != -1)) {
            endflag=0;
        }
        printf("Debug 2\n");
        if(endflag!=0){
            if((msgrcv(msgid, &message, sizeof(struct MyMessage) - sizeof(long), 1008, 0) == -1)){
                
            }
            printf("Debug 3\n");
            struct Plane p=message.plane;
            if(p.stage==1){
                p.stage++;
                stats++;
                struct MyMessage ms;
                ms.plane=p;
                ms.mtype=p.Arriv+2000;
                if (msgsnd(msgid, &ms, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	        perror("msgsnd");
        	        exit(1);
    	        }	
            }
            if(p.stage==3){
                p.stage++;
                printf("Hit");
                struct MyMessage ms;
                ms.plane=p;
                ms.mtype=p.dept+2000;
                FILE *file = fopen("AirTrafficController.txt", "a");
                fprintf(file, "Plane %d has departed from Airport %d and will land at Airport %d.\n",p.pid, p.dept,p.Arriv);	
                fclose(file);
                sleep(24);
                if (msgsnd(msgid, &ms, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	        perror("msgsnd");
        	        exit(1);
    	        }
                
            }
            if(p.stage==5){
                p.stage++;
                stats--;
                struct MyMessage ms;
                ms.plane=p;
                ms.mtype=p.pid;
                if (msgsnd(msgid, &ms, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	        perror("msgsnd");
        	        exit(1);
    	        }	
            }
            
        }else{
            if(stats==0){
                struct Plane p;
                p.terminate=1;
                struct MyMessage m;
                m.plane.terminate=1;
                int x=2001;
                for(int i=0;i<numofplane;i++){
                    m.mtype=x;
                    if (msgsnd(msgid, &m, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	            perror("msgsnd");
        	            exit(1);
    	            }
                    x++;
                }
                sleep(20);
                if (msgctl(msgid, IPC_RMID, NULL) == -1) {
                    perror("msgctl");
                    exit(EXIT_FAILURE);
                }
                return 0;
            }else{
                if((msgrcv(msgid, &message, sizeof(struct MyMessage) - sizeof(long), 1008, 0) == -1)){
                    exit(1);
                }
                struct Plane p=message.plane;
                if(p.stage==1){
                    p.stage=7;
                    
                    struct MyMessage ms;
                    ms.plane=p;
                    ms.mtype=p.pid;
                    if (msgsnd(msgid, &ms, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	            perror("msgsnd");
        	            exit(1);
    	            }	
                }
                if(p.stage==3){
                    p.stage++;
                    struct MyMessage ms;
                    ms.plane=p;
                    ms.mtype=p.dept+2000;
                    if (msgsnd(msgid, &ms, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	            perror("msgsnd");
        	            exit(1);
    	            }	
                }
                if(p.stage==5){
                    p.stage++;
                    stats--;
                    struct MyMessage ms;
                    ms.plane=p;
                    ms.mtype=p.pid;
                    if (msgsnd(msgid, &ms, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	            perror("msgsnd");
        	            exit(1);
    	            }	
                }
            }

        }


    }
}

