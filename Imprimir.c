#include "Imprimir.h"

static inline void print_deleted_lines(FILE *salida, char **lines, int from, int to){
	while(from <= to){
		fprintf(salida, "< %s", lines[from]);
		from++;
	}
}

static inline void print_added_lines(FILE *salida, char **lines, int from, int to){
	while(from <= to){
		fprintf(salida, "> %s", lines[from]);
		from++;
	}
}

static inline void print_changed_lines(FILE *salida, char **lines, int from1, int to1, int from2, int to2){
	while(from1 <= to1){
		fprintf(salida, "< %s", lines[from1]);
		from1++;
	}
	fprintf(salida, "---\n");
	while(from2 <= to2){
		fprintf(salida, "> %s", lines[from2]);
		from2++;
	}
}


void Imprimir_diff_lines(char **lines, Ind *field, int lines_n1, int lines_n2, FILE *salida){

	int i = 0, j = 0, k, l, ant1 = -1, ant2 = -1;
	int salir = 0;

	if(lines_n1 == 0){	//primer archivo vacio
		if(lines_n2 == 0)	// segundo archivo vacio
			return;
		else {
			/*agregue lineas a "new"*/
			if(lines_n2 > 1)
				/*agregue mas de una linea */
				fprintf(salida,"%da%d,%d\n", i, 1, lines_n2);
			else
				/* agregue solo una linea */
				fprintf(salida,"%da%d\n", i, 1);

			print_added_lines(salida, lines, 0, lines_n2 - 1);
			return;
		}
	} else if(lines_n2 == 0){	//segundo archivo vacio
		/*elimine lineas de "original"*/
		if(lines_n1 > 1)
			/* elimine mas de una linea */
			fprintf(salida, "%d,%dd%d\n", 1, lines_n1, j);
		else
			/* elimine solo una linea */
			fprintf(salida, "%dd%d\n", 1, j);

		print_deleted_lines(salida, lines, 0, lines_n1 - 1);
		return;
	}

	while(!salir){
		while(i < lines_n1){
			if(IS_ONE(i, field->indicador1)){
				k = i - ant1 - 1;	// k <-- cant de ceros consecutivos en indicador1
				i++;
				break;
			}
			i++;
		}
	
		while(j < lines_n2){
			if(IS_ONE(j, field->indicador2)){
				l = j - ant2 - 1;	// l <-- cant de ceros consecutivos en indicador2
				j++;
				break;
			}
			j++;
		}
		
		if(i == lines_n1 && j == lines_n2){
			if(!IS_ONE(i - 1, field->indicador1) || !IS_ONE(j - 1, field->indicador2)){
				k = i - ant1 - 1;
				l = j - ant2 - 1;
			} else {
				k = 0;
				l = 0;
			}
			salir = 1;
		}

/*NOTA: ant1 y ant2 indican la posicion donde se encuentra el bit anterior mas proximo en uno, estas
 posiciones arrancan desde uno, por lo tanto al imprimir el numero de linea equivalente debemos sumarle uno*/

		if(k == 0 && l == 0){
			ant1 = i - 1;	// guardo la pos donde se encontro el uno anterior
			ant2 = j - 1;	// guardo la pos donde se encontro el uno anterior
			continue;
		}
		if(k == 0  &&  l > 0){
			/* agregue lineas nuevas en el archivo "new" */
			if(l > 1)
				fprintf(salida,"%da%d,%d\n", ant1 + 1, ant2 + 2, (ant2 + 1) + l);
			else
				fprintf(salida,"%da%d\n", ant1 + 1, (ant2 + 1) + l);

			print_added_lines(salida, lines, lines_n1 + ant2 + 1, lines_n1 + ant2 + l);
			ant1 = i - 1;
			ant2 = j - 1;
			continue;
		}
		if(k > 0  &&  l == 0){
			/* elimine lineas del archivo "original" */
			if(k > 1)
				fprintf(salida, "%d,%dd%d\n", ant1 + 2, ant1 + 1 + k, j-1);
			else
				fprintf(salida, "%dd%d\n", ant1 + 1 + k, j-1);

			print_deleted_lines(salida, lines, ant1 + 1, ant1 + k);
			ant1 = i - 1;
			ant2 = j - 1;
			continue;
		} else {/* 	k > 0  &&  l > 0	 */
			/* reemplace lineas del archivo "original" */
			if(k > 1){
				if(l > 1)
					fprintf(salida, "%d,%dc%d,%d\n",ant1+2,ant1+1+k,ant2+2,ant2+1+l);
				else
					fprintf(salida, "%d,%dc%d\n",ant1 + 2, ant1 + 1 + k, ant2 + 1 + l);
			} else {
				if(l > 1)
					fprintf(salida, "%dc%d,%d\n",ant1 + 1 + k, ant2 + 2,ant2 + 1 + l);
				else
					fprintf(salida,"%dc%d\n",ant1 + 1 + k, ant2 + 1 + l);
			}

			print_changed_lines(salida,lines,ant1+1,ant1+k,lines_n1+ant2+1,lines_n1+ant2+l);
			ant1 = i - 1;
			ant2 = j - 1;
			continue;
		}
	}	
}
