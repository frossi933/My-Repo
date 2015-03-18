#ifndef __DIFF_H__
#define __DIFF_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_NUMBER_OF_LINES		200
#define STEP_BUF						100
#define LENGTH_LINE					300
#define SET(ind,n)				((ind[(n)/8]) |= (1<<(7-((n)%8))))
#define IS_ONE(i,indi)			((indi[(i)/8]) & (1<<(7-((i)%8))))


typedef struct {
	unsigned char	*indicador1;
	unsigned char	*indicador2;
} Ind;

/* funciones auxiliares */
void 	visit_lines(void *data,int i1,int i2,void *extra_data);

int 		compare_lines(void *a, void *b);


/* funcion que muestra las diferencias entre "original" y "new", a traves de "salida" */
void		my_diff(FILE *original, FILE *new, FILE *salida);

#endif	/* __DIFF_H__ */
