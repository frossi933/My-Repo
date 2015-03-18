#ifndef __PATH_H__
#define __PATH_H__

#include "Common.h"

/* devuelve 1 si existe el directorio INDEX en alguno de los directorios padres de "path"(en ese caso "result" contiene la ruta de dicho directorio contenedor de INDEX)
	    0 en caso de no existir en dichos directorios ("result" = NULL)*/
int		exist_index_up(char *path, char *result);

/* devuelve 1 si existe el directorio INDEX dentro de "path", o en alguno de sus subdirectorios(en este caso "result" contiene la ruta de dicho directorio contenedor de INDEX)
	    0 en caso de no existir en dichos directorios("result" = NULL)  */
int		exist_index_down(char *path, char *result);

char 	*get_element_from_path(char *path_complete);

char 	*name_of_son(TreeNode *parent);

char 	*get_father_element(char *path_comp);

/* Retorna 1 si todo andubo bien, y 0 en caso de error */
int 		get_path_complete(char *element, char *result);

/* Retorna 1 si existe el elemento "path"
		   0 en caso contrario */
int 		exist_elem(char *path);

void 	get_relative_path(char *path_complete, char *index_path, char *result);

/*funcion que verifica la existencia de INDEX, y si este existe obtengo su ruta, la del archivo contents y la del archivo que contiene el n° de revision */
void 	*verify_and_get_paths(char *index_path, char *contents_path, char *rev_path);

void 	get_rev_path(int nro_rev, char *index_path, char *result);

/* Retorna 1 si "path" es un directorio 
		   0 en caso contrario */
int 		is_dir(const char *path);

#endif /* __PATH_H__ */
