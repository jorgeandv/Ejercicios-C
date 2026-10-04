#include <stddef.h>

#include "prev.h"

//La función asignarPrev recibe como parametro, un puntero 't' que apunta a la raiz del arbol t
// y un puntero a puntero 'pprev' que apunta a el nodo previo

/*
¿Por que *pprev es de entrada y de salida?
Porque cuando se llama a la función, el *pprev apunta al nodo que se visitó antes de empezar con el árbol
(inicialmente apunta al nodo 0).

Cuando se sale de la función, el *pprev apunta al último nodo visitado.
*/

void asignarPrev(Nodo *t, Nodo **pprev) {
    //Caso base: El arbol es vacio
    if (t == NULL) {
        return; //No hacemos nada
    }

    //Como el arbol se recorre inorden, hay que asignarPrev a izq, nodo, der.
    
    //Se usa asignarPrev() en el subarbol izquierdo
    //Al salir de la función, el *pprev queda apuntando al último nodo visitado en ese subarbol izquierdo
    asignarPrev(t->izq, pprev);

    //Guardamos una copia (localmente), el puntero prev es una copia del puntero que apunta al nodo previo (*pprev)
    Nodo *prev = *pprev;     

    //Cuando visitamos el nodo t, su nodo previo es prev (el último visitado del subarbol izquierdo)
    t->prev = prev;
    //Cuando visitamos el nodo t, asignamos NULL a su nodo próximo por ahora
    t->prox = NULL;
    //Si el nodo previo a T no es NULL
    if (t->prev != NULL) {
        (t->prev)->prox = t;    //Accedo al nodo proximo del nodo previo a t y le doy el valor de t
    }
    //Antes de continuar el recorrido (con el subarbol derecho) asigno t a *pprev pues t es el último nodo visitado, 
    //y por lo tanto, debe ser el previo del primer nodo del subarbol derecho
    *pprev = t;

    //Se usa asignarPrev() en el subarbol derecho
    //Al salir de la función, el *pprev queda apuntando al último nodo visitado en ese subarbol derecho
    asignarPrev(t->der, pprev);
}

