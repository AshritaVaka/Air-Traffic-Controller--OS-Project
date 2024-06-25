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
struct MyMessage {
    long mtype;
    struct Plane plane;
};
int main(){
    int incomadd;
    int sendadd;
    scanf("%d",&incomadd);
    scanf("%d",&sendadd);
    key_t key;
    int msgid;
		
	key = ftok("AirTrafficController.txt", 65);
	if (key == -1) {
        perror("ftok");
    	exit(1);
    }

    	// Create a message queue
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
	    perror("msgget");
	    exit(1);
	}
    while(1){
        printf("Make a choice 1) send 2)recieve");
        int c;
        scanf("%d",&c);
        if(c==1){
            struct Plane p={1,2,1,1000,5,7,0};
            p.Arriv=1;
            p.dept=2;
            p.tweight=1000;
            p.stage=3;
            struct MyMessage m;
            m.mtype=sendadd;
            m.plane=p;
            if (msgsnd(msgid, &m, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	    perror("msgsnd");
            	exit(1);
    	    }


        }else if(c==2){
            struct MyMessage m;
            if ((msgrcv(msgid, &m, sizeof(struct MyMessage) - sizeof(long), incomadd, IPC_NOWAIT) == -1)) {
                perror("msgsnd");
            	exit(1);
            }
            struct Plane p=m.plane;
            printf("%d %d %d %d %d %d",p.Arriv,p.dept,p.numofpass,p.tweight,p.type,p.pid);
        }else{
            return 0;
        }
    }
}