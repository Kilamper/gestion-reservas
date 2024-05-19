#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "sala.h"

// Se comprueba el estado de la sala imprimiendo los valores
// almacenados en la posición de cada asiento, y se imprimen
// tanto el aforo máximo como la ocupación actual.
void estado_sala() {
  printf("·Estado de la sala:\n");
  for (int i = 0; i < capacity; i++) {
    printf("%d	", estado_asiento(i));
  }
  printf("\n·Aforo: %d\n", capacidad());
  printf("·Ocupación actual: %d\n", asientos_ocupados());
  printf("·Asientos desocupados: %d\n", asientos_libres());
}

// Se comprueba de manera rigurosa el resultado de la
// operación reserva_asiento() imprimiendo mensajes para
// todos los posibles casos.
void sentarse(int id) {
  int result = reserva_asiento(id);
  if (result == -1) {
    printf("- No se ha podido reservar ningún asiento, debido a que el aforo está al máximo\n");
  } else {
    printf("+ Se ha reservado el asiento número %d\n", result);
  }
}

// Se libera un asiento buscando el que esté por la persona
// con el id propuesto, y se comprueba de manera rigurosa el
// resultado de la operación libera_asiento() imprimiendo 
// mensajes para cada resultado posible.
void levantarse(int id) {
  if (asientos_ocupados() == 0) {
    printf("- No se ha podido liberar ningún asiento, debido a que la sala está vacía\n");
  } else {
    int result;
    for(int i = 0; i < capacity; i++) {
      if (estado_asiento(i) == id) {
        result = libera_asiento(i);
        printf("+ Se ha liberado el asiento %d ocupado por la persona con id: %d\n", i, result);
        return;
      }
    }
    printf("- No se ha encontrado ningún asiento ocupado por la persona con id: %d\n", id);
  }
}

// Se reservan múltiples asientos para N personas a la vez sacando
// sus ids de una lista pasada por parámetro, y se imprimen
// mensajes indicando el resultado obtenido de la operación.
void reserva_multiple(int npersonas, int Lista_id[]) {
  if (npersonas < 0) {
    printf("- No se pueden reservar cantidades negativas de asientos\n");
  } else if (asientos_ocupados() == capacidad()) {
    printf("- No se pueden reservar asientos, ya que la sala está llena\n");
  } else if (npersonas > asientos_libres()) {
    printf("- No hay suficientes asientos libres para reservar %d asientos\n", npersonas);
  } else {
    printf("+ Se han reservado los asientos:\n");
    for (int i = 0; i < npersonas; i++) {
      printf("%d ", reserva_asiento(Lista_id[i]));
    }
    printf("\n");
  }
}

// Se liberan N asientos de una vez, y se imprimen mensajes
// indicando el resultado de la operación en cada caso posible.
void libera_multiple(int nasientos) {
  if (nasientos < 0) {
    printf("- No se pueden liberar cantidades negativas de asientos\n");
  } else if (asientos_libres() == capacidad()) {
    printf("- No se pueden liberar asientos, ya que la sala está vacía\n");
  } else if (nasientos > asientos_ocupados()) {
    printf("- No hay suficientes asientos ocupados para liberar %d asientos\n", nasientos);
  } else {
    printf("+ Se han liberado los asientos ocupados por las personas con los siguientes ids:\n");
    for (int i = 0; i < nasientos; i++) {
      printf("%d ", libera_asiento(i));
    }
    printf("\n");
  }
}
