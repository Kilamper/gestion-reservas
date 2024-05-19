// sala.c
// ===============
// 

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "sala.h"
#include "retardo.h"

int capacity;
int *sala;
int libres;
int ocupados;
int hombres;
int mujeres;
pthread_mutex_t cerrojo = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_reserva = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_libera = PTHREAD_COND_INITIALIZER;

// Se crea una nueva sala con la capacidad indicada,
// se reserva el espacio necesario en memoria y
// se inicializa la sala a 0 para indicar que está vacía.
void crea_sala(int capacidad) {
  if (capacidad < 0) {
    fprintf(stderr, "Valor de capacidad no válido\n");
    exit(-1);
  }
  capacity = capacidad;
  sala = (int*)malloc(capacity*sizeof(int));
  if (sala == NULL) {
    fprintf(stderr, "Error crítico al reservar la memoria\n");
    exit(-1);
  }
  libres = capacidad;
  ocupados = 0;
  for (int i = 0; i < capacity; i++) {
    *(sala + i) = 0;
  }
}

// Se libera el espacio de memoria utilizado para la sala.
void elimina_sala() {
  if (pthread_mutex_lock(&cerrojo) == -1) {
    exit(-1);
  }
  ocupados = 0;
  libres = capacity;
  free(sala);
  if (pthread_mutex_unlock(&cerrojo) == -1) {
    exit(-1);
  }
}

// Se reserva un asiento para una persona con identificador id,
// si la operación resulta exitosa se devuelve el número del
// asiento reservado, si no se devuelve -1.
int reserva_asiento(int id) {
  if (id != 0) {
    if (pthread_mutex_lock(&cerrojo) == -1) {
      exit(-1);
    }
    int ratio;
    if (id == 1) {
      ratio = hombres+1 > (ocupados+1)*0.6;
    } else if (id == 2) {
      ratio = mujeres+1 > (ocupados+1)*0.6;
    }
    while (libres == 0 || (ocupados+1 >= 10 && ratio)) {
      if (pthread_cond_wait(&cond_reserva, &cerrojo) == -1) {
        exit(-1);
      }
    }
    for (int i = 0; i < capacity; i++) {
      if (*(sala + i) == 0) {
        *(sala + i) = id;
        ocupados++;
        libres--;
        if (id == 1) hombres++;
        if (id == 2) mujeres++;
        if (pthread_cond_signal(&cond_libera) == -1) {
          exit(-1);
        }
        if (pthread_mutex_unlock(&cerrojo) == -1) {
          exit(-1);
        }
        return(i);
      }
    }
    if (pthread_mutex_unlock(&cerrojo) == -1) {
      exit(-1);
    }
  }
  return(-1);
}

// Se libera el asiento indicado, si la operación resulta
// exitosa se devuelve el id de la persona que lo ocupaba,
// si no se devuelve -1.
int libera_asiento(int asiento) {
  if ((asiento < 0) || (asiento >= capacity)) {
    return(-1);
  }
  if (pthread_mutex_lock(&cerrojo) == -1) {
    exit(-1);
  }
  if (*(sala + asiento) == 0) {
    if (pthread_mutex_unlock(&cerrojo) == -1) {
      exit(-1);
    }
    return(-1);
  }
  while (ocupados == 0) {
    if (pthread_cond_wait(&cond_libera, &cerrojo) == -1) {
      exit(-1);
    }
  }
  int id = *(sala + asiento);
  *(sala + asiento) = 0;
  ocupados--;
  libres++;
  if (pthread_cond_signal(&cond_reserva) == -1) {
    exit(-1);
  }
  if (pthread_mutex_unlock(&cerrojo) == -1) {
    exit(-1);
  }
  return(id);
}

// Se comprueba el estado de un asiento concreto, si el
// asiento se encuentra dentro de la capacidad se devuelve
// el valor almacenado en su posición, si no se devuelve -1.
int estado_asiento(int asiento) {
  if ((asiento < 0) || (asiento >= capacity)) {
    return(-1);
  } else {
    if (pthread_mutex_lock(&cerrojo) == -1) {
      exit(-1);
    }
    int estado = *(sala + asiento);
    if (pthread_mutex_unlock(&cerrojo) == -1) {
      exit(-1);
    }
    return(estado);
  }
}

// Se comprueba el estado de la sala imprimiendo los valores
// almacenados en la posición de cada asiento.
void estado_sala() {
  if (pthread_mutex_lock(&cerrojo) == -1) {
    exit(-1);
  }
  printf("·Estado de la sala:\n");
  for (int i = 0; i < capacity; i++) {
    printf("%d  ", *(sala + i));
  }
  printf("\n");
  if (pthread_mutex_unlock(&cerrojo) == -1) {
    exit(-1);
  }
}

// Se devuelve la cuenta de los asientos que están libres.
int asientos_libres() {
  if (pthread_mutex_lock(&cerrojo) == -1) {
    exit(-1);
  }
  int ret = libres;
  if (pthread_mutex_unlock(&cerrojo) == -1) {
    exit(-1);
  }
  return(ret);
}

// Se devuelve la cuenta de los asientos que están ocupados.
int asientos_ocupados() {
  if (pthread_mutex_lock(&cerrojo) == -1) {
    exit(-1);
  }
  int ret = ocupados;
  if (pthread_mutex_unlock(&cerrojo) == -1) {
    exit(-1);
  }
  return(ret);
}

// Se devuelve la capacidad de la sala.
int capacidad() {
  return(capacity);
}

// Se vacía la sala por completo.
void vacia_sala() {
  if (pthread_mutex_lock(&cerrojo) == -1) {
    exit(-1);
  }
  ocupados = 0;
  libres = capacity;
  for (int i = 0; i < capacity; i++) {
    *(sala + i) = 0;
  }
  if (pthread_mutex_unlock(&cerrojo) == -1) {
    exit(-1);
  }
}

unsigned long tamaño_bloque(const char* ruta_fichero) {
  struct stat buf;
  if (stat(ruta_fichero, &buf) == -1) {
    return(-1);
  }
  return(buf.st_blksize);
}

// Se guarda el estado actual de la sala en el fichero que se
// pasa como argumento, se devuelve 0 si todo ha salido bien
// o -1 en caso de error.
int guarda_estado_sala(const char* ruta_fichero) {
  int fd = open(ruta_fichero, O_RDWR|O_CREAT|O_APPEND, S_IRWXU);
  if (fd == -1) {
    fprintf(stderr, "Error al abrir el archivo para guardar el estado\n");
    return(-1);
  }
  unsigned long block = tamaño_bloque(ruta_fichero);
  if (block == -1) {
    close(fd);
    return(-1);
  }
  if (pthread_mutex_lock(&cerrojo) == -1) {
    close(fd);
    exit(-1);
  }
  ftruncate(fd, 0);
  if (write(fd, &capacity, sizeof(int)) == -1) {
    fprintf(stderr, "Error al guardar la capacidad de la sala\n");
    if (pthread_mutex_unlock(&cerrojo) == -1) {
      close(fd);
      exit(-1);
    }
    return(-1);
  }
  ftruncate(fd, sizeof(int));
  for (int i = 0; i < capacity*sizeof(int); i += block) {
    int escritos = write(fd, sala + i/sizeof(int), block);
    if (escritos == -1) {
      fprintf(stderr, "Error al escribir en el archivo\n");
      if (pthread_mutex_unlock(&cerrojo) == -1) {
        close(fd);
        exit(-1);
      }
      close(fd);
      return(-1);
    } else if (escritos == 0) {
      break;
    }
  }
  if (pthread_mutex_unlock(&cerrojo) == -1) {
    close(fd);
    exit(-1);
  }
  return(close(fd));
}

// Se recupera el estado de la sala a partir de la información
// guardada en el fichero que se pasa como argumento, se devuelve
// 0 si todo ha ido bien o -1 en caso de error.
int recupera_estado_sala(const char* ruta_fichero) {
  int fd = open(ruta_fichero, O_RDWR|O_APPEND, S_IRWXU);
  if (fd == -1) {
    fprintf(stderr, "Error al abrir el archivo para recuperar el estado\n");
    return(-1);
  }
  unsigned long block = tamaño_bloque(ruta_fichero);
  if (block == -1) {
    close(fd);
    return(-1);
  }
  int stored_capacity = recupera_capacidad(ruta_fichero);
  if (stored_capacity == -1) {
    close(fd);
    return(-1);
  }
  if (capacity < stored_capacity) {
    fprintf(stderr, "La capacidad de la sala a sobreescribir no es suficiente\n");
    close(fd);
    return(-1);
  }
  if (pthread_mutex_lock(&cerrojo) == -1) {
    close(fd);
    exit(-1);
  }
  lseek(fd, sizeof(int), SEEK_SET);
  for (int i = 0; i < stored_capacity*sizeof(int); i += block) {
    int leidos = read(fd, sala + i/sizeof(int), block);
    if (leidos == -1) {
      fprintf(stderr, "Error al leer el archivo\n");
      if (pthread_mutex_unlock(&cerrojo) == -1) {
        close(fd);
        exit(-1);
      }
      close(fd);
      return(-1);
    } else if (leidos == 0) {
      break;
    }
  }
  if (pthread_mutex_unlock(&cerrojo) == -1) {
    close(fd);
    exit(-1);
  }
  estado_sala_recuperada();
  return(close(fd));
}

// Se recupera la capacidad de la sala almacenada en un fichero preexistente
int recupera_capacidad(const char* ruta_fichero) {
  int fd = open(ruta_fichero, O_RDWR|O_APPEND, S_IRWXU);
  if (fd == -1) {
    fprintf(stderr, "Error al abrir el archivo para recuperar la capacidad\n");
    return(-1);
  }
  if (pthread_mutex_lock(&cerrojo) == -1) {
    close(fd);
    exit(-1);
  }
  lseek(fd, 0, SEEK_SET);
  int stored_capacity;
  if (read(fd, &stored_capacity, sizeof(int)) == -1) {
    fprintf(stderr, "Error al recuperar la capacidad de la sala\n");
    if (pthread_mutex_unlock(&cerrojo) == -1) {
      close(fd);
      exit(-1);
    }
    close(fd);
    return(-1);
  }
  if (pthread_mutex_unlock(&cerrojo) == -1) {
    close(fd);
    exit(-1);
  }
  if (close(fd) == -1) {
    return(-1);
  }
  return(stored_capacity);
}

// Se guarda el estado de un conjunto de asientos de la sala en un fichero
// preexistente que se pasa como argumento, se devuelve 0 si todo ha ido bien
// o -1 en caso de error.
int guarda_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
  int fd = open(ruta_fichero, O_RDWR, S_IRWXU);
  if (fd == -1) {
    fprintf(stderr, "Error al abrir el archivo para guardar el estado\n");
    return(-1);
  }
  int stored_capacity = recupera_capacidad(ruta_fichero);
  if (stored_capacity == -1) {
    close(fd);
    return(-1);
  }
  if (capacity > stored_capacity) {
    fprintf(stderr, "La capacidad de la sala a sobreescribir no es suficiente\n");
    close(fd);
    return(-1);
  }
  for (int i = 0; i < num_asientos; i++) {
    int asiento = *(id_asientos + i);
    if ((asiento < 0) || (asiento > capacity) || (asiento > stored_capacity)) {
      close(fd);
      return(-1);
    }
  }
  if (pthread_mutex_lock(&cerrojo) == -1) {
    close(fd);
    exit(-1);
  }
  for (int i = 0; i < num_asientos; i++) {
    lseek(fd, sizeof(int) + *(id_asientos + i)*sizeof(int), SEEK_SET);
    int escritos = write(fd, sala + *(id_asientos + i), sizeof(int));
    if (escritos == -1) {
      fprintf(stderr, "Error al escribir en el archivo\n");
      if (pthread_mutex_unlock(&cerrojo) == -1) {
        close(fd);
        exit(-1);
      }
      close(fd);
      return(-1);
    }
  }
  if (pthread_mutex_unlock(&cerrojo) == -1) {
    close(fd);
    exit(-1);
  }
  return(close(fd));
}

// Se recupera el estado de un conjunto de asientos de la sala a partir de la
// información guardada en un fichero preexistente que se pasa como argumento,
// se devuelve 0 si todo ha ido bien o -1 en caso de error.
int recupera_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
  int fd = open(ruta_fichero, O_RDWR|O_APPEND, S_IRWXU);
  if (fd == -1) {
    fprintf(stderr, "Error al abrir el archivo para recuperar el estado\n");
    return(-1);
  }
  int stored_capacity = recupera_capacidad(ruta_fichero);
  if (stored_capacity == -1) {
    close(fd);
    return(-1);
  }
  if (capacity < stored_capacity) {
    fprintf(stderr, "La capacidad de la sala a sobreescribir no es suficiente\n");
    close(fd);
    return(-1);
  }
  if (pthread_mutex_lock(&cerrojo) == -1) {
    close(fd);
    exit(-1);
  }
  for (int i = 0; i < num_asientos; i++) {
    lseek(fd, sizeof(int) + *(id_asientos + i)*sizeof(int), SEEK_SET);
    int leidos = read(fd, sala + *(id_asientos + i), sizeof(int));
    if (leidos == -1) {
      fprintf(stderr, "Error al leer el archivo\n");
      if (pthread_mutex_unlock(&cerrojo) == -1) {
        close(fd);
        exit(-1);
      }
      close(fd);
      return(-1);
    }
  }
  if (pthread_mutex_unlock(&cerrojo) == -1) {
    close(fd);
    exit(-1);
  }
  estado_sala_recuperada();
  return(close(fd));
}

// Se asigna el estado de ocupación de la sala recuperada a la nueva sala.
void estado_sala_recuperada() {
  if (pthread_mutex_lock(&cerrojo) == -1) {
    exit(-1);
  }
  ocupados = 0;
  libres = capacity;
  for (int i = 0; i < capacity; i++) {
    if (*(sala + i) != 0) {
      ocupados++;
      libres--;
    }
  }
  if (pthread_mutex_unlock(&cerrojo) == -1) {
    exit(-1);
  }
}

// Se destruyen tanto el cerrojo como las variables condición. 
void destroy() {
  if (pthread_mutex_destroy(&cerrojo)) {
    exit(-1);
  }
  if (pthread_cond_destroy(&cond_reserva)) {
    exit(-1);
  }
  if (pthread_cond_destroy(&cond_libera)) {
    exit(-1);
  }
}
