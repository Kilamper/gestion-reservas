#include <stdio.h>
#include <stdlib.h>
#include "sala.h"

// Se crea una nueva sala con la capacidad indicada,
// se reserva el espacio necesario en memoria y
// se inicializa la sala a 0 para indicar que está vacía.
void crea_sala(int capacidad) {
  if (capacidad < 0) {
    printf("Valor de capacidad no válido\n");
    exit(-1);
  }
  capacity = capacidad;
  sala = (int*)malloc(capacity*sizeof(int));
  if (sala == NULL) {
    printf("Error crítico al reservar la memoria\n");
    exit(-1);
  }
  libres = capacidad;
  for (int i = 0; i < capacity; i++) {
    *(sala + i) = 0;
  }
}

// Se libera el espacio de memoria utilizado para la sala.
void elimina_sala() {
  ocupados = 0;
  free(sala);
}

// Se reserva un asiento para una persona con identificador id,
// si la operación resulta exitosa se devuelve el número del
// asiento reservado, si no se devuelve -1.
int reserva_asiento(int id) {
  if (asientos_ocupados() != capacity) {
    for (int i = 0; i < capacity; i++) {
      if (*(sala + i) == 0) {
        *(sala + i) = id;
        ocupados++;
        libres--;
        return(i);
      }
    }
  }
  return(-1);
}

// Se libera el asiento indicado, si la operación resulta
// exitosa se devuelve el id de la persona que lo ocupaba,
// si no se devuelve -1.
int libera_asiento(int asiento) {
  if ((asientos_libres() == capacity) || (asiento >= capacity)) {
    return(-1);
  } else if ((asiento < 0) || (*(sala + asiento) == 0)) {
    return(-1);
  } else {
    int id = *(sala + asiento);
    *(sala + asiento) = 0;
    ocupados--;
    libres++;
    return(id);
  }
}

// Se comprueba el estado de un asiento concreto, si el
// asiento se encuentra dentro de la capacidad se devuelve
// el valor almacenado en su posición, si no se devuelve -1.
int estado_asiento(int asiento) {
  if ((asiento >= capacity) || (asiento < 0)) {
    return(-1);
  } else {
    return(*(sala + asiento));
  }
}

// Se realiza la cuenta de los asientos que están libres.
int asientos_libres() {
  return(libres);
}

// Se realiza la cuenta de los asientos que están ocupados.
int asientos_ocupados() {
  return(ocupados);
}

// Se devuelve la capacidad de la sala.
int capacidad() {
  return(capacity);
}
