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

void updateResponseTime(int* isDone,int* response_Time,int size,int quantum);
void getAverageTime(int end,char** name,int* burstTime,int size,int quantum);
void waitingTime(int* response_Time,int size, int* burstTime,int* waiting_Time,int quantum);
void turnAroundTime(int size, int* burstTime, int* waitingTime,int* turnAroundTime);


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

	struct node* aux = head;
	struct node* sorted;

	for(int j = 0;j < 20;j++) {
		for(int k = 0;k < counter;k++) {
			if(aux->task->priority == j) {
				insert(&sorted,aux->task);
				delete(&aux,aux->task);
			}
			else aux = aux->next;
		}
		aux = head;
	}

	int size = counter;
	int* priority_array = malloc(sizeof(int)*size);
	int *burst_time = malloc(sizeof(int)*size);
	int quantum = 10;
	int time = 0;
	int done = 0;

	//coloca os valores do nome,prioridade e burst
	struct node* temp = sorted;
	char** id = malloc(sizeof(char*)*counter);
	for(int i = counter-1;i >= 0;i--) {
		id[i] = malloc(sizeof(char)*(strlen(temp->task->name)+1));
		strcpy(id[i],temp->task->name);
		burst_time[i] = temp->task->burst;
		priority_array[i] = temp->task->priority;
		temp = temp->next;
	}


	int* remaining_burstTime = malloc(sizeof(int)*size);

	for(int i = 0;i < size;i++) {
		remaining_burstTime[i] = burst_time[i]; 
	}

	int end;
	for(int i = 0;i < size;i++) {
		end = i;
		for(int j = end;j < size;j++){
			if(priority_array[i] == priority_array[j]) end++;
		}

		if(priority_array[i] > priority_array[i-1]) {
			//aqui
			done = 0;

			while(!done) {

				for(int k = i;k < end;k++) {
					
					done = 1;

					if(remaining_burstTime[k] > 0) {

						done = 0;

						if(remaining_burstTime[k] > quantum){
							time += quantum;
							remaining_burstTime[k] -= quantum;
						}else{
							done = 1;
							time = time + remaining_burstTime[k];
							waiting_Time[k] = time - burst_time[k];
							response_Time[k] = 0;
						}
					}
				}
			}
		} 
	}

	for(int i = 0;i < counter;i++) {
		turn_Around_Time[i] = waiting_Time[i] + burst_time[i];
	}


	int totalWaitingTime = 0;
	int totalTurnAroundTime = 0;
	int totalresponseTime = 0;


	FILE* fp;
	fp = fopen("priority_rr.txt","w");

	fprintf(fp,"EXECUTION BY priority_RR\n");

	for(int i = 0;i < counter;i++) {
		fprintf(fp, "%s for %d units\n",id[i],burst_time[i]);
	}

	fprintf(fp, "\nMETRICS\n");

	for (int i=0; i < counter; i++) {
		totalWaitingTime += waiting_Time[i];
		totalTurnAroundTime += turn_Around_Time[i];
		totalresponseTime += response_Time[i];
		fprintf(fp, "\n%s\n",id[i]);
		fprintf(fp, "Turnaround Time: %d\n", turn_Around_Time[i]);
		fprintf(fp, "Waiting Time: %d\n", waiting_Time[i]);
		fprintf(fp, "response Time: %d\n", response_Time[i]);
	}

	fprintf(fp,"\nAverage waiting time = %.2f", (float)totalWaitingTime / (float)counter);
	fprintf(fp,"\nAverage turnaround time = %.2f\n", (float)totalTurnAroundTime / (float)counter);
	fprintf(fp,"Average response time = %.2f\n", (float)totalresponseTime / (float)counter);
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


void getAverageTime(int end,char** name,int* burstTime,int size,int quantum)
{

	int totalWaitingTime = 0;
	int totalTurnAroundTime = 0;
	int totalresponseTime = 0;
	int* waiting_Time = malloc(sizeof(int)*size);
	int* turn_Around_Time = malloc(sizeof(int)*size);
	int* response_Time = calloc(0,sizeof(int)*size);
	
	waitingTime(response_Time,end,burstTime,waiting_Time,quantum);
	turnAroundTime(end,burstTime,waiting_Time,turn_Around_Time);

	FILE* fp;
	fp = fopen("priority_rr.txt","a");

	fprintf(fp,"EXECUTION BY RR\n");

	for(int i = 0;i < end;i++) {
		fprintf(fp, "%s for %d units\n",name[i],burstTime[i]);
	}

	fprintf(fp, "\nMETRICS\n");

	for (int i=end; i>=0; i--) {
		totalWaitingTime += waiting_Time[i];
		totalTurnAroundTime += turn_Around_Time[i];
		totalresponseTime += response_Time[i];
		fprintf(fp, "\n%s\n",name[i]);
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
