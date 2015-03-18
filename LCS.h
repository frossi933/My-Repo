#ifndef __LCS_H__
#define __LCS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*funcion comparacion: return 1 si son iguales
						     0 si son distintos*/
typedef int (*CompareFunc) (void *a, void *b);

typedef void (*VisitorLCSFunc) (void *data, int i1, int i2, void *extra_data);

/*	aplica visit a los elementos de la subsecuencia comun mas larga, entre sec1 y sec2, y a la pos de cada uno dentro de los arreglos	*/
void LCS(void *sec1, void *sec2, size_t size, int lon1, int lon2, CompareFunc compare, VisitorLCSFunc visit, void *extra_data);

#endif	/* __LCS_H__ */
