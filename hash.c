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
#define MAX_PRIMO 100

typedef struct elemento{
    char* clave;
    void* elemento;
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
size_t hasheador(const char* clave){
    size_t numero = 0;
    for (int i = 0; i < strlen(clave); i++)
        numero += (size_t)(clave[i]*(i+1));
    return numero;
}

ele_t* crear_elemento(const char* clave, void* elemento){
    ele_t* aux = calloc(1, sizeof(ele_t));
    if(!aux)
        return NULL;
    aux->elemento = elemento;
    aux->clave = malloc(strlen(clave)+1);
    if(!aux->clave){
        free(aux);
        return NULL;
    }
    strcpy(aux->clave, clave);
    return aux;
}


/*
 * Recibira una lista y una clave y un puntero a posicion.
 * 
 * Buscara y devolvera el elemento, si es que existe. O
 * NULL si hay un error o no esta. Si recibe un puntero a un entero de posicion
 * le asiganara un valor para borrar el elemento en dicha posicion de la lista
 */
ele_t* buscar_elemento(lista_t* lista, const char* clave, int* posicion){
    lista_iterador_t* iterador = lista_iterador_crear(lista);
    if(!iterador)
        return NULL;
    ele_t* aux = NULL;
    bool encontrado = false;
    int i = 0;
    while (lista_iterador_tiene_siguiente(iterador) && !encontrado){
        aux = (ele_t*)lista_iterador_siguiente(iterador);
        if(strcmp(clave, aux->clave) == IGUAL)
            encontrado = true;
        i++;
    }
    lista_iterador_destruir(iterador);
    if (!encontrado)
        return NULL;
    if(*posicion != ERROR)
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
 * 
 * Creara un nuevo hash y hara un swap con el nuevo. Despues destruira el viejo.
 */
int rehash(hash_t* hash);

/*
 * En el caso de que se reciba una clave existenete se llamara a esta funcion, mandandole el hash
 * el elemento nuevo a insertar y la lista donde se encuentra dicha clave.
 * 
 * Buscara el elemento en la lista y hara un swap con el nuevo para que este quede insertado. Luego
 * liberara la memoria del elemento viejo.
 */
void reemplazar(hash_t* hash, ele_t* nuevo, lista_t* lista){
    int pos = 0;
    ele_t* borrado = buscar_elemento(lista, nuevo->clave, &pos);
    ele_t aux = *borrado;
    *borrado = *nuevo;
    *nuevo = aux;
    free(nuevo->clave);
    if(hash->destructor)
        hash->destructor(nuevo->elemento);
    free(nuevo);
}

int hash_insertar(hash_t* hash, const char* clave, void* elemento){
    if(!hash || !clave)
        return ERROR;
    size_t pos = (hasheador(clave)%hash->capacidad);
    if(!hash->vector[pos].lista){
        hash->vector[pos].lista = lista_crear();
        if(!hash->vector[pos].lista)
            return ERROR;
        hash->pos_habilitadas++;
    }
    if(!lista_vacia(hash->vector[pos].lista)){
        if(hash_contiene(hash, clave)){
            ele_t* nuevo = crear_elemento(clave, elemento);
            if(!nuevo)
                return ERROR;
            reemplazar(hash, nuevo, hash->vector[pos].lista);
            return EXITO;
        }
    }
    ele_t* insertado = crear_elemento(clave, elemento);
    int retorno = lista_insertar(hash->vector[pos].lista, insertado);
    if(retorno == ERROR)
        return ERROR;
    hash->cant_elementos++;
    if(calcular_carga(hash)>=MAX_CARGA)
        rehash(hash); 
    return EXITO;
}

int hash_quitar(hash_t *hash, const char *clave){
    if(!hash || !clave)
        return ERROR;
    int retorno = EXITO, pos_lista = 0;
    size_t pos = (hasheador(clave) % hash->capacidad);
    if (lista_vacia(hash->vector[pos].lista))
        return ERROR;
    ele_t* aux = buscar_elemento(hash->vector[pos].lista, clave, &pos_lista);
    if(!aux)
        return ERROR;
    free(aux->clave);
    if (hash->destructor)
        hash->destructor(aux->elemento);
    free(aux);
    retorno = lista_borrar_de_posicion(hash->vector[pos].lista, (size_t)pos_lista);
    if(retorno == ERROR)
        return ERROR;
    hash->cant_elementos--;
    return retorno;
}

void* hash_obtener(hash_t *hash, const char *clave){
    if(!hash || !clave)
        return NULL;
    size_t pos = (hasheador(clave) % hash->capacidad);
    if(lista_vacia(hash->vector[pos].lista))
        return NULL;
    int numero = ERROR;
    ele_t* aux = buscar_elemento(hash->vector[pos].lista, clave, &numero);
    if(!aux)
        return NULL;
    return aux->elemento;
}

bool hash_contiene(hash_t *hash, const char *clave){
    if(!hash || !clave)
        return false;
    int i = ERROR;
    size_t pos = (hasheador(clave) % hash->capacidad);
    if (lista_vacia(hash->vector[pos].lista))
        return false;
    ele_t *esta = buscar_elemento(hash->vector[pos].lista, clave, &i);
    if (!esta)
        return false;
     if (strcmp((const char *)(esta->clave), clave) != IGUAL)
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
            free(aux->clave);
             if(hash->destructor)
                hash->destructor(aux->elemento);
            free(aux);
        }
        lista_iterador_destruir(iterador);
        lista_destruir(hash->vector[i].lista);
    }
    free(hash->vector);
    free(hash);
}

size_t hash_con_cada_clave(hash_t* hash, bool (*funcion)(hash_t* hash, const char* clave, void* aux), void* aux){
    if(!hash || !funcion)
        return VACIO;
    size_t cant = 0;
    bool corte = false;
    int i = 0;
    while(i<hash->capacidad && !corte){
        if(!lista_vacia(hash->vector[i].lista)){
            lista_iterador_t* iterador = lista_iterador_crear(hash->vector[i].lista);
            if(!iterador)
                return VACIO;
            ele_t* elem = NULL;
            while (lista_iterador_tiene_siguiente(iterador) && !corte){
                elem = lista_iterador_siguiente(iterador);
                corte = funcion(hash, elem->clave, aux);
                cant++;
            }
            lista_iterador_destruir(iterador);
        }
        i++;
    }
    return cant;
}


/*
 * En el caso de que se deba rehashear se llamara a esta funcion la cual recibira
 * el hash, una clave y el hash nuevo.
 * 
 * Inseratara los elementos en el hash nuevo y siempre retornara false
 */
bool rehashear(hash_t* hash, const char* clave, void* aux){
    hash_t* nuevo = (hash_t*)aux;
    hash_insertar(nuevo, clave, hash_obtener(hash, clave));
    return false;
}

/*
 * Recibira la capacidad del hash viejo para asignarle la capacidad al nuevo
 * 
 * Calculara un numero primo si la capacidad original es menor o igual a 100,
 * si no, devolvera el doble de la original mas uno.
 */
size_t proximo_primo(size_t capacidad){
    size_t primo = ((capacidad * 2) + 1);
    if(capacidad <= MAX_PRIMO){
        while(primo%2==0 || primo%3==0 || primo%5==0 || primo%7==0)
            primo++;
    }else
        primo = ((capacidad * 2) + 1);
    return primo;
}

int rehash(hash_t *hash){
    hash_t* nuevo = hash_crear(hash->destructor, proximo_primo(hash->capacidad));
    if(nuevo == NULL){
        return ERROR;
    }
    size_t cantidad = hash_con_cada_clave(hash, rehashear, nuevo);
    if(cantidad != hash->cant_elementos){
        hash_destruir(nuevo);
        return ERROR;
    }

    hash_t aux = *hash;
    *hash = *nuevo;
    *nuevo = aux;
    nuevo->destructor = NULL;
    hash_destruir(nuevo);
    return EXITO;
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
    if(iterador->posicion >= (iterador->hash->capacidad-1) && !iterador->sigue)
        return false;
    return true;
}

const char* hash_iterador_siguiente(hash_iterador_t* iterador){
    if(!iterador)
        return NULL;
    if(!iterador->lista){
        if(!iterador->hash->vector[iterador->posicion].lista){ 
            iterador->posicion++;
        }
        iterador->lista = iterador->hash->vector[iterador->posicion].lista;
        iterador->iterador_lista = lista_iterador_crear(iterador->lista);
    }
    iterador->sigue = lista_iterador_tiene_siguiente(iterador->iterador_lista);
    if(!iterador->sigue){
        if(iterador->posicion < (iterador->hash->capacidad) - 1){
            lista_iterador_destruir(iterador->iterador_lista);
            iterador->posicion++;
            bool corte = false;
            while(iterador->posicion < iterador->hash->capacidad && !corte){
                if (!iterador->hash->vector[iterador->posicion].lista)
                    iterador->posicion++;
                else
                    corte = true;
            }
            if(corte){
                iterador->lista = iterador->hash->vector[iterador->posicion].lista;
                iterador->iterador_lista = lista_iterador_crear(iterador->lista);
                iterador->sigue = lista_iterador_tiene_siguiente(iterador->iterador_lista);
            }else{
                iterador->sigue = false;
                return NULL;
            }
        }else{
            return NULL;
        }
    }
    iterador->elemento = (ele_t*)lista_iterador_siguiente(iterador->iterador_lista);
    if(!iterador->elemento)
        return NULL;
    return (const char*)(iterador->elemento->clave);
}

void hash_iterador_destruir(hash_iterador_t *iterador){
    if(!iterador)
        return;
    if(iterador->sigue)
        free(iterador->iterador_lista);
    free(iterador);
}