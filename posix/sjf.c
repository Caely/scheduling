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
void writeFILE(struct node* nomes,char** name,int* waiting_Time,int* turn_Around_Time,int* response_Time,int* burst);


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
printf("aqui\n");
	int* burst = malloc(sizeof(int)*counter);
	aloca_burst(burst);

	int* waiting_Time = malloc(sizeof(int)*counter);
	int* turn_Around_Time = malloc(sizeof(int)*counter);
	int* response_Time = malloc(sizeof(int)*counter);

	struct node* aux = head;
	struct node* sorted;

	for(int j = 0;j < 40;j++) {
		for(int k = 0;k < counter;k++) {
			if(aux->task->burst == j) {
				insert(&sorted,aux->task);
				delete(&aux,aux->task);
			}
			else aux = aux->next;
		}
		aux = head;
	}
	

	char** name = malloc(sizeof(char*)*counter);
	struct node* nodeAtual = sorted;
	
	for(int i = 0;i < counter;i++) {
		name[i] = malloc(sizeof(char)*(strlen(nodeAtual->task->name)+1));
		strcpy(name[i],nodeAtual->task->name);
		if(i == 0) {
			waiting_Time[i] = 0;
			burst[i] = nodeAtual->task->burst;
			turn_Around_Time[i] = waiting_Time[i] + burst[i];
			response_Time[i] = waiting_Time[i];
		}

		else {
			waiting_Time[i] = waiting_Time[i-1] + burst[i-1];
			burst[i] = nodeAtual->task->burst;
			turn_Around_Time[i] = burst[i] + waiting_Time[i];
			response_Time[i] = waiting_Time[i];
		}
		nodeAtual = nodeAtual->next;
	}
	nodeAtual = sorted;

	writeFILE(nodeAtual,name,waiting_Time,turn_Around_Time,response_Time,burst);
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


void writeFILE(struct node* nomes,char** name,int* waiting_Time,int* turn_Around_Time,int* response_Time,int* burst) {

	int totalWaitingTime = 0;
	int totalTurnAroundTime = 0;
	int totalresponseTime = 0;


	FILE* fp;
	fp = fopen("sjf.txt","w");

	fprintf(fp,"EXECUTION BY RR\n");

	for(int i = 0;i < counter;i++) {
		fprintf(fp, "%s for %d units\n",name[i],burst[i]);
	}

	fprintf(fp, "\nMETRICS\n");

	for (int i=0; i < counter; i++) {
		totalWaitingTime += waiting_Time[i];
		totalTurnAroundTime += turn_Around_Time[i];
		totalresponseTime += response_Time[i];
		fprintf(fp, "\n%s\n",nomes->task->name);
		fprintf(fp, "Turnaround Time: %d\n", turn_Around_Time[i]);
		fprintf(fp, "Waiting Time: %d\n", waiting_Time[i]);
		fprintf(fp, "response Time: %d\n", response_Time[i]);
		nomes = nomes->next;
	}

	fprintf(fp,"\nAverage waiting time = %.2f", (float)totalWaitingTime / (float)counter);
	fprintf(fp,"\nAverage turnaround time = %.2f\n", (float)totalTurnAroundTime / (float)counter);
	fprintf(fp,"Average response time = %.2f\n", (float)totalresponseTime / (float)counter);
}