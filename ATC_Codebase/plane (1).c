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



    	int planeID;
    	printf("Enter Plane ID: \n");
    	scanf("%d", &planeID);
    	printf("Plane ID entered: %d\n", planeID);


    	int planeType;
    	printf("Enter Type of Plane (1 for passenger, 0 for cargo): \n");
    	scanf("%d", &planeType);
    	printf("Type of Plane entered: %d\n", planeType);
		struct Plane p;
		p.pid=planeID;
		p.stage=1;
		p.terminate=0;
		if(planeType==1){
			int num;
			int isparent=1;
			int ischild=-1;
			printf("Enter Number of Occupied Seats:\n");
			scanf("%d", &num);
			pid_t pas[num];
			int mypipe[num][2];
			printf("Enter Weight of Your Luggage: and Your Weight ('Lw Yw')\n");
			for(int i=0;i<num;i++){
				pas[i]=-1;
				if (pipe(mypipe[i]) == -1) { // Create the pipe
    	    		perror("pipe");
    	    		exit(EXIT_FAILURE);
    			}
				if(isparent==1){
					pas[i]=fork();
				}
				if(pas[i]==0){
					ischild=i;
					isparent=0;
					break;
				}
			}
			if(isparent==0){
				printf("%d",ischild);
				close(mypipe[ischild][0]); 
				int lweight,bweight;
				scanf("%d %d",&lweight,&bweight);
				write(mypipe[ischild][1], &lweight, sizeof(int));
        		write(mypipe[ischild][1], &bweight, sizeof(int));
				close(mypipe[ischild][1]);
				return 0;
		
			}
			for(int i=0;i<num;i++){
	 			wait(NULL);
			}
			int data=0;
			for(int i=0;i<num;i++){
				int lweight,bweight;
				read(mypipe[i][0], &lweight, sizeof(int));
        		read(mypipe[i][0], &bweight, sizeof(int));
				data+=lweight;
				data+=bweight;	
			}
			
			printf("Enter Airport Number for Departure:");
			int dept;
			scanf("%d",&dept);
			printf("Enter Airport Number for Arrival:");
			int arv;
			scanf("%d",&arv);
			p.Arriv=arv;
			p.dept=dept;
			p.type=1;
			p.tweight=data+7*75;
			p.numofpass=num;
		}else if(planeType==0){
			printf("Enter Number of Cargo Items:");
			int numofcargo;
			scanf("%d",&numofcargo);
			printf("Enter Average Weight of Cargo Items:");
			int avgweight;
			scanf("%d",&avgweight);
			printf("Enter Airport Number for Departure:");
			int dept;
			scanf("%d",&dept);
			printf("Enter Airport Number for Arrival:");
			int arv;
			scanf("%d",&arv);
			p.Arriv=arv;
			p.dept=dept;
			p.type=0;
			p.tweight=numofcargo*avgweight+2*75;
			p.numofpass=2;

		}
		struct MyMessage m1;
		m1.mtype=1008;
		m1.plane=p;
		if (msgsnd(msgid, &m1, sizeof(struct MyMessage) - sizeof(long), 0) == -1) {
        	perror("msgsnd");
        	exit(1);
    	}
		struct MyMessage m2;
		if ((msgrcv(msgid, &m2, sizeof(struct MyMessage) - sizeof(long), planeID, 0) == -1)) {
            
        }
		
		p=m2.plane;		
		//sleep(30);
		printf("Plane %d has successfully traveled from Airport %d to Airport %d!",m2.plane.pid,m2.plane.dept,m2.plane.Arriv);
		return 0;
	
}



