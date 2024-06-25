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
    printf("Do you want the Air Traffic Control System to terminate?(Y for Yes and N for No)\n");

    key_t key;
    int msgid;
    

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



    while(1){
        char c;
        
        scanf("%c",&c);
        if(c=='y'||c=='Y'){
            struct MyMessage m;
            m.mtype=1010;
            
            if (msgsnd(msgid, &m, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	        perror("msgsnd");
        	        exit(1);
    	    }
            
            return 0;
        }else{
            
        }
        
    }
}