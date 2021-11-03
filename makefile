program : ospedale.o triage.o reparto.o prestazione.o
	./ospedale

ospedale.o : librerie.h ospedale.c
	gcc  -o ospedale -Wall -pedantic ospedale.c

triage.o : librerie.h triage.c
	gcc  -o triage -Wall -pedantic triage.c

reparto.o : librerie.h reparto.c
	gcc  -o reparto -Wall -pedantic reparto.c

prestazione.o : librerie.h prestazione.c
	gcc  -o prestazione -Wall -pedantic prestazione.c 
	
clean:
	rm -f ospedale triage reparto prestazione a.out
