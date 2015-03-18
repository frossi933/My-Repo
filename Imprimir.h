#ifndef __IMPRIMIR_H__
#define __IMPRIMIR_H__

#include <stdio.h>
#include <string.h>

#include "Diff.h"

/* Imprime las diferencias con el formato de salida perteneciente al comando 'diff' */
void Imprimir_diff_lines(char **lines, Ind *field, int lines_n1, int lines_n2, FILE *salida);

#endif	/* __IMPRIMIR_H__ */
