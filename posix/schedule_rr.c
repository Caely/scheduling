#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"


struct node* head;
int counter = 0;

void turnAroundTime(int size, int* burstTime, int* waitingTime,int* turnAroundTime);
void waitingTime(int* response_Time, int size, int* burstTime, int* waiting_Time,int quantum);
void getAverageTime(int* burstTime,int size,int quantum);
void updateResponseTime(int* isDone,int* response_Time,int size,int quantum);
void aloca(int* priority,int* burst);


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
	
	int size = counter;
//	char* name = malloc(sizeof(char)*size);
	int* priority_array = malloc(sizeof(int)*size);
	int *burst_time = malloc(sizeof(int)*size);
	int quantum = 10;
	
	aloca(priority_array,burst_time);

	getAverageTime(burst_time,size, quantum);
}



void aloca(int* priority,int* burst) {

	struct node* aux = head;

	for(int i = counter-1;i >= 0;i--) {
		burst[i] = aux->task->burst;
		priority[i] = aux->task->priority;
		aux = aux->next;
	}
}





void turnAroundTime(int size, int* burstTime, 
	int* waitingTime,int* turnAroundTime)
{
	for(int i = 0;i < size;i++)
		turnAroundTime[i] = waitingTime[i] + burstTime[i];
}


void waitingTime(int* response_Time,int size, int* burstTime,
	int* waiting_Time,int quantum)
{
	int* isDone = calloc(0,sizeof(int)*size);

	int* remaining_burstTime = malloc(sizeof(int)*size);

	for(int i = 0;i < size;i++) {
		remaining_burstTime[i] = burstTime[i]; 
	}

	int time = 0;
	int done = 0;

	while(!done) {

		done = 1;
		
		for(int i = 0;i < size;i++) {
			
			if(remaining_burstTime[i] > 0) {
				
				done = 0;
				
				if(remaining_burstTime[i] > quantum) {
					time += quantum;
					remaining_burstTime[i] -= quantum;
				}
				
				else {
					time = time + remaining_burstTime[i];
					waiting_Time[i] = time - burstTime[i];
					remaining_burstTime[i] = 0;
					isDone[i] = 1;
					updateResponseTime(isDone,response_Time,size,quantum);
				}
				
			}
		}
	}
}


void getAverageTime(int* burstTime,int size,int quantum)
{
	int totalWaitingTime = 0;
	int totalTurnAroundTime = 0;
	int totalresponseTime = 0;
	int* waiting_Time = malloc(sizeof(int)*size);
	int* turn_Around_Time = malloc(sizeof(int)*size);
	int* response_Time = calloc(0,sizeof(int)*size);
	
	waitingTime(response_Time,size,burstTime,waiting_Time,quantum);
	turnAroundTime(size,burstTime,waiting_Time,turn_Around_Time);

	FILE* fp;
	fp = fopen("rr.txt","w");

	fprintf(fp,"EXECUTION BY RR\n");

	for(int i = 0;i < counter;i++) {
		fprintf(fp, "[T%d] for %d units\n",i+1,burstTime[i]);
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


void updateResponseTime(int* isDone,int* response_Time,int size,int quantum) {

	for(int i = 0;i < size;i++) {
		if(!isDone[i])
			response_Time[i] += quantum;
	}
}
