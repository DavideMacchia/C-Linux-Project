/*
 * File principale del progetto "Hospital OS" 
 * NEXT GOALS
 * -rimuovere le librerie annidate
 * -modularizzare il lavoro
 *
 */

#include "../lib/lib.h"

union semun {
		int val;                  
    	struct semid_ds *buf;     
    	unsigned short *array;                                         
   		struct seminfo *__buf;    
};
union semun arg;

/*prototipi*/
void clean_file_log();
void pulisci();
void termina();

struct sembuf diminuisci_semaforo = {0, -1, 0};

/*variabili*/
char *fifo;
int num_pazienti, num_reparti, num_secondi;	
int rand_sint;
int rand_paz;
int uscita = 1;

int coda_id;
int sem_id;

FILE* file_osp;
FILE* file_log;
FILE* file_sint;
FILE* file_paz;

pid_t pid;


int main(){
	signal(SIGQUIT, pulisci);
	signal(SIGALRM, termina);
	
	clean_file_log();
	
	/*Estrazione dati da file configurazione.txt*/
	num_pazienti = leggi_num_pazienti();
	num_reparti = leggi_num_reparti();
	num_secondi = leggi_num_secondi();
	
	/*controllo dei parametri di configurazione.txt*/
	if((num_reparti > 10) || (num_reparti < 1) || (num_pazienti < 1)){
		perror("Uno o piu' parametri ERRATI in file configurazione.txt\n");
       	exit(EXIT_FAILURE);
	}
	
	coda_id = apri_coda('a');
	
	sem_id = apri_semaforo('b');
	
	
	/*imposto il valore del semaforo a num_pazienti-1*/
	arg.val = num_pazienti-1;
	
	if(semctl(sem_id, 0, SETVAL, arg) == -1){	
		perror("Error in function: 'semctl'");
		exit(EXIT_FAILURE);
	}
	
	
	/*Creazione del processo triage.c attraverso le funzioni fork() e successivamente execlp()*/
	pid = fork();
	if(pid == -1){
		perror("Errore durante la fork\n");
		exit(EXIT_FAILURE);
	}else if(pid == 0){
		if(execlp("./triage", NULL) == -1){
			perror("Errore nell'execlp all'apertura del triage.\n");
			exit(EXIT_FAILURE);
		}
	}else{
		/*Contatore numero pazienti presenti nel file patients.txt*/
		int counter_paz = 0;
		if((file_paz = fopen("patients.txt", "r")) == NULL){
			perror("Errore all'apertura del file \"patients.txt\"\n");
			exit(EXIT_FAILURE);
		}
		while(fscanf(file_paz,"%*s %*s") != EOF){
			counter_paz++;
		}
		if(fclose(file_paz) == EOF){
			perror("Errore durante la chiusura del file \"patients.txt\"\n");
			exit(EXIT_FAILURE);
		}
		
		
		/*Contatore numero sintomi presenti nel file symptoms.txt*/
		int counter_sint = 0;
		if((file_sint = fopen("symptoms.txt", "r")) == NULL){
			perror("Errore durante l'apertura del file \"symptoms.txt\"\n");
			exit(EXIT_FAILURE);
		}
		while(fscanf(file_sint,"%*s %*d") != EOF){
			counter_sint++;
		}
		if(fclose(file_sint) == EOF){
			perror("Errore durante la chiusura del file \"symptoms.txt\"\n");
			exit(EXIT_FAILURE);
		}
		
		
		/*Imposto i secodi che il segnale SIGALRM deve attendere*/
        alarm(num_secondi);
		
        printf("I pazienti entrano...\n");
		printf("\n");
		int j;
		int timer = 0;
		int bool_check = 0;
		int paz_inseriti = 1;
		while(1){
			/*Si inizializzano paziente.nome e paziente.cognome con un paziente selezionato casualmente dal file patients.txt*/
			srand(time(NULL));
			rand_paz = (rand() % counter_paz) + 1;
			
			if((file_paz = fopen("patients.txt", "r")) == NULL){
				perror("Errore all'apertura del file \"patients.txt\"\n");
				exit(EXIT_FAILURE);
			}
			j = 0;
			while(j != rand_paz - 1){
				fscanf(file_paz, "%*s %*s");
				j++;
			}
			if(fscanf(file_paz, "%s %s", paziente.nome, paziente.cognome) != 2){
				perror("Errore durante la lettura del file \"patients.txt\"\n");
				exit(EXIT_FAILURE);
			}
			if(fclose(file_paz) == EOF){
				perror("Errore durante la chiusura del file \"patients.txt\"\n");
				exit(EXIT_FAILURE);
			}
			
			
			/*Si inizializza paziente.sintomo con un sintomo selezionato casualmente dal file symptoms.txt*/
			srand(time(NULL));
			rand_sint = (rand() % counter_sint) + 1;
			
			if((file_sint = fopen("symptoms.txt", "r")) == NULL){
				perror("Errore all'apertura del file \"symptoms.txt\"\n");
				exit(EXIT_FAILURE);
			}
			j = 0;
			while(j != rand_sint - 1){
				fscanf(file_sint, "%*s %*d");
				j++;
			}
			if(fscanf(file_sint, "%s %*d", paziente.sintomo) != 1){
				perror("Errore durante la lettura del file \"symptoms.txt\"\n");
				exit(EXIT_FAILURE);
			}
			if(fclose(file_sint) == EOF){
				perror("Errore durante la chiusura del file \"symptoms.txt\"\n");
				exit(EXIT_FAILURE);
			}
			
			
			/*Inserimento paziente nella coda*/
			strcpy(buffer_coda.mtext.nome, paziente.nome);
			strcpy(buffer_coda.mtext.cognome, paziente.cognome);
			strcpy(buffer_coda.mtext.sintomo, paziente.sintomo);
			
			buffer_coda.mtype = paz_inseriti; 
			
			
			/*STAMPA A VIDEO PAZIENTE e CREAZIONE FILE DI LOG(hospital.txt)*/
			if((file_osp = fopen("LOG_ospedale.txt", "a")) == NULL){
				perror("Errore all'apertura del file \"LOG_ospedale.txt\"\n");
				exit(EXIT_FAILURE);
			}
			fprintf(file_osp,"PAZIENTE '%s %s':\n", buffer_coda.mtext.nome, buffer_coda.mtext.cognome);
			fprintf(file_osp,"----Trasferito da OSPEDALE a TRIAGE\n");
			fprintf(file_osp,"----Sintomo: %s\n",buffer_coda.mtext.sintomo);
			/* A sto punto che sappiamo che il progetto funziona, non vedo più la necessità di stampare
			* la gravità NULL
			fprintf(file_osp,"----Gravità: -\n");*/
			fclose(file_osp);
			
			printf("PAZIENTE '%s %s':\n", buffer_coda.mtext.nome, buffer_coda.mtext.cognome);
			printf("----Trasferito da OSPEDALE a TRIAGE\n");
			printf("----Sintomo: %s\n",buffer_coda.mtext.sintomo);
			/*printf("----Gravità: -\n");*/
			
			if(msgsnd(coda_id, &buffer_coda, sizeof(pazienti), 0) == -1){
				perror("Errore durante la msgsnd \n");
				exit(EXIT_FAILURE);
			}
			/*RIDUZIONE DI UNO VALORE SEMAFORO, nessuna perror o exit(causerebbe problemi in alcuni casi)*/
			semop(sem_id, &diminuisci_semaforo, 1);
			paz_inseriti++;
			
			/*PER EVITARE RISCHIO BLOCCO Ftok*/
			sleep(1);
			
			/*Quando il timer scade...
			ATTESA CHE I PAZIENTI ESCANO*/
			if(uscita == 0){
				while(semctl(sem_id, 0, GETVAL, arg) < num_pazienti && bool_check == 0){
					sleep(1);
					/*USCITA DI SICUREZZA, nel caso il semaforo non sia ben aggiornato(errore +/- 1)*/
					if(semctl(sem_id, 0, GETVAL, arg) == num_pazienti -1){
						timer++;
						if(timer > 11){
							bool_check = 1;
						}
					}else{
						timer = 0;
					}
				}
				/*Chiusura processi e pulizia memoria*/
				kill(pid, SIGTERM);
				sleep(1);
				pulisci();
				printf("Terminazione Programma\n");
			}
		}
	}
	return 0;
}

void termina(){
	printf("\nTEMPO SCADUTO, attesa che tutti i pazienti escano da service\n\n");
	uscita = 0;
	
}

void pulisci(){
		/*Funzione SIGQUIT
		Alloco lo spazio in memoria per la stringa fifo per l'eliminazione delle code dei reparti e delle fifo*/
		if((fifo = (char*) malloc (sizeof(char) * 10)) == NULL){
			perror("Errore durante l'allocazione di memoria\n");
			exit(EXIT_FAILURE);
		}
		strcpy(fifo, "fifo_1");
		
		/*RIMOZIONE CODA OSPEDALE-TRIAGE*/
		coda_id = apri_coda('a');
		if(msgctl(coda_id, IPC_RMID, 0) == -1){
			perror("Errore nel msgctl durante la rimozione coda hospital-triage\n");
			exit(EXIT_FAILURE);
		}
		
		/*RIMOZIONE FIFO e RESTANTI CODE*/
		num_reparti = leggi_num_reparti();
		
		int i;
		for(i=1; i <= num_reparti; i++){
			coda_id = apri_coda(i + '0');
			if(msgctl(coda_id, IPC_RMID, 0) == -1){
				perror("Errore della msgctl \n");
				exit(EXIT_FAILURE);
			}
			if(unlink(fifo) == -1){
				perror("Errore nella funzione unlink \n");
				exit(EXIT_FAILURE);
			}
			fifo[5] += 1;
		}
		
		//RIMOZIONE SEMAFORO
		sem_id = apri_semaforo('b');
		if(semctl(sem_id, 0, IPC_RMID, arg) == -1){
			perror("Error in function: 'semctl'\n");
			exit(EXIT_FAILURE);
		}
		
		printf("Operazioni completate con successo:\n");
		printf("- Eliminazione delle strutture dinamiche in memoria: FIFO, CODE, SEMAFORI\n");	
		exit(EXIT_SUCCESS);
	}

void clean_file_log(){
		//Eliminazione file di log hospital
		if((file_log = fopen("LOG_ospedale.txt", "w")) == NULL){
			perror("Error in function: 'fopen'\n");
			exit(EXIT_FAILURE);
		}
		if(remove("LOG_ospedale.txt") == -1){
			perror("Error in function: 'remove'\n");
			exit(EXIT_FAILURE);
		}
		//Eliminazione file di log triage
		if((file_log = fopen("LOG_triage.txt", "w")) == NULL){
			perror("Error in function: 'fopen'\n");
			exit(EXIT_FAILURE);
		}
		if(remove("LOG_triage.txt") == -1){
			perror("Error in function: 'remove'\n");
			exit(EXIT_FAILURE);
		}
		int i;
		char file_name[25];
		char *tmp;
		if((tmp = (char*) malloc (sizeof(char) * 5)) == NULL){	
			perror("Error in function: 'malloc'");
			exit(EXIT_FAILURE);
		}
		strcpy(tmp, "0");
		for(i=0; i<10; i++){
			tmp[0] += 1;
			//Pulizia file di log prestazioni
			strcpy(file_name, "LOG_prestazione_");
			strcat(file_name, &tmp[0]);
			strcat(file_name, ".txt");
			if((file_log = fopen(file_name, 'w')) != NULL){
				if(remove(file_name) == -1){
					perror("Error in function: 'remove'\n");
					exit(EXIT_FAILURE);
				}
			}
			//Pulizia file di log reparti
			strcpy(file_name, "LOG_reparto_");
			strcat(file_name, &tmp[0]);
			strcat(file_name, ".txt");
			if((file_log = fopen(file_name, 'w')) != NULL){
				if(remove(file_name) == -1){
					perror("Error in function: 'remove'\n");
					exit(EXIT_FAILURE);
				}
			}
		}
		printf("--COMPLETATA--Pulizia File di LOG obsoleti--\n");
}