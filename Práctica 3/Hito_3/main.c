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

void* rutina_reserva(void* arg) {
	int param = *((int*) arg);
	for (int i = 0; i < 3; i++) {
	  reserva_asiento(param);
	  pausa_aleatoria(2.0);
	}
	return NULL;
}

void* rutina_libera(void* arg) {
	int n = 0;
	int i = 0;
        while (n < 3) {
          if (libera_asiento(i) != -1) {
	    pausa_aleatoria(2.0);
	    n++;
	  }
	  i++;
	  if (i == capacidad()) {
	    i = 0;
	  }
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
	if (argc != 3) {
	  fprintf(stderr, "Invocación incorrecta: ./multihilos n m\n");
	  exit(-1);
	}
	crea_sala(250);
	int Nhilos_reserva = atoi(argv[1]);
	int Nhilos_libera = atoi(argv[2]);
	pthread_t hilos_reserva[Nhilos_reserva];
	pthread_t hilos_libera[Nhilos_libera];
	pthread_t hilo_estado;
	int ret;
	for (int i = 1; i <= Nhilos_reserva; i++) {
	  ret = pthread_create(&hilos_reserva[i-1], NULL, rutina_reserva, &i);
	  if (ret == -1) {
	    errno = ret;
	    fprintf(stderr, "Error %d pthread_create al crear el hilo_reserva_%d: %s\n", errno, i, strerror(errno));
	    exit(-1);
	  }
	}
	for (int j = 1; j <= Nhilos_libera; j++) {
	  ret = pthread_create(&hilos_libera[j-1], NULL, rutina_libera, NULL);
	  if (ret == -1) {
	    errno = ret;
	    fprintf(stderr, "Error %d pthread_create al crear el hilo_libera_%d: %s\n", errno, j, strerror(errno));
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
	for (int i = 1; i <= Nhilos_reserva; i++) {
	  ret = pthread_join(hilos_reserva[i-1], &dummy);
	  if (ret == -1) {
	    errno = ret;
	    fprintf(stderr, "Error %d pthread_join sobre hilo_reserva_%d: %s\n", errno, i, strerror(errno));
	    exit(-1);
	  }
	}
	for (int j = 1; j <= Nhilos_libera; j++) {
	  ret = pthread_join(hilos_libera[j-1], &dummy);
	  if (ret == -1) {
	    errno = ret;
	    fprintf(stderr, "Error %d pthread_join sobre hilo_libera_%d: %s\n", errno, j, strerror(errno));
	  }
	}
	elimina_sala();
	destroy();
	return(0);
}

