// main.c
// ===============
// 

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "sala.h"
#include "retardo.h"

void* rutina_hombres(void* arg) {
	int asiento = reserva_asiento(1);
	pausa_aleatoria(2.0);
	libera_asiento(asiento);
	return NULL;
}

void* rutina_mujeres(void* arg) {
	int asiento = reserva_asiento(2);
	pausa_aleatoria(2.0);
	libera_asiento(asiento);
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
	if (argc != 3) {
	  fprintf(stderr, "Invocación incorrecta: ./multihilos nhombres nmujeres\n");
	  exit(-1);
	}
	crea_sala(50);
	int nhombres = atoi(argv[1]);
	int nmujeres = atoi(argv[2]);
	pthread_t hilos_hombres[nhombres];
	pthread_t hilos_mujeres[nmujeres];
	pthread_t hilo_estado;
	int ret;
	for (int i = 1; i <= nhombres; i++) {
	  ret = pthread_create(&hilos_hombres[i-1], NULL, rutina_hombres, NULL);
	  if (ret == -1) {
	    errno = ret;
	    fprintf(stderr, "Error %d pthread_create al crear el hilo_hombres_%d: %s\n", errno, i, strerror(errno));
	    exit(-1);
	  }
	}
	for (int j = 1; j <= nmujeres; j++) {
	  ret = pthread_create(&hilos_mujeres[j-1], NULL, rutina_mujeres, NULL);
	  if (ret == -1) {
	    errno = ret;
	    fprintf(stderr, "Error %d pthread_create al crear el hilo_mujeres_%d: %s\n", errno, j, strerror(errno));
	    exit(-1);
	  }
	}
	ret = pthread_create(&hilo_estado, NULL, rutina_estado, NULL);
	if (ret == -1) {
	  errno = ret;
	  fprintf(stderr, "Error %d pthread_create al crear el hilo_estado: %s\n", errno, strerror(errno));
	  exit(-1);
	}
	void* dummy;
	for (int i = 1; i <= nhombres; i++) {
	  ret = pthread_join(hilos_hombres[i-1], &dummy);
	  if (ret == -1) {
	    errno = ret;
	    fprintf(stderr, "Error %d pthread_join sobre hilo_hombres_%d: %s\n", errno, i, strerror(errno));
	    exit(-1);
	  }
	}
	for (int j = 1; j <= nmujeres; j++) {
	  ret = pthread_join(hilos_mujeres[j-1], &dummy);
	  if (ret == -1) {
	    errno = ret;
	    fprintf(stderr, "Error %d pthread_join sobre hilo_mujeres_%d: %s\n", errno, j, strerror(errno));
	  }
	}
	elimina_sala();
	destroy();
	return(0);
}

