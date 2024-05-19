// Utilidad para generar pausas aleatorias
// Se pueden utilizar en las prácticas de programación concurrente

#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "retardo.h"

static struct timespec calcula_pausa_aleatoria (float max_segundos) {
  const long MILLARDO = 1000000000L;
  assert (RAND_MAX >= MILLARDO);

  struct timespec pausa;
  if (max_segundos < 1.0) 
  {
    pausa.tv_sec = 0;
    long tope = (long)(max_segundos*MILLARDO);
    pausa.tv_nsec = rand() % tope;
  } 
  else 
  {
    long segundos = (long)max_segundos;
    long nanosegundos = (long)((max_segundos-segundos)*MILLARDO);
    do {
      pausa.tv_sec = rand() % (segundos+1);
      pausa.tv_nsec = rand() % MILLARDO;      
    } while (pausa.tv_sec==segundos && pausa.tv_nsec>nanosegundos);
  }
  return pausa;
}

static void ejecuta_pausa(struct timespec* pausa) {
  struct timespec restante = { .tv_sec=0, .tv_nsec=0 };
  for(;;) {
    nanosleep(pausa,&restante);
    if (restante.tv_sec>0 || restante.tv_nsec>0) {
      *pausa = restante;
    }
    else break;
  }  
}

// Genera una pausa aleatoria entre 0 y max_segundos
void pausa_aleatoria (float max_segundos) {
  assert (max_segundos>0);
  struct timespec pausa = calcula_pausa_aleatoria(max_segundos);
  ejecuta_pausa(&pausa);
}

