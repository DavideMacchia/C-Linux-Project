#include "librerie.h"


void termina(int sig);

//VARIABILI
int num_reparti;
char *fifo;
char tmp[20];
int counter_rep = 1;

FILE* file_triage;
FILE* file_sint;

int sem_id;
int coda_id;

int id_fifo;
int i;

//array di pid 
pid_t array_pid[10];

int main(){
	
	signal(SIGTERM, termina);

	//PRELEVO num_reparti
	num_reparti = leggi_num_reparti();
	
	
	//ALLOCO MEMORIA PER *FIFO
	if((fifo = (char*) malloc (sizeof(char) * 10)) == NULL){	
		perror("Error in function: 'malloc'\n");
		exit(EXIT_FAILURE);
	}
	strcpy(fifo, "fifo_0");
	
	
	//CREA I REPARTI
	for(i=0; i < num_reparti; i++){
		fifo[5] += 1;
		crea_coda(fifo, fifo[5]);
		
		pid_t pid;
		pid = fork();
		if(pid == -1){
			perror("Error in function: 'fork'\n");
			exit(EXIT_FAILURE);
		}else if(pid == 0){
			if(execlp("./reparto", fifo, NULL) == -1){
				perror("Error in function: 'execlp'\n");	
				exit(EXIT_FAILURE);
			}
		}
		array_pid[i] = pid;
	}
	//Creazione coda
	coda_id = apri_coda('a');
		
	strcpy(fifo, "fifo_1");
	while(1){
		//PRELEVO PAZIENTE DALLA CODA
		if(msgrcv(coda_id, &buffer_coda, sizeof(pazienti), 0, 0) == -1){
			perror("Error in function: 'msgrcv'\n");
			exit(EXIT_FAILURE);
		}
		paziente = buffer_coda.mtext;
		
		//ASSEGNAZIONE GRAVITÀ corrispondente al sintomo del paziente
        if((file_sint = fopen("sintomi.txt", "r")) == NULL){
			perror("Error in function: 'fopen'\n");
			exit(EXIT_FAILURE);
		}
        do{
        	if(fscanf(file_sint, "%s %d", tmp, &paziente.gravita) < 1){
				perror("Error in function: 'fscanf'\n");
				exit(EXIT_FAILURE);
            }
        }while(strcmp(paziente.sintomo, tmp) != 0);
				
        if(fclose(file_sint) == EOF){
            perror("Error in function: 'fclose'\n");
			exit(EXIT_FAILURE);
        }
		
		
		//STAMPA A VIDEO PAZIENTE e CREAZIONE FILE DI LOG(triage.txt)
        if((file_triage = fopen("LOG_triage.txt", "a")) == NULL){
            perror("Errore fopen triage\n");
			exit(EXIT_FAILURE);
        }
		fprintf(file_triage,"PAZIENTE '%s %s':\n", buffer_coda.mtext.nome, buffer_coda.mtext.cognome);
        fprintf(file_triage,"----Trasferito da TRAIGE a REPARTO n°%d\n",counter_rep);
        fprintf(file_triage,"----Sintomo: %s\n",buffer_coda.mtext.sintomo);
        fprintf(file_triage,"----Gravità: %d\n",paziente.gravita);
		fclose(file_triage);
		
        printf("PAZIENTE '%s %s':\n", buffer_coda.mtext.nome, buffer_coda.mtext.cognome);
        printf("----Trasferito da TRAIGE a REPARTO n°%d\n",counter_rep);
        printf("----Sintomo: %s\n",buffer_coda.mtext.sintomo);
        printf("----Gravità: %d\n",paziente.gravita);
		
		
        //INSERIMENTO PAZIENTE IN FIFO
        write_fifo(fifo, &paziente);
		
		
        //INCREMENTA REPARTI 
        //serve perchè mandiamo il primo paziente nel reparto 1,e così via...
        fifo[5] += 1;
        counter_rep++;
        if(counter_rep >= num_reparti + 1){
        	fifo[5] = '1';
        	counter_rep = 1;
        }
	}
	return 0;
}

void termina(int sig){
	for(i=0; i < num_reparti; i++){
		kill(array_pid[i], SIGTERM);
	}
	exit(EXIT_SUCCESS);
}