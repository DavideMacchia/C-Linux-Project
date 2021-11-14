#include "../lib/lib.h"

void termina(int sig);

union semun {
		int val;                  
    	struct semid_ds *buf;     
    	unsigned short *array;                                         
   		struct seminfo *__buf;    
};
union semun arg;

struct sembuf incrementa_semaforo = {0, 1, 0};

//VARIABILI
FILE* file_prest;
int coda_id;
int sem_id;


int main(int argc, char *argv[]){
	
	signal(SIGTERM, termina);
	
	//INIZIALIZZAZIONE CODA tramite nome FIFO
	char *fifo;
	if((fifo = (char*) malloc (sizeof(char) * 10)) == NULL){
		perror("Error in function: 'malloc'\n");
		exit(EXIT_FAILURE);
	}
	strcpy(fifo, argv[0]);
	
	
	//INIZIALIZZAZIONE CODA
	coda_id = apri_coda(fifo[5]);
	
	//INIZIALIZZAZIONE SEMAFORO
	sem_id = apri_semaforo('b');
	
	
	
	int attesa;
	char file_name[25];
	strcpy(file_name, "LOG_prestazione_");
	strcat(file_name, &fifo[5]);
	strcat(file_name, ".txt");
	
	while(1){
		//PRELEVA PAZIENTE
        	if(msgrcv(coda_id, &buffer_coda, sizeof(queue_buffer), -21, 0) == -1){
				perror("Error in function: 'msgrcv'\n");
				exit(EXIT_FAILURE);
			}
            paziente = buffer_coda.mtext;
			
            //CREAZIONE ATTESA
			srand(paziente.gravita);
            attesa = (rand() % 10) + 1;
		
            //STAMPA A VIDEO ENTRATA PAZIENTE e CREAZIONE FILE DI LOG(service.txt)
            if((file_prest = fopen(file_name, "a")) == NULL){
                perror("Error in function: 'fopen'\n");
				exit(EXIT_FAILURE);
            }
            printf("PAZIENTE '%s %s':\n", buffer_coda.mtext.nome, buffer_coda.mtext.cognome);
            printf("----ENTRA in Prestazione\n");
			printf("----ATTESA: %d\n",attesa);
			fprintf(file_prest, "PAZIENTE '%s %s':\n", buffer_coda.mtext.nome, buffer_coda.mtext.cognome);
            fprintf(file_prest, "----ENTRA in Prestazione\n");
			fprintf(file_prest, "----ATTESA: %d\n",attesa);
            
			//ATTENDI PRESTAZIONE
            sleep(attesa);
		
			printf("PAZIENTE '%s %s':\n", buffer_coda.mtext.nome, buffer_coda.mtext.cognome);
            printf("----ESCE da Prestazione\n");
			printf("----ATTESA: Terminata\n");
			fprintf(file_prest, "PAZIENTE '%s %s':\n", buffer_coda.mtext.nome, buffer_coda.mtext.cognome);
            fprintf(file_prest, "----ESCE da Prestazione\n");
			fprintf(file_prest, "----ATTESA: Terminata\n");
				
			fclose(file_prest);
		
            //INCREMENTA SEMAFORO,libera un posto nel ciclo while dell'hospital
            if(semop(sem_id, &incrementa_semaforo, 1) == -1){
                perror("Error in function: 'semop-PRESTAZIONE\n'");
				exit(EXIT_FAILURE);
            }
			sleep(1);
	}
}

void termina(int sig){
	exit(EXIT_SUCCESS);
}