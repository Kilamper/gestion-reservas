// test_sala.c
// ===============
// Batería de pruebas de la biblioteca "sala.h/sala.c"
//

#include <assert.h>
#include <stdio.h>
#include "sala.h"
#include "test.h"

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
	printf("\n----------------------Test-Reserva-Simple----------------------\n");
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
	printf("\n---------------------Test-Reserva-Multiple---------------------\n");
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

void test_SalaVacía() {
	printf("\n------------------------Test-Sala-Vacía------------------------\n");
	#define CAPACIDAD_VACIA 20
	crea_sala(CAPACIDAD_VACIA);
	estado_sala();
	levantarse(100);
	libera_multiple(10);
	DebeSerCierto(libera_asiento(3) == -1);
	elimina_sala();
}
void test_SalaLLena() {
	printf("\n------------------------Test-Sala-Llena------------------------\n");
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
	printf("\n---------------------Test-Reserva-Negativa---------------------\n");
	#define CAPACIDAD_NEGATIVA -5
	crea_sala(CAPACIDAD_NEGATIVA);
	crea_sala(CAPACIDAD_INICIAL);
	int IDs[1];
	IDs[0] = 1;
	reserva_multiple(-1, IDs);
	libera_multiple(-1);
	DebeSerCierto(libera_asiento(-1) == -1);
	DebeSerCierto(estado_asiento(-1) == -1);
	elimina_sala();
}

void test_ReservaTeclado() {
	printf("\n----------------------Test-Reserva-Teclado----------------------\n");
	printf("Introduce la capacida de la sala: ");
	scanf("%d", &capacity);
	crea_sala(capacity);
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

void ejecuta_tests () {
	test_ReservaBasica();
	// Añadir nuevos tests
	test_ReservaSimple();
	test_ReservaMultiple();
	test_SalaVacía();
	test_SalaLLena();
	test_ReservaNegativa();
	test_ReservaTeclado();
}

void main()
{
	puts("Iniciando tests...");
	
	ejecuta_tests();
	
	puts("Batería de test completa.");
}

