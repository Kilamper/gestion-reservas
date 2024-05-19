void crea_sala(int capacidad);
void elimina_sala();
void vacia_sala();
int reserva_asiento(int id);
int libera_asiento(int asiento);
int estado_asiento(int asiento);
void estado_sala();
int guarda_estado_sala(const char* ruta_fichero);
int recupera_estado_sala(const char* ruta_fichero);
int recupera_capacidad(const char* ruta_fichero);
int guarda_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos);
int recupera_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos);
void estado_sala_recuperada();
int asientos_libres();
int asientos_ocupados();
int capacidad();

