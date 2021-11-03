#include <stdio.h>
#include <stdlib.h>

#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/stat.h>

#include <unistd.h>
#include <unistd.h>
#include <string.h>

#include <time.h>
#include <fcntl.h>
#include <signal.h>

//STRUTTURE
	typedef struct paz{
		char nome[20];
		char cognome[20];
		char sintomo[20];
		int gravita;
		int num_turno;
	}pazienti;

	pazienti paziente;


	typedef struct msgbuf{
    	long mtype;
    	pazienti mtext;
	}queue_buffer;

	queue_buffer buffer_coda;


//FUNZIONI
int apri_coda(char seed){
	int coda_id;
	key_t key_coda;
	key_coda = ftok(".", seed);
	if(key_coda == -1){
		perror("Error in function: 'ftok'\n");
		exit(EXIT_FAILURE);
	}
	coda_id = msgget(key_coda, 0666);
	if(coda_id == -1){
		perror("Error in function: 'msgget'\n");
		exit(EXIT_FAILURE);
	}
	return coda_id;
}


int apri_semaforo(char seed){
	key_t  key_sem;
	int sem_id;
	key_sem = ftok(".", seed);
	if(key_sem == -1){
		perror("Error in function: 'ftok'\n");
		exit(EXIT_FAILURE);
    }
	sem_id = semget(key_sem, 1, IPC_CREAT|0666);
	if(sem_id == -1){
		perror("Error in function: 'semget'\n");
		exit(EXIT_FAILURE);
	}
	return sem_id;
}


void crea_coda(char* fifo, char x){
	key_t key_coda;
	if(mkfifo(fifo, 0666) == -1){
		perror("Error in function: 'mkfifo'\n");
		exit(EXIT_FAILURE);
	}
	if((key_coda = ftok(".", x)) == -1){
		perror("Error in function: 'ftok'\n");
		exit(EXIT_FAILURE);
	}
	if(msgget(key_coda, IPC_CREAT|0666) == -1){
		perror("Error in function: 'msgget'\n");
		exit(EXIT_FAILURE);
	}
}

void write_fifo(char *fifo, pazienti *paziente){
	int id_fifo;
	id_fifo = open(fifo, O_WRONLY);
	if(id_fifo == -1){
    	perror("Error in function: 'open'\n");
		exit(EXIT_FAILURE);
    }
    if(write(id_fifo, paziente, sizeof(pazienti)) == -1){
    	perror("Error in function: 'write'\n");
		exit(EXIT_FAILURE);
    }
	if(close(id_fifo) == -1){
        perror("Error in function: 'close'\n");
		exit(EXIT_FAILURE);
	}
}


pazienti read_fifo(char *fifo){
	int id_fifo;
	id_fifo = open(fifo, O_RDONLY);
	if(id_fifo == -1){
    	perror("Error in function: 'open'\n");
		exit(EXIT_FAILURE);
    }
    if(read(id_fifo, &paziente, sizeof(pazienti)) == -1){
    	perror("Error in function: 'write'\n");
		exit(EXIT_FAILURE);
    }
	if(close(id_fifo) == -1){
        perror("Error in function: 'close'\n");
		exit(EXIT_FAILURE);
	}
	return paziente;
}

int leggi_num_pazienti(){
	FILE* file_config;
	int num_pazienti;
	if((file_config = fopen("configurazione.txt", "r")) == NULL){
		perror("Error in fuction: 'fopen'\n");
		exit(EXIT_FAILURE);
	}
	if(fscanf(file_config, "%*s %d", &num_pazienti) == 0){
		perror("Error in function: 'fscanf'\n");
		exit(EXIT_FAILURE);
	}
	if(fclose(file_config) == EOF){
		perror("Error in function: 'fclose'\n");
		exit(EXIT_FAILURE);
	}
	return num_pazienti;
}


int leggi_num_reparti(){
	FILE* file_config;
	int num_reparti;
	if((file_config = fopen("configurazione.txt", "r")) == NULL){
		perror("Error in fuction: 'fopen'\n");
		exit(EXIT_FAILURE);
	}
	if(fscanf(file_config, "%*s %*d %*s %d", &num_reparti) == 0){
		perror("Error in function: 'fscanf'\n");
		exit(EXIT_FAILURE);
	}
	if(fclose(file_config) == EOF){
		perror("Error in function: 'fclose'\n");
		exit(EXIT_FAILURE);
	}
	return num_reparti;
}


int leggi_num_secondi(){
	FILE* file_config;
	int num_secondi;
	if((file_config = fopen("configurazione.txt", "r")) == NULL){
		perror("Error in fuction: 'fopen'\n");
		exit(EXIT_FAILURE);
	}
	if(fscanf(file_config, "%*s %*d %*s %*d %*s %d", &num_secondi) == 0){
		perror("Error in function: 'fscanf'\n");
		exit(EXIT_FAILURE);
	}
	if(fclose(file_config) == EOF){
		perror("Error in function: 'fclose'\n");
		exit(EXIT_FAILURE);
	}
	return num_secondi;
}

int[] parse_config_file(){
	FILE* file_config;
	int config_values[3];
	if((file_config = fopen("configurations.txt", "r")) == NULL){
		perror("Error in fuction: 'fopen'\n");
		exit(EXIT_FAILURE);
	}
	if(fscanf(file_config, "%*s %d %*s %d %*s %d", &config_values[0], &config_values[1], &config_values[2]) == 0){
		perror("Error in function: 'fscanf'\n");
		exit(EXIT_FAILURE);
	}
	if(fclose(file_config) == EOF){
		perror("Error in function: 'fclose'\n");
		exit(EXIT_FAILURE);
	}
	return config_values;
}
