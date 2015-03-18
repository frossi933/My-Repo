#ifndef __FILE_H__
#define __FILE_H__

/* Realiza los patchs correspondientes del archivo "path", desde el que se encontraba en la rev "from" hasta el de la rev "to" */
void 	make_patch(int from, int to, char *path);

/* crea el archivo de diferencias correspondiente al archivo del nodo "_node" */
void 	create_diffs(void *_node, void *_new_rev);

/*muestra en pantalla las diferencias del archivo "path" actual con el correspondiente a la revision "old_rev"*/
void	show_diffs(char *path, int old_rev);

/* muestra en pantalla el nombre relativo del archivo contenido en el nodo "_node" */
void 	compare_show_files(void *_node, void *_index_path);

#endif /* __FILE_H__ */
