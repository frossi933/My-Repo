#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include "Common.h"

#include <stdio.h>

/* muestra en pantalla que el init se ha realizado con exito */
void 	message_init_done(void);

/* muestra en pantalla que el add se ha realizado con exito */
void 	message_add_done(void);

/* muestra en pantalla los nodos que se estan agregando al repositorio */
void 	message_adding_node(void *_node, void *extra);

/* muestra en pantalla que el rm se ha realizado con exito */
void 	message_rm_done(void *_node, void *extra);

/* muestra en pantalla que el commit se ha realizado con exito */
void 	message_commit_done(void);

/* establece el estado de "_node" como "_type", y lo muestra en pantalla */
void 	set_and_print_st(void *_node , void *_type);

/* muestra en pantalla que ambos repositorios son iguales */
void 	message_repos_equals(void);

/* muestra en pantalla el path y el estado correspondiente a "_node" */
void 	message_print_status(void *_node, void *_path);

/* Muestra en pantalla el archivo que se encuentra en el ultimo commit para la funcion log */
void 	message_current_file(void *_node, void *_extra);

/* Muestra en pantalla el archivo que se encuentra en el commit de la revision dada por la funcion log pero no en la actualidad */
void 	message_old_file(void *_node, void *_extra);

/* muestra en pantalla las diferencias entre los archivos "res_patch" y "current_file" */
void 	message_print_diffs(FILE *res_patch, FILE *current_file, char *path);

/* muestra en pantalla el mensaje y la fecha del commit de "file" en la revision numero "rev" */
void 	show_time_and_message(FILE *file, int rev);

#endif	/* __MESSAGES_H__ */