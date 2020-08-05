#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "lista.h"
#include "hash.h"
#include "hash_iterador.h"

#define CAPACIDAD_MIN 3
#define ERROR -1
#define EXITO 0
#define IGUAL 0
#define VACIO 0
#define MAX_CARGA 75

typedef struct elemento{
    char* patente;
    void* despcripcion;
}ele_t;


typedef struct vector{
    lista_t* lista;
}vector_t;

struct hash{
    vector_t* vector;
    hash_destruir_dato_t destructor;
    size_t capacidad;
    size_t cant_elementos;
    size_t pos_habilitadas;
};

struct hash_iter{
    hash_t* hash;
    lista_t* lista;
    lista_iterador_t* iterador_lista;
    int posicion;
    size_t capacidad;
    bool sigue;
    ele_t* elemento;
};

hash_t* hash_crear(hash_destruir_dato_t destruir_elemento, size_t capacidad){
    if(!capacidad)
        return NULL;
    hash_t* aux = calloc(1, sizeof(hash_t));
    if(!aux)
        return NULL;
    if(capacidad < CAPACIDAD_MIN)
        capacidad = CAPACIDAD_MIN;
    aux->capacidad = capacidad;
    vector_t* vector_aux = calloc(aux->capacidad, sizeof(vector_t));
    if(!vector_aux){
        free(aux);
        return NULL;
    }
    aux->vector = vector_aux;
    aux->destructor = destruir_elemento;
    return aux;
}

/*
 * Se le enviara una clave y devolvera la posicion en la
 * cual se tiene que insertar o esta el elemento de dicha
 * clave
 */
int hasher(const char* clave, hash_t* hash){
    size_t numero = 0;
    for (int i = 0; i < strlen(clave); i++)
        numero += clave[i]*(i+1);
    return numero % hash->capacidad;
}

ele_t* crear_elemento(const char* clave, void* elemento){
    ele_t* aux = calloc(1, sizeof(ele_t));
    if(!aux)
        return NULL;
    aux->despcripcion = elemento;
    aux->patente = malloc(strlen(clave)+1);
    if(!aux->patente){
        free(aux);
        return NULL;
    }
    strcpy(aux->patente, clave);
    return aux;
}


/*
 * Recibira una lista y una clave y un puntero a posicion.
 * 
 * Buscara y devolvera el elemento, si es que existe. O
 *  NULL si hay un error o no esta. Si recibe un puntero a posicion
 * le asiganara un valor para borrar el elemento en dicha posicion de la lista
 */
ele_t* buscar_elemento(lista_t* lista, const char* clave, int* posicion){
    lista_iterador_t* iterador = lista_iterador_crear(lista);
    ele_t* aux = NULL;
    bool encontrado = false;
    int i = 0;
    while (lista_iterador_tiene_siguiente(iterador) && !encontrado){
        aux = (ele_t*)lista_iterador_siguiente(iterador);
        if (strcmp(clave, aux->patente) == IGUAL)
            encontrado = true;
        i++;
    }
    lista_iterador_destruir(iterador);
    if (!encontrado)
        return NULL;
    if(*posicion)
        *posicion = (i-1);
    return aux;
}

//Calcula el porcentaje de carga del hash
size_t calcular_carga(hash_t* hash){
    return((hash->pos_habilitadas*100)/hash->capacidad);
}


/*
 * En el caso de que se exceda el factor de balanceo se llamara a esta funcion
 * pasandole el hash.
 * Devolvera un nuevo vector de listas ya hasheado con mas capacidad la cual sera
 * el numero primo mas cercano al doble de la capacidad original
 */
vector_t* rehash(hash_t* hash){

}

int hash_insertar(hash_t* hash, const char* clave, void* elemento){
    if(!hash || !clave || !elemento)
        return ERROR;
    int pos = hashing(clave);
    if(!hash->vector[pos].lista){
        lista_t* aux= lista_crear();
        if(!aux)
            return ERROR;
        hash->pos_habilitadas++;
    }
    ele_t* vehiculo = crear_elemento(clave, elemento);
    int retorno = lista_insertar(hash->vector[pos].lista, vehiculo);
    if(retorno == ERROR)
        return ERROR;
    hash->cant_elementos++;
    if(calcular_carga(hash)>=MAX_CARGA)
        hash->vector = rehash(hash); 
    return EXITO;
}

int hash_quitar(hash_t *hash, const char *clave){
    if(!hash || !clave)
        return ERROR;
    int retorno = EXITO, pos_lista;
    int pos = hashing(clave);
    if (lista_vacia(hash->vector[pos].lista))
        return ERROR;
    ele_t* aux = buscar_elemento(hash->vector[pos].lista, clave, &pos_lista);
    free(aux->patente);
    hash->destructor(aux);
    retorno = lista_borrar_de_posicion(hash->vector[pos].lista, (size_t)pos_lista);
    if(retorno == ERROR)
        return ERROR;
    hash->cant_elementos--;
    return retorno;
}

void* hash_obtener(hash_t *hash, const char *clave){
    if(!hash || !clave)
        return NULL;
    int pos = hashing(clave);
    if(lista_vacia(hash->vector[pos].lista))
        return NULL;
    ele_t* aux = buscar_elemento(hash->vector[pos].lista, clave, NULL);
    if(!aux)
        return NULL;
    return aux;
}

bool hash_contiene(hash_t *hash, const char *clave){
    if(!hash || !clave)
        return false;
    ele_t* esta = hash_obtener(hash, clave);
    if(!esta)
        return false;
    return true;
}

size_t hash_cantidad(hash_t *hash){
    if(!hash)
        return VACIO;
    return hash->cant_elementos;
}

void hash_destruir(hash_t *hash){
    if(!hash)
        return;
    for(int i = 0; i<hash->capacidad; i++){
        lista_iterador_t* iterador = lista_iterador_crear(hash->vector[i].lista);
        ele_t* aux = NULL;
        while(lista_iterador_tiene_siguiente(iterador)){
            aux = lista_iterador_siguiente(iterador);
            free(aux->patente);
            hash->destructor(aux);
        }
        lista_iterador_destruir(iterador);
        lista_destruir(hash->vector[i].lista);
    }
    free(hash->vector);
    free(hash);
}

size_t hash_con_cada_clave(hash_t* hash, bool (*funcion)(hash_t* hash, const char* clave, void* aux), void* aux){
    if(!hash || !funcion || !aux)
        return VACIO;
    size_t cant = 0;
    bool corte = false;
    int i = 0;
    while(i<hash->capacidad && !corte){
        if(!lista_vacia(hash->vector[i].lista)){
            lista_iterador_t* iterador = lista_iterador_crear(hash->vector[i].lista);
            ele_t* elem = NULL;
            while (lista_iterador_tiene_siguiente(iterador) && !corte){
                elem = lista_iterador_siguiente(iterador);
                corte = funcion(hash, elem->patente, aux);
                cant++;
            }
            lista_iterador_destruir(iterador);
            i++;
        }
    }
    return cant;
}

hash_iterador_t* hash_iterador_crear(hash_t* hash){
    if(!hash)
        return NULL;
    hash_iterador_t* aux = calloc(1, sizeof(hash_iterador_t));
    if(!aux)
        return NULL;
    aux->hash = hash;
    aux->sigue = true;
    return aux;
}

bool hash_iterador_tiene_siguiente(hash_iterador_t *iterador){
    if(!iterador)
        return false;
    if(iterador->posicion >= (iterador->capacidad-1) && !iterador->sigue)
        return false;
    return true;
}

const char* hash_iterador_siguiente(hash_iterador_t* iterador){
    if(!iterador)
        return NULL;
    if(!iterador->lista){
        if(!iterador->hash->vector[iterador->posicion].lista){ 
            iterador->posicion++;
            return NULL;
        }
        iterador->lista = iterador->hash->vector[iterador->posicion].lista;
        iterador->iterador_lista = lista_iterador_crear(iterador->lista);
    }
    iterador->sigue = lista_iterador_tiene_siguiente(iterador->iterador_lista);
    iterador->elemento = (ele_t*)lista_iterador_siguiente(iterador->iterador_lista);

    
}
