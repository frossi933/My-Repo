#ifndef __CONTENT_H__
#define __CONTENT_H__

#include "Common.h"
#include "Tree.h"

#include <stdio.h>

/* Devuelve el numero de la linea en la que se encuentra "path" en "contents", o -1 si no se encuentra */
int 		elem_in_contents(char *path, char *contents_path);

/* Obtiene el numero de revision actual que se encuentra en el archivo "rev_path" y lo guarda en la variable apuntada por "rev */
void 	get_number_of_rev(char *rev_path, int *rev);

/*	inicializa el archivo "contents"
	devuelve 0 si fue inicializado correctamente
			1 si ocurrio algun error */
int		content_init(char *path);

/*	vuelca los archivos de "root" al archivo "path_file"	
	"type" indica el modo con el que deben imprimirse los nodos en "path_file", en caso de ser necesario "mes" contiene el mensaje correspondiente al commit
	Retorna 1 si hubo algun error 
		      0 en caso contrario 		*/
int 		print_tree(TreeNode *root, char *path_file, ConstructionType type, char *mes);

/*	actualiza el numero de revision del archivo contents	*/
void	update_rev(char *rev_path, int new_rev);

#endif /*__CONTENT_H__*/
