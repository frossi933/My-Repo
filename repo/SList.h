#ifndef __SLIST_H__
#define __SLIST_H__

#include "Common.h"

#include <stdlib.h>
#include <stddef.h>

#define slist_data(l)       ((l)->data)
#define slist_next(l)       ((l)->next)
#define slist_is_empty(l)   ((l) == NULL)

/* agrega un nodo con "data" al final de "list" y devuelve la nueva lista */
SList 	*slist_append (SList *list, void *data);

/* agrega un nodo con "data" al principio de "list" y devuelve la nueva lista */
SList 	*slist_prepend (SList *list, void *data);

/* Destruye la lista "list" */
void  	slist_destroy (SList *list);

/* Aplica la funcion "visit" a cada nodo de "list" */
void 	 slist_foreach (SList *list, VisitorFunc visit, void *extra_data);

/* retorna un puntero a una nueva lista creada, que es igual a "list" sacandole los nodos q poseen a "data". "list" es liberada pero los nodos removidos no */
SList	*slist_remove (SList *list, void *data);

/* Retorna la longitud de la lista apuntada por "list" */
size_t slist_length (SList *list);

#endif /* __SLIST_H__ */
