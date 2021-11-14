program : hospital.o triage.o ward.o service.o
	./hospital

hospital.o : lib/lib.h src/hospital.c
	gcc  -o hospital -Wall -pedantic hospital.c

triage.o : lib/lib.h src/triage.c
	gcc  -o triage -Wall -pedantic triage.c

ward.o : lib/lib.h src/ward.c
	gcc  -o ward -Wall -pedantic ward.c

service.o : lib/lib.h src/service.c
	gcc  -o service -Wall -pedantic service.c
	
clean:
	rm -f hospital triage ward service a.out
