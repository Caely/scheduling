#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

struct node* head;
int counter = 0;

void add(char* name,int priority,int burst);
void schedule();
void aloca_burst(int* aloca_burst);
int getwait(int* burst_time,int* waiting_Time, int size);
void gettat(int* turn_Around_Time,int* burst_time,int* waiting_Time,int i);
void writeFILE(int* waiting_Time,int* turn_Around_Time,int* response_Time,int* burst);


void add(char* name,int priority,int burst) {

	counter++;

	Task* newTask = malloc(sizeof(Task));
	newTask->name = malloc(sizeof(char)*(strlen(name)+1));
	strcpy(newTask->name,name);
	newTask->priority = priority;
	newTask->burst = burst;

	insert(&head,newTask);
}

void schedule() {
	
	int* burst = malloc(sizeof(int)*counter);
	aloca_burst(burst);

	int* waiting_Time = malloc(sizeof(int)*counter);
	int* turn_Around_Time = malloc(sizeof(int)*counter);
	int* response_Time = malloc(sizeof(int)*counter);
	

	for(int i = counter-1;i >= 0;i--) {
		waiting_Time[i] = getwait(burst,waiting_Time,i);
		gettat(turn_Around_Time,burst,waiting_Time,i);
		response_Time[i] = waiting_Time[i];
		printf("p[%d] -b %d - wt - %d -tat %d\n",i+1,burst[i],waiting_Time[i],turn_Around_Time[i]);
	}


	struct node* aux = head;

	for(int i = 0;i < counter;i++) {
		run(aux->task,i);
		aux = aux->next;
	}

	writeFILE(waiting_Time,turn_Around_Time,response_Time,burst);
}


void aloca_burst(int* aloca_burst) {

	struct node* aux = head;

	for(int i = counter-1;i >= 0;i--) {
		aloca_burst[i] = aux->task->burst;
		aux = aux->next;
	}
}


int getwait(int* burst_time,int* waiting_Time, int size) {

	int time;
	if(size == counter-1)
		time = 0;
	else 
		time = burst_time[size+1] + waiting_Time[size+1];

	return time;
}


void gettat(int* turn_Around_Time,int* burst_time,int* waiting_Time,int i) {

	turn_Around_Time[i] = burst_time[i] + waiting_Time[i];
}


void writeFILE(int* waiting_Time,int* turn_Around_Time,int* response_Time,int* burst) {

	int totalWaitingTime = 0;
	int totalTurnAroundTime = 0;
	int totalresponseTime = 0;


	FILE* fp;
	fp = fopen("fcfs.txt","w");

	fprintf(fp,"EXECUTION BY FCFS\n");

	for(int i = 0;i < counter;i++) {
		fprintf(fp, "[T%d] for %d units\n",i+1,burst[i]);
	}

	fprintf(fp, "\nMETRICS\n");

	for (int i=counter-1; i>=0; i--) {
		totalWaitingTime += waiting_Time[i];
		totalTurnAroundTime += turn_Around_Time[i];
		totalresponseTime += response_Time[i];
		fprintf(fp, "\n[T%d]\n",i+1);
		fprintf(fp, "Turnaround Time: %d\n", turn_Around_Time[i]);
		fprintf(fp, "Waiting Time: %d\n", waiting_Time[i]);
		fprintf(fp, "response Time: %d\n", response_Time[i]);
	}

	fprintf(fp,"\nAverage waiting time = %.2f", (float)totalWaitingTime / (float)counter);
	fprintf(fp,"\nAverage turnaround time = %.2f\n", (float)totalTurnAroundTime / (float)counter);
	fprintf(fp,"Average response time = %.2f\n", (float)totalresponseTime / (float)counter);
}