#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "test.h"
#include "sala.h"

#define DebeSerCierto(x)	assert(x)
#define DebeSerFalso(x)		assert(!(x))

void INICIO_TEST (const char* titulo_test)
{
  printf("********** batería de pruebas para %s: ", titulo_test); 
 	// fflush fuerza que se imprima el mensaje anterior
	// sin necesidad de utilizar un salto de línea
	fflush(stdout);
}

void FIN_TEST (const char* titulo_test)
{
  printf ("********** hecho\n");
}


void test_ReservaBasica() {
	int mi_asiento;
	#define CAPACIDAD_CUYAS 500
	#define ID_1 1500
	INICIO_TEST("Reserva básica");
	crea_sala(CAPACIDAD_CUYAS);
	DebeSerCierto(capacidad()==CAPACIDAD_CUYAS);
	DebeSerCierto((mi_asiento=reserva_asiento(ID_1))>=0);
	DebeSerCierto(asientos_libres()+asientos_ocupados()==CAPACIDAD_CUYAS);
	DebeSerCierto(estado_asiento(mi_asiento)>0);
	DebeSerCierto(libera_asiento(mi_asiento)==ID_1);
	DebeSerCierto(asientos_libres()+asientos_ocupados()==CAPACIDAD_CUYAS);
        elimina_sala();
	FIN_TEST("Reserva básica");
}

void test_ReservaSimple() {
	printf("----------------------Test-Nº1-Reserva-Simple----------------------\n");
	#define CAPACIDAD_INICIAL 20
	#define ID_2 1200
	#define ID_3 1300
	crea_sala(CAPACIDAD_INICIAL);
	sentarse(ID_2);
	estado_sala();
	levantarse(ID_3);
	levantarse(ID_2);
	elimina_sala();
}

void test_ReservaMultiple() {
	printf("---------------------Test-Nº2-Reserva-Multiple---------------------\n");
	#define NUEVA_CAPACIDAD 30
	#define ID_4 200
	#define N 20
	crea_sala(NUEVA_CAPACIDAD);
	int IDs[N];
	int i;
	for (i = 0; i < N; i++) {
	  IDs[i] = ID_4 + i;
	}
	reserva_multiple(N, IDs);
	estado_sala();
	reserva_multiple(N, IDs);
	libera_multiple(N);
	elimina_sala();
}

void test_SalaVacia() {
	printf("------------------------Test-Nº3-Sala-Vacía------------------------\n");
	#define CAPACIDAD_VACIA 20
	crea_sala(CAPACIDAD_VACIA);
	estado_sala();
	levantarse(100);
	libera_multiple(10);
	DebeSerCierto(libera_asiento(3) == -1);
	elimina_sala();
}
void test_SalaLLena() {
	printf("------------------------Test-Nº4-Sala-Llena------------------------\n");
	#define CAPACIDAD_LLENA 10
	crea_sala(CAPACIDAD_LLENA);
	int IDs[CAPACIDAD_LLENA];
	for (int i = 1; i <= CAPACIDAD_LLENA; i++) {
	  IDs[i-1] = i;
	  reserva_asiento(i);
	}
	estado_sala();
	sentarse(120);
	reserva_multiple(10, IDs);
	DebeSerCierto(reserva_asiento(100) == -1);
	elimina_sala();
}

void test_ReservaNegativa() {
	printf("---------------------Test-Nº5-Reserva-Negativa---------------------\n");
	#define CAPACIDAD_NEGATIVA -5
	crea_sala(CAPACIDAD_INICIAL);
	int IDs[1];
	IDs[0] = 1;
	reserva_multiple(-1, IDs);
	libera_multiple(-1);
	DebeSerCierto(libera_asiento(-1) == -1);
	DebeSerCierto(estado_asiento(-1) == -1);
	elimina_sala();
	crea_sala(CAPACIDAD_NEGATIVA);
}

void test_ReservaTeclado() {
	printf("----------------------Test-Nº6-Reserva-Teclado----------------------\n");
	printf("Introduce la capacida de la sala: ");
	int capacidad;
	scanf("%d", &capacidad);
	crea_sala(capacidad);
	int id;
	printf("Reserva asiento con id: ");
	scanf("%d", &id);
	sentarse(id);
	estado_sala();
	int asiento;
	printf("Libera el asiento número: ");
	scanf("%d", &asiento);
	printf("libera_asiento da como resultado: %d\n", libera_asiento(asiento));
	printf("Visualiza el estado del asiento: ");
	scanf("%d", &asiento);
	printf("estado_asiento da como resultado: %d\n", estado_asiento(asiento));
	elimina_sala();
}

void test_GuardadoCompleto() {
	printf("---------------------Test-Nº7-Guardado-Completo---------------------\n");
	crea_sala(100);
	for (int i = 1; i <= 80; i++) {
	  reserva_asiento(i);
	}
	guarda_estado_sala("Estado_Sala.txt");	
	elimina_sala();
	crea_sala(100);
	recupera_estado_sala("Estado_Sala.txt");
	estado_sala();
	elimina_sala();
}

void test_GuardadoParcial() {
	printf("---------------------Test-Nº8-Guardado-Parcial----------------------\n");
	crea_sala(100);
	for (int i = 80; i >= 1; i--) {
	  reserva_asiento(i);
	}
	int asientos[5];
	for (int i = 0; i < 5; i++) {
	  asientos[i] = 2*i;
	}
	guarda_estadoparcial_sala("Estado_Sala.txt", 5, asientos);
	elimina_sala();
	crea_sala(25);
	recupera_estadoparcial_sala("Estado_Sala.txt", 5, asientos);
	elimina_sala();
	crea_sala(100);
	recupera_estadoparcial_sala("Estado_Sala.txt", 5, asientos);
	estado_sala();
	elimina_sala();
}

// Se comprueba el estado de la sala imprimiendo los valores
// almacenados en la posición de cada asiento, y se imprimen
// tanto el aforo máximo como la ocupación actual.
void estado_sala() {
  printf("·Estado de a sala:\n");
  for (int i = 0; i < capacidad(); i++) {
    printf("%d  ", estado_asiento(i));
  }
  printf("\n·Aforo: %d\n", capacidad());
  printf("·Ocupación actual: %d\n", asientos_ocupados());
  printf("·Asientos disponibles: %d\n", asientos_libres());
}

// Se comprueba de manera rigurosa el resultado de la
// operación reserva_asiento() imprimiendo mensajes para
// todos los posibles casos.
void sentarse(int id) {
  int result = reserva_asiento(id);
  if (result == -1) {
    printf("- No se ha podido reservar ningún asiento, debido a que el aforo está al máximo\n");
  }
}

// Se liberan todos los asientos reservados por la persona
// con el id propuesto, y se comprueba de manera rigurosa el
// resultado de la operación libera_asiento() imprimiendo 
// mensajes para cada resultado posible.
void levantarse(int id) {
  if (asientos_ocupados() == 0) {
    printf("- No se ha podido liberar ningún asiento, debido a que la sala está vacía\n");
  } else {
    int result = 0;
    for(int i = 0; i < capacidad(); i++) {
      if (estado_asiento(i) == id) {
        result = libera_asiento(i);
      }
    }
    if (result == 0) {
      printf("- No se ha encontrado ningún asiento ocupado por la persona con id: %d\n", id);
    }
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

