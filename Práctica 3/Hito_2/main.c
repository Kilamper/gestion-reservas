// test_sala.c
// ===============
// Batería de pruebas de la biblioteca "sala.h/sala.c"
//

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "sala.h"
#include "retardo.h"

void* rutina(void* arg) {
	int param = *((int*) arg);
	int asientos[3];
	for (int i = 0; i < 3; i++) {
	  asientos[i] = reserva_asiento(param);
	  if (asientos[i] == -1) {
	    fprintf(stderr, "ERROR: No hay asientos disponibles para reservar\n\n");
	    pthread_exit((void*) -1);
	  }
	  pausa_aleatoria(2.0);
	}
	for (int i = 0; i < 3; i++) {
	  if (libera_asiento(asientos[i]) == -1) {
	    fprintf(stderr, "ERROR: El asiento a liberar está vacío\n\n");
	    pthread_exit((void*) -1);
	  }
	  pausa_aleatoria(2.0);
	}
	return NULL;
}

void* rutina_estado(void* arg) {
	while(1) {
	  estado_sala();
	  printf("\n");
	  usleep(500000);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
	  fprintf(stderr, "Invocación incorrecta: .multihilos n\n");
	  exit(-1);
	}
	crea_sala(250);
	int Nhilos = atoi(argv[1]);
	pthread_t hilos[Nhilos];
	pthread_t hilo_estado;
	estado_sala();
	int ret;
	for (int i = 1; i <= Nhilos; i++) {
	  ret = pthread_create(&hilos[i-1], NULL, rutina, &i);
	  if (ret == -1) {
	    errno = ret;
	    fprintf(stderr,"Error %d pthread_create al crear el hilo%d: %s\n", errno, i, strerror(errno));
	    exit(-1);
	  }
	}
	ret = pthread_create(&hilo_estado, NULL, rutina_estado, NULL);
	if (ret == -1) {
	  errno = ret;
	  fprintf(stderr,"Error %d pthread_create al crear el hilo_estado: %s\n", errno, strerror(errno));
	  exit(-1);
	}
	void* dummy;
	for (int i = 1; i <= Nhilos; i++) {
	  ret = pthread_join(hilos[i-1], &dummy);
	  if (ret == -1) {
	    errno = ret;
	    fprintf(stderr,"Error %d pthread_join sobre hilo%d: %s\n", errno, i, strerror(errno));
	    exit(-1);
	  }
	}
	elimina_sala();
	destroy();
	return(0);
}

