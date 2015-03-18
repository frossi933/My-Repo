#include "LCS.h"

typedef enum { ARRIBA , IZQUIERDA , DIAGONAL } movimiento;

typedef struct {
	int			lon; 		//long del lcs anterior
	movimiento	mov; 	//movimiento de avance en la matriz
} Elem_Matriz;

static void LCS_internal(void *sec1, size_t size, Elem_Matriz **matriz, int lon1, int lon2, VisitorLCSFunc visit, void *extra_data){
	if(lon1 == 0  ||  lon2 == 0)
		return;
	if(matriz[lon1][lon2].mov == DIAGONAL){
		visit(sec1+(lon1-1)*size, lon1-1, lon2-1, extra_data);
		LCS_internal(sec1, size, matriz, lon1-1, lon2-1, visit, extra_data);
	} else if (matriz[lon1][lon2].mov == IZQUIERDA)
		LCS_internal(sec1, size, matriz, lon1-1, lon2, visit, extra_data);
	else
		LCS_internal(sec1, size, matriz, lon1, lon2-1, visit, extra_data);
}

static void free_mat(Elem_Matriz **mat, int i){
	int k;
	for(k = 0; k < i; k++)
		free(mat[k]);
	free(mat);
}

void LCS(void *sec1,void *sec2,size_t size,int lon1,int lon2,CompareFunc compare,VisitorLCSFunc visit,void *extra_data){

	/*creo la matriz de "lon1"+1 columnas y "lon2"+1 filas */
	Elem_Matriz **mat = (Elem_Matriz **)malloc((lon1+1)*sizeof(Elem_Matriz *));
	int i, j, k;

	/* la inicializo en cero */
	for(i=0;i<=lon1;i++){
		mat[i] = (Elem_Matriz *)malloc((lon2+1)*sizeof(Elem_Matriz));
		mat[i][0].lon = 0;
	}
	for(i=1;i<=lon2;i++)
		mat[0][i].lon = 0;
/*
*	En nuestro caso:
*	sec1 <--- char **lines_original
*	sec2 <--- char **lines_new
*/
	for(i = 1; i <= lon1; i++){
  		for(j = 1; j <= lon2; j++){
			if(compare((sec1 + (i-1)*size), (sec2 + (j-1)*size))){
				mat[i][j].lon = mat[i-1][j-1].lon + 1;
				mat[i][j].mov = DIAGONAL;
			} else if (mat[i-1][j].lon >= mat[i][j-1].lon){
				mat[i][j].lon = mat[i-1][j].lon;
				mat[i][j].mov = IZQUIERDA;
			} else {
				mat[i][j].lon = mat[i][j-1].lon;
				mat[i][j].mov = ARRIBA;
			}
		}
	}

	LCS_internal(sec1, size, mat, lon1, lon2, visit, extra_data);
	free_mat(mat, lon1 + 1);
}
