#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "lista.h"

#define EXITO 0
#define INICIO 0
#define ERROR -1
#define UNICO 1
#define MENSAJE "Un iterador deja de ser valido si se modifica la lista. Por lo tanto se detruye"

typedef struct nodo{
  void* dato;
  struct nodo* siguiente;
}nodo_t;

struct lista{
  nodo_t* inicio;
  nodo_t* final;
  int cantidad;
};

struct lista_iterador{
  bool sigue;
  int cantidad;
  nodo_t* posicion;
  lista_t* lista;
};

/*
 * Pre: Recibira dos nodos, uno nuevo a insertar en cierta posicion y el nodo anterior
 * a la posicion.
 *
 * Post: Se insertara el nodo en su nueva posicion teniendo en cuenta los punteros a
 * los siguientes.
 */
void insertar(nodo_t* insertado, nodo_t* chequeado){
  insertado->siguiente = chequeado->siguiente;
  chequeado->siguiente = insertado;
}

/*
 * Le asignara el elemento al nuevo nodo, sea un valor o NULL.
 */
void agregar_dato(nodo_t* nodo, void* elemento){
  if(!elemento){
    nodo->dato = NULL;
  }else{
    nodo->dato = elemento;
  }
}
/*
 * Pre: Recibira una lista, el nodo nuevo a insertar y su elemento.
 *
 * Post: Agregara el nuevo nodo al final de la lista y devolvera 0.
 */

int insertar_ultimo(lista_t* lista, nodo_t* nuevo, void* elemento){
  agregar_dato(nuevo, elemento);
  nuevo->siguiente = NULL;
  if(!lista->inicio){
    lista->inicio = nuevo;
    lista->final = nuevo;
  }else{
    nodo_t* ultimo = lista->final;
    ultimo->siguiente = nuevo;
    lista->final = nuevo;
  }
  lista->cantidad++;
  return EXITO;
}
/*
 * Borrara la primera posicion de la lista y modificara el puntero al inicio.
 */
int borrar_primero(lista_t* lista){
  nodo_t* aux = lista->inicio;
  if(lista->cantidad == UNICO)
    aux->siguiente = NULL;

  lista->inicio = aux->siguiente;
  free(aux);
  lista->cantidad--;
  return EXITO;
}

lista_t* lista_crear(){
  lista_t* lista;
  lista = calloc(1, sizeof(lista_t));
  if(!lista){
    return NULL;
  }
  return lista;
}

int lista_insertar(lista_t* lista, void* elemento){
  nodo_t* nuevo;
  if(!lista){
    return ERROR;
  }
  nuevo = malloc(sizeof(nodo_t));
  if(!nuevo){
    return ERROR;
  }
  return insertar_ultimo(lista, nuevo, elemento);
}
/*
 * Recibira dos nodos e insertara el nuevo al inicio de la lista y modificara el puntero al inicio.
 */
int insertar_primero(nodo_t* insertado, nodo_t* chequeado, lista_t* lista){
  insertado->siguiente = chequeado;
  lista->inicio = insertado;
  lista->cantidad++;
  return EXITO;
}

int lista_insertar_en_posicion(lista_t* lista, void* elemento, size_t posicion){
  if(!lista || (int)(posicion) < INICIO){
    return ERROR;
  }
  int anterior = ((int)(posicion)-1);
  nodo_t* insertado = malloc(sizeof(nodo_t));
  if(!insertado){
    return ERROR;
  }
  agregar_dato(insertado, elemento);
  nodo_t* chequeado = lista->inicio;
  if((int)(posicion) == INICIO){
    return insertar_primero(insertado, chequeado, lista);
  }
  if(lista->cantidad < (int)(posicion)){
    return insertar_ultimo(lista, insertado, elemento);
  }
  for(int i = 0; i < anterior; i++){
    chequeado = (chequeado->siguiente);
  }
  insertar(insertado, chequeado);
  lista->cantidad++;
  return EXITO;
}

int lista_borrar(lista_t* lista){
  if(!lista || !lista->cantidad){
    return ERROR;
  }
  if(lista->cantidad == 1)
    return borrar_primero(lista);

  nodo_t* final_nuevo;
  nodo_t* aux;
  aux = lista->inicio;
  for(int i = 0; i < (lista->cantidad - 1); i++){
    final_nuevo = aux;
    aux = aux->siguiente;
  }
  lista->final = final_nuevo;
  final_nuevo->siguiente = NULL;
  free(aux);
  lista->cantidad--;
  return EXITO;
}

/*
 * Pre: Recibira una lista y la posicion deseada para borrar.
 *
 * Post: Borrara la posicion deseada y solo devolvera 0 ya que los chequeos
 * se haran antes.
 */
int buscar_y_borrar(lista_t* lista, size_t posicion){
  int anterior = ((int)(posicion) - 1);
  nodo_t* buscador = lista->inicio;
  nodo_t* borrado;
  for(int i = 0; i < anterior; i++){
    buscador = buscador->siguiente;
  }
  borrado = buscador->siguiente;
  buscador->siguiente = borrado->siguiente;
  free(borrado);
  lista->cantidad--;
  return EXITO;
}

int lista_borrar_de_posicion(lista_t* lista, size_t posicion){
  if(!lista || !lista->cantidad){
    return ERROR;
  }
  if((lista->cantidad-1) <= (int)(posicion)){
    return lista_borrar(lista);
  }
  if((int)(posicion) == INICIO){
    return borrar_primero(lista);
  }
  return buscar_y_borrar(lista, posicion);
}

void* lista_elemento_en_posicion(lista_t* lista, size_t posicion){
  if(!lista || (int)(posicion) >= lista->cantidad || (int)(posicion) < INICIO){
    return NULL;
  }
  if((lista->cantidad - 1) == (int)(posicion)){
    return lista->final->dato;
  }
  nodo_t* buscado;
  buscado = lista->inicio;
  for(int i = 0; i < (int)(posicion); i++){
    buscado = buscado->siguiente;
  }
  return (void*)(buscado->dato);
}

void* lista_ultimo(lista_t* lista){
  if(!lista || lista->cantidad == 0){
    return NULL;
  }
  if(lista->final->dato == NULL){
    return NULL;
  }
  return lista->final->dato;
}

bool lista_vacia(lista_t* lista){
  if(lista == NULL){
    return true;
  }else if(lista->cantidad == 0){
    return true;
  }
  return false;
}

size_t lista_elementos(lista_t* lista){
  if(!lista){
    return (size_t)(NULL);
  }
  return (size_t)(lista->cantidad);
}

int lista_apilar(lista_t* lista, void* elemento){
  if(!lista){
    return ERROR;
  }
  nodo_t* nuevo;
  nuevo = malloc(sizeof(lista_t));
  if(!nuevo){
    return ERROR;
  }
  return insertar_ultimo(lista, nuevo, elemento);
}

int lista_desapilar(lista_t* lista){
  if(!lista){
    return ERROR;
  }
  if(lista->cantidad == 1){
    free(lista->inicio);
    lista->inicio = NULL;
    lista->cantidad--;
    return EXITO;
  }
  return lista_borrar(lista);
}

void* lista_tope(lista_t* lista){
  if(!lista || !lista->inicio || !lista->final){
    return NULL;
  }
  return (void*)(lista->final->dato);
}

int lista_encolar(lista_t* lista, void* elemento){
  if(!lista){
    return ERROR;
  }
  nodo_t* nuevo = malloc(sizeof(nodo_t));
  if(!nuevo){
    return ERROR;
  }
  return insertar_ultimo(lista, nuevo, elemento);
}

int lista_desencolar(lista_t* lista){
  if(!lista){
    return ERROR;
  }
  return borrar_primero(lista);
}

void* lista_primero(lista_t* lista){
  if(!lista){
    return NULL;
  }
  return (void*)(lista->inicio->dato);
}

void destruir_nodos(nodo_t* borrado, int cantidad){
  nodo_t* auxiliar;
  for(int i = 0; i < cantidad; i++){
    auxiliar = borrado->siguiente;
    free(borrado);
    borrado = auxiliar;
  }
}

void lista_destruir(lista_t* lista){
  if(!lista){
    return;
  }
  nodo_t* borrado = lista->inicio;
  if(lista->inicio){
    destruir_nodos(borrado, lista->cantidad);
  }
  free(lista);
}

void lista_iterador_destruir(lista_iterador_t* iterador){
  free(iterador);
}

/*
 * Pre:Recibe un nodo y la lista por la cual itera
 * Post: Devolvera true en el caso de que se haya modificado la lista, o sea si
 * la cantidad de elementos de la lista es distinta a cuando el iterador se creo
 */
bool no_es_valido(lista_iterador_t* iterador, lista_t* lista){
  return (iterador->cantidad != lista->cantidad);
}

/*
 * Recibe un iterador y en el caso de que el iterador o su lista
 * no sean validos devolvera true para indicar que si hay un error
 */
bool hay_error(lista_iterador_t* iterador){
  if(iterador == NULL){
    return true;
  }else if(iterador->lista == NULL){
    return true;
  }
  return false;
}

/*
 * Recibira un iterador para chequear si este o su lista son validos,
 * devolvera true si esta todo bien y false si hay un problema.
 */
bool es_valido(lista_iterador_t* iterador){
  if(hay_error(iterador)){
    return false;
  }else if(no_es_valido(iterador, iterador->lista)){
    lista_iterador_destruir(iterador);
    printf("%s\n", MENSAJE);
    return false;
  }
  return true;
}

lista_iterador_t* lista_iterador_crear(lista_t* lista){
  if(!lista){
    return NULL;
  }
  lista_iterador_t* iterador = calloc(1, sizeof(lista_iterador_t));
  iterador->lista = lista;
  iterador->cantidad = lista->cantidad;
  iterador->sigue = true;
  return iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t* iterador){
  if(!es_valido(iterador)){
    return false;
  }
  nodo_t* aux = iterador->posicion;
  if(aux == NULL){
    iterador->posicion = iterador->lista->inicio;
    return true;
  }
  if(!iterador->posicion->siguiente){
    iterador->sigue = false;
    return false;
  }
  iterador->posicion = aux->siguiente;
  return true;
}

void* lista_iterador_siguiente(lista_iterador_t* iterador){
  if(!es_valido(iterador)){
    return false;
  }
  if(iterador->posicion == NULL)
    return NULL;
  if(iterador->sigue == false)
    return NULL;

  nodo_t* devolucion = iterador->posicion;
  return (void*)(devolucion->dato);
}

void lista_con_cada_elemento(lista_t* lista, void (*funcion)(void*, void*), void *contexto){
  if(!lista || !funcion){
    return;
  }
  nodo_t* posicion = lista->inicio;
  for(int i = 0; i < lista->cantidad; i++){
    funcion(posicion->dato, contexto);
    posicion = posicion->siguiente;
  }

}
