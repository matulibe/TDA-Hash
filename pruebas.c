#include "hash.h"
#include "hash_iterador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR -1
#define EXITO 0
#define VACIO 0

#define ROJO "\x1b[1;31m"
#define RESET "\x1b[0m"
#define VERDE "\x1b[1;32m"

//strdup no lo podemos usar porque es POSIX pero no es C99
char *duplicar_string(const char *s){
    if (!s)
        return NULL;

    char *p = malloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

void destruir_string(void *elemento){
    if (elemento){
        printf("(Destructor) Libero el vehiculo: %s\n", (char *)elemento);
        free(elemento);
    }
}

bool mostrar_patente(hash_t *hash, const char *clave, void *aux){
    if (!clave)
        return true;

    aux = aux;
    hash = hash;

    printf("Patente en el hash: %s\n", clave);

    return false;
}

void guardar_vehiculo(hash_t *garage, const char *patente, const char *descripcion){
    int retorno = hash_insertar(garage, patente, duplicar_string(descripcion));
    printf("Guardando vehiculo patente %s (%s): ", patente, descripcion);
    printf("%s\n", retorno == 0 ? VERDE"EXITO"RESET : ROJO"FALLO"RESET);
}

void quitar_vehiculo(hash_t *garage, const char *patente){
    int retorno = hash_quitar(garage, patente);
    printf("Retirando vehiculo patente %s: ", patente);
    printf("%s\n", retorno == 0 ? VERDE"EXITO"RESET : ROJO"FALLO"RESET);
}

void verificar_vehiculo(hash_t *garage, const char *patente, bool deberia_existir){
    printf("Verifico el vehiculo patente %s: ", patente);
    bool retorno = hash_contiene(garage, patente);
    printf("%s\n", (retorno == deberia_existir) ? VERDE"EXITO"RESET : ROJO"FALLO"RESET);
}



void pruebas_null(){
    printf("\nHago pruebas con NULL\n");

    printf("inserto en un hash NULL (FALLA): %s\n", hash_insertar(NULL, "clave", "elemento")==ERROR?VERDE"EXITO"RESET:ROJO"FALLO"RESET);
    printf("Verifico que algo exista en un hash NULL (FALLA): %s\n", hash_contiene(NULL, "clave") == false ? VERDE"EXITO"RESET : ROJO"FALLO"RESET);
    printf("Cantidad en un hash NULL es cero: %s\n", hash_cantidad(NULL)==VACIO?VERDE"EXITO"RESET:ROJO"FALLO"RESET);
    printf("Intento quitar un elemento en un hash NULL (FALLA): %s\n", hash_quitar(NULL, "Nada") == ERROR ? VERDE "EXITO" RESET : ROJO "FALLO" RESET);
    printf("Busco un elemento en un hash NULL (FALLA): %s\n", hash_obtener(NULL, "Nada") == NULL ? VERDE "EXITO" RESET : ROJO "FALLO" RESET);

    hash_t* hash = hash_crear(destruir_string, 2);
    printf("inserto en un hash una clave NULL (FALLA): %s\n", hash_insertar(hash, NULL, "elemento") == ERROR ? VERDE"EXITO"RESET : ROJO"FALLO"RESET);
    printf("inserto en un hash un elemento NULL (INSERTA): %s\n", hash_insertar(hash, "PASA", NULL) == EXITO ? VERDE"EXITO"RESET : ROJO"FALLO"RESET);
    printf("Busco en un hash una clave NULL (FALLA): %s\n", hash_contiene(hash, NULL) == false ? VERDE"EXITO"RESET : ROJO"FALLO"RESET);
    

    printf("Creo un iterador externo con un hash NULL (FALLA): %s\n", hash_iterador_crear(NULL) == NULL ? VERDE"EXITO"RESET : ROJO"FALLO"RESET);
    printf("Avanzo en un iterador externo NULL (FALLA): %s\n", hash_iterador_siguiente(NULL) == NULL ? VERDE"EXITO"RESET : ROJO"FALLO"RESET);
    printf("Veo si un iterador externo NULL tiene siguiente (FALLA): %s\n", hash_iterador_tiene_siguiente(NULL) == false ? VERDE"EXITO"RESET : ROJO"FALLO"RESET);
    printf("Pruebo iterador interno con hash NULL (FALLA): %s\n", hash_con_cada_clave(NULL, mostrar_patente, NULL)==VACIO? VERDE"EXITO"RESET:ROJO"FALLO"RESET);
    printf("Pruebo iterador interno con hash NULL (FALLA): %s\n", hash_con_cada_clave(hash, NULL, NULL)==VACIO? VERDE"EXITO"RESET:ROJO"FALLO"RESET);
    hash_destruir(hash);

    printf("Creo un hash con destructor NULL (SE CREA): ");
    hash_t* sin_destructor = hash_crear(NULL, 4);
    printf("%s\n", sin_destructor != NULL?VERDE"EXITO"RESET:ROJO"FALLO"RESET);
    hash_destruir(sin_destructor);
}


void pruebas_hash_vacio(){
    printf("\nHago pruebas con un hash vacio\n");
    hash_t* hash = hash_crear(destruir_string, 2);
    printf("Cantidad en el hash es cero: %s\n", hash_cantidad(hash) == VACIO ? VERDE "EXITO" RESET : ROJO "FALLO" RESET);
    printf("Intento quitar un elemento en el hash vacio (FALLA): %s\n", hash_quitar(hash, "No existe") == ERROR ? VERDE "EXITO" RESET : ROJO "FALLO" RESET);
    printf("Verifico que algo exista en el hash vacio (FALLA): %s\n", hash_contiene(hash, "clave") == false ? VERDE "EXITO" RESET : ROJO "FALLO" RESET);
    printf("Busco un elemento en un hash vacio (FALLA): %s\n", hash_obtener(hash, "clave")==NULL?VERDE"EXITO"RESET:ROJO"FALLO"RESET);
    hash_destruir(hash);
}

void pruebas_funcionamiento(){
    printf("Pruebo el funcionamiento general del codigo\n");
    hash_t* garage = hash_crear(destruir_string, 3);

    printf("Agrego autos al garage\n");

    guardar_vehiculo(garage, "AC123BD", "Auto de Mariano");
    guardar_vehiculo(garage, "OPQ976", "Auto de Lucas");
    guardar_vehiculo(garage, "A421ACB", "Moto de Manu");
    guardar_vehiculo(garage, "AA442CD", "Auto de Guido");
    guardar_vehiculo(garage, "AC152AD", "Auto de Agustina");
    guardar_vehiculo(garage, "DZE443", "Auto de Jonathan");
    guardar_vehiculo(garage, "AA436BA", "Auto de Gonzalo");
    guardar_vehiculo(garage, "QDM443", "Auto de Daniela");
    guardar_vehiculo(garage, "BD123AC", "Auto de Pablo");
    guardar_vehiculo(garage, "CD442AA", "Auto de Micaela");
    guardar_vehiculo(garage, "PQO697", "Auto de Juan");
    guardar_vehiculo(garage, "FGV882", "Auto de Mateo");
    guardar_vehiculo(garage, "PDG702", "Auto de Marcos");
    guardar_vehiculo(garage, "BUC231", "Auto de Enrique");
    guardar_vehiculo(garage, "AD224ZE", "Auto de Fausto");
    guardar_vehiculo(garage, "AD123IT", "Auto de Jaime");
    guardar_vehiculo(garage, "AC675UY", "Auto de Carolina");
    guardar_vehiculo(garage, "AA234DS", "Auto de Luquitas");
    guardar_vehiculo(garage, "AC518LN", "Auto de Nicolas");
    guardar_vehiculo(garage, "ZZ999ZZ", "Auto de Hector");
    guardar_vehiculo(garage, "FFV976", "Auto de Valentina");
    guardar_vehiculo(garage, "HSG098", "Auto de Macarena");
    guardar_vehiculo(garage, "ASD654", "Auto de Ines");
    guardar_vehiculo(garage, "CAR105", "Auto de Carlos");
    guardar_vehiculo(garage, "KJL871", "Auto de Jose");


    printf("\nVerifico que esten algunas claves\n");
    verificar_vehiculo(garage, "QDM443", true);
    verificar_vehiculo(garage, "PQO697", true);
    verificar_vehiculo(garage, "PDG702", true);
    verificar_vehiculo(garage, "FGV882", true);
    verificar_vehiculo(garage, "CAR105", true);
    verificar_vehiculo(garage, "ASD654", true);
    verificar_vehiculo(garage, "ZZ999ZZ", true);
    verificar_vehiculo(garage, "BUC231", true);

    printf("\nMando elementos con la misma clave\n");
    guardar_vehiculo(garage, "FGV882", "La Cucaracha Atomica");
    guardar_vehiculo(garage, "DZE443", "Auto de Jonathan otra vez");
    guardar_vehiculo(garage, "AC152AD", "Auto de Agustina otra vez");
    guardar_vehiculo(garage, "AA234DS", "Auto de Luquitas otra vez");
    guardar_vehiculo(garage, "AC675UY", "Auto de Caro");
    guardar_vehiculo(garage, "AD123IT", "Auto de Jaime otra vez");
    guardar_vehiculo(garage, "QDM443", "Auto de Daniela otra vez");

    printf("\nRetiro algunois vehiculos\n");
    quitar_vehiculo(garage, "QDM443");
    quitar_vehiculo(garage, "PQO697");
    quitar_vehiculo(garage, "CAR105");
    quitar_vehiculo(garage, "PDG702");
    quitar_vehiculo(garage, "AC123BD");
    quitar_vehiculo(garage, "CD442AA");
    quitar_vehiculo(garage, "ASD654");
    quitar_vehiculo(garage, "AD123IT");
    quitar_vehiculo(garage, "ZZ999ZZ");

    printf("\nVerifico que esten algunas claves no esten\n");
    verificar_vehiculo(garage, "QDM443", false);
    verificar_vehiculo(garage, "PQO697", false);
    verificar_vehiculo(garage, "AAA123", false);
    verificar_vehiculo(garage, "BB998CC", false);
    verificar_vehiculo(garage, "N03574", false);
    verificar_vehiculo(garage, "NOEXISTE", false);
    verificar_vehiculo(garage, "TAMPOCOEXISTE", false);
    verificar_vehiculo(garage, "FGH456", false);
    verificar_vehiculo(garage, "IU6TY2", false);
    verificar_vehiculo(garage, "LLL999", false);
    verificar_vehiculo(garage, "AB987PP", false);
    verificar_vehiculo(garage, "FGV883", false);
    verificar_vehiculo(garage, "PDG702", false);

    printf("\nPruebo el iterador externo\n");
    hash_iterador_t *iter = hash_iterador_crear(garage);
    size_t listados = 0;

    while (hash_iterador_tiene_siguiente(iter)){
        const char *clave = hash_iterador_siguiente(iter);
        if (clave){
            listados++;
            printf("Patente: %s -- Vehiculo: %s\n", clave, (char *)hash_obtener(garage, clave));
        }
    }

    printf("Cantidad de autos guardados: %zu. Cantidad de autos listados: %zu -- %s\n\n", hash_cantidad(garage), listados, (hash_cantidad(garage) == listados) ? VERDE"EXITO"RESET : ROJO"FALLO"RESET);

    hash_iterador_destruir(iter);

    printf("\nPruebo el iterador interno\n");
    size_t impresas = hash_con_cada_clave(garage, mostrar_patente, NULL);
    printf("Se mostraron %zu patentes con el iterador interno\n\n", impresas);

    hash_destruir(garage);
}

int main(){
    pruebas_funcionamiento();
    pruebas_hash_vacio();
    pruebas_null();
    return 0;
}
