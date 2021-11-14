#include "../lib/lib.h"

void termina(int sig);

//VARIABILI
char *fifo;
int id_fifo;

int sem_id;
int coda_id;

pid_t pid;

FILE* file_rep;

int main(int argc, char *argv[]){
	
	signal(SIGTERM, termina);
	
	//INIZIALIZZO CODA CORRISPONDENTE
	coda_id = apri_coda(fifo[5]);
	 
	//PRELEVA FIFO CORRISPONDENTE
	if((fifo = (char*) malloc (sizeof(char) * 10)) == NULL){	
		perror("Error in function: 'malloc'\n");
		exit(EXIT_FAILURE);
	}
	strcpy(fifo, argv[0]);
	
	
	//INIZIALIZZAZIONE PROCESSO 'service.c'
	pid = fork();
	if(pid == -1){
		perror("Error in function: 'fork'\n");
		exit(EXIT_FAILURE);
	}else if(pid == 0){
		if(execlp("./service", fifo, NULL) == -1){
			perror("Error in function: 'execlp'\n");
			exit(EXIT_FAILURE);
		}
	}else{
		char file_name[25];
		strcpy(file_name, "LOG_reparto_");
		strcat(file_name, &fifo[5]);
		strcat(file_name, ".txt");
		
		int counter_mtype = 10;
		while(1){
            //INIZIALIZZAZIONE FIFO
			paziente = read_fifo(fifo);
			
            //GESTIONE ORDINE DI ARRIVO A PRESTAZIONE
            buffer_coda.mtext = paziente;
            buffer_coda.mtype = paziente.gravita + counter_mtype;
			
			counter_mtype--;
			if(counter_mtype == 1){
				 counter_mtype = 10;
			}
			
			//STAMPA A VIDEO PAZIENTE e CREAZIONE FILE DI LOG(reparto.txt)
            if((file_rep = fopen(file_name, "a")) == NULL){
                perror("Error in function: 'fopen'\n");
				exit(EXIT_FAILURE);
            }
			fprintf(file_rep,"PAZIENTE '%s %s':\n", buffer_coda.mtext.nome, buffer_coda.mtext.cognome);
            fprintf(file_rep,"----Trasferito da REPARTO a PRESTAZIONE\n");
            fprintf(file_rep,"----Sintomo: %s\n",buffer_coda.mtext.sintomo);
            fprintf(file_rep,"----Gravità: %d\n",buffer_coda.mtext.gravita);
			fclose(file_rep);
			
            printf("PAZIENTE '%s %s':\n", buffer_coda.mtext.nome, buffer_coda.mtext.cognome);
            printf("----Trasferito da REPARTO a PRESTAZIONE\n");
            printf("----Sintomo: %s\n",buffer_coda.mtext.sintomo);
            printf("----Gravità: %d\n",buffer_coda.mtext.gravita);
			
			
			//MANDA PAZIENTE
            if(msgsnd(coda_id, &buffer_coda, sizeof(pazienti), 0) == -1){
                perror("Error in function: 'msgsnd'\n");
				exit(EXIT_FAILURE);
            }
		}
	}
}
void termina(int sig){
	kill(pid, SIGTERM);
	exit(EXIT_SUCCESS);
}