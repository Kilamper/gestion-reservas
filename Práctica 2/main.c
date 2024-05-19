// test_sala.c
// ===============
// Batería de pruebas de la biblioteca "sala.h/sala.c"
//

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sala.h"
#include "test.h"

int ejecuta_crea(char* ruta, int flag_o) {
	int existe = access(ruta, F_OK);
	if (((existe != -1) && flag_o) || (existe == -1)) {
	  if (guarda_estado_sala(ruta) == -1) {
	    return(-1);
	  }
	} else if ((existe != -1) && !flag_o) {
	  fprintf(stderr, "Archivo existente: ./misala --crea -f ruta -o -c capacidad\n");
	  return(-1);
	}
	elimina_sala();
	return(0);
}

int ejecuta_reserva(int argc, char *argv[], char* ruta, int num_asientos) {
	if (num_asientos <= 0) {
	  fprintf(stderr, "El número de asientos debe ser un entero mayor que 0\n");
	  return(-1);
	} else if (num_asientos != argc-6) {
	  fprintf(stderr, "La cantidad de identificadores debe coincidir con el número de asientos\n");
	  return(-1);
	}
	int asientos[num_asientos];
	for (int n = 0; n < num_asientos; n++) {
	  int asiento = reserva_asiento(atoi(argv[6+n]));
	  if (asiento != -1) {
	    asientos[n] = asiento;
	  }
	}
	guarda_estadoparcial_sala(ruta, num_asientos, asientos);
	elimina_sala();
	return(0);
}

int ejecuta_anula(int argc, char *argv[], char* ruta, int flag_a, int flag_i) {
	int num_asientos = argc-5;
	int asientos[num_asientos];
	for (int i = 5; i < argc; i++) {
	  if (flag_a) {
	    if (libera_asiento(atoi(argv[i])) != -1) {
	      asientos[i-5] = atoi(argv[i]);
	    }
	  } else if (flag_i) {
	    levantarse(atoi(argv[i]));
	  }
	}
	if (flag_a) {
	  guarda_estadoparcial_sala(ruta, num_asientos, asientos);
	} else if (flag_i) {
	  guarda_estado_sala(ruta);
	}
	elimina_sala();
	return(0);
}

int ejecuta_compara(char *argv[]) {
	int capacidad_sala1 = recupera_capacidad(argv[2]);
	if (capacidad_sala1 == -1) {
	  puts("-1");
	  return(-1);
	}
	crea_sala(capacidad_sala1);
	int sala[capacidad_sala1];
	if (recupera_estado_sala(argv[2]) == -1) {
	  puts("-1");
	  return(-1);
	}
	for (int i = 0; i < capacidad_sala1; i++) {
	  sala[i] = estado_asiento(i);
	}
	elimina_sala();
	int capacidad_sala2 = recupera_capacidad(argv[3]);
	if (capacidad_sala2 == -1) {
	  puts("-1");
	  return(-1);
	}
	crea_sala(capacidad_sala2);
	if (recupera_estado_sala(argv[3]) == -1) {
	  puts("-1");
	  return(-1);
	} else if (capacidad_sala1 != capacidad_sala2) {
	  puts("1");
	  return(1);
	}
	for (int i = 0; i < capacidad(); i++) {
	  if (sala[i] != estado_asiento(i)) {
		puts("1");
		return(1);
	  }
	}
	elimina_sala();
	puts("0");
	return(0);		
}

void ejecuta_test(int test) {
	switch(test) {
	  case 0:
	    test_ReservaBasica();
	    break;
	  case 1:
	    test_ReservaSimple();
	    break;
	  case 2:
	    test_ReservaMultiple();
	    break;
	  case 3:
	    test_SalaVacia();
	    break;
	  case 4:
	    test_SalaLLena();
	    break;
	  case 5:
	    test_ReservaNegativa();
	    break;
	  case 6:
	    test_ReservaTeclado();
	    break;
	  case 7:
	    test_GuardadoCompleto();  
	    break;
	  case 8:
	    test_GuardadoParcial();
	    break;
	}
}

int main(int argc, char *argv[]) {
	int opt;
	int long_flag;
	char* ruta;
	int flag_f = 0;
	int flag_o = 0;
	int flag_n = 0;
	int flag_a = 0;
	int flag_i = 0;
	int num_asientos;
	int option_index;
	static struct option long_options[] = {
	  {"crea", no_argument, 0, 1},
	  {"reserva", no_argument, 0, 2},
	  {"anula", no_argument, 0, 3},
	  {"estado", no_argument, 0, 4},
	  {"compara", no_argument, 0, 5},
	  {"test", required_argument, 0, 't'},
	  {NULL, 0, 0, 0}
	};
	while ((opt = getopt_long(argc, argv, "c:f:on:ait:", long_options, &option_index)) != -1) {
	  switch(opt) {
	    case 1:
	      long_flag = opt;
	      if (argc != 6 && argc != 7) {
	        fprintf(stderr, "Invocación incorrecta: ./misala --crea -f ruta [-o] -c capacidad\n");
	        exit(-1);
	      }
	      break;
	    case 2:
	      long_flag = opt;
	      if (argc < 6) {
	        fprintf(stderr, "Invocación incorrecta: ./misala --reserva -f ruta -n id_persona1...\n");
	        exit(-1);
	      }
	      break;
	    case 3: 
	      long_flag = opt;
	      if (argc < 5) {
	        fprintf(stderr, "Invocación incorrecta: ./misala --anula -f ruta -a/-i id_asiento1/id_persona1...\n");
	        exit(-1);
	      }
	      break;
	    case 4:
	      long_flag = opt;
	      if (argc != 4) {
	        fprintf(stderr, "Invocación incorrecta: ./misala --estado -f ruta\n");
	        exit(-1);
	      }
	      break;
	    case 5:
	      long_flag = opt;
	      if (argc != 4) {
	        fprintf(stderr, "Invocación incorrecta: ./misala --compara ruta1 ruta2\n");
	        exit(-1);
	      }
	      break;
	    case 'f':
	      flag_f = 1;
	      ruta = optarg;
	      break;
	    case 'o':
	      flag_o = 1;
	      break;
	    case 'c':
	      int capacidad = atoi(optarg);
	      crea_sala(capacidad);
	      break;
	    case 'n':
	      flag_n = 1;
	      num_asientos = atoi(optarg);
	      break;
	    case 'a': 
	      flag_a = 1;
	      break;
	    case 'i':
	      flag_i = 1;
	      break;
	    case 't':
	      if (argc != 3) {
	        fprintf(stderr, "Invocación incorrecta: ./misala --test id_prueba\n");
	        exit(-1);
	      }
	      ejecuta_test(atoi(optarg));
	      break;
	    case ':':
	      printf("La opción necesita un valor\n");
	      break;
	    case '?':
	      fprintf(stderr, "Modificador no válido\n");
	      break;
	  }
	}
	if (long_flag == 1 && flag_f) {
	  ejecuta_crea(ruta, flag_o);
	  flag_f = 0;
	  flag_o = 0;
	}
	if ((long_flag >= 2 && long_flag <= 4) && flag_f) {
	  int capacidad_recuperada = recupera_capacidad(ruta);
	  if (capacidad_recuperada == -1) {
	    return(-1);
	  }
	  crea_sala(capacidad_recuperada);
	  if (recupera_estado_sala(ruta) == -1) {
	    return(-1);
	  }
	  if (long_flag == 2 && flag_n) {
	    ejecuta_reserva(argc, argv, ruta, num_asientos);
	    flag_n = 0;
	  } else if (long_flag == 3 && (flag_a || flag_i)) {
	    ejecuta_anula(argc, argv, ruta, flag_a, flag_i);
	    flag_a = 0;
	    flag_i = 0;
	  } else if (long_flag == 4) {
	    estado_sala();
	    elimina_sala();
	  }
	} else if (long_flag == 5) {
	    ejecuta_compara(argv);
	}
	return(0);
}

