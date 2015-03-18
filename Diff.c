#include "Diff.h"

void visit_lines(void *data,int i1,int i2,void *extra_data){
	SET(((Ind *)extra_data)->indicador1, i1);
	SET(((Ind *)extra_data)->indicador2, i2);
}

int compare_lines(void *a, void *b){
	if(!strcmp(*(char **)a, *(char **)b))
		return 1;
	return 0;
}

void	my_diff(FILE *original, FILE *new, FILE *salida){

	char 	**lines = (char **)malloc(INITIAL_NUMBER_OF_LINES * sizeof(char *));
	int		i = 0, j, k;
	int		allocated_lines = INITIAL_NUMBER_OF_LINES;
	int		lines_n = 0, lines_n1 = 0, lines_n2 = 0;

	char	*line;
	size_t 	max_length = LENGTH_LINE;
	int 		bytes_read;

	line = NULL;
	bytes_read = getline(&line, &max_length, original);

	while(bytes_read != -1){
		/*verifico el tamaño de lines*/
		if(lines_n == allocated_lines){
			/* agrando el espacio que le corresponde a lines, para seguir guardando las lineas del archivo */
			lines = (char **)realloc(lines, sizeof(char *)*(allocated_lines+STEP_BUF));
			allocated_lines+=STEP_BUF;
		}

		/* guardo la lenia en lines*/
		lines[lines_n] = line;
		lines_n++;

		/*leo otra linea de original*/
		line = NULL;
		bytes_read = getline(&line, &max_length, original);
	}

	free(line);	//libero el ultimo bloque que fue leido por getline y devolvio -1

	lines_n1 = lines_n;

	line = NULL;
	bytes_read = getline(&line, &max_length, new);

	while(bytes_read != -1){
		/*verifico el tamaño de lines*/
		if(lines_n == allocated_lines){
			/* agrando el espacio que le corresponde a lines, para seguir guardando las lineas del archivo */
			lines=(char **)realloc(lines,sizeof(char *)*(allocated_lines+STEP_BUF));
			allocated_lines+=STEP_BUF;
		}

		/* guardo la linea en lines*/
		lines[lines_n] = line;
		lines_n++;

		/*leo otra linea de original*/
		line = NULL;
		bytes_read = getline(&line, &max_length, new);	
	}
	free(line);	//libero el ultimo bloque que fue leido por getline y devolvio -1

	/* achico el arreglo, sacando el espacio no utilizado */
	lines = (char **)realloc(lines, lines_n * sizeof(char *));
	allocated_lines = lines_n;
	lines_n2 = lines_n - lines_n1;

	/* Inicializo los indicadores en cero */
	Ind *field = (Ind *)malloc(sizeof(Ind));
	field->indicador1 = (unsigned char *)calloc((lines_n1/8) + 1, sizeof(unsigned char));
	field->indicador2 = (unsigned char *)calloc((lines_n2/8) + 1, sizeof(unsigned char));

	/* obtengo la subsecuencia mas larga entra las lineas, seteando los indicadores segun las lineas en comun */
	LCS(lines, &lines[lines_n1], sizeof(char *), lines_n1, lines_n2, compare_lines, visit_lines, field);

	/* imprimo las diferencias */
	Imprimir_diff_lines(lines, field, lines_n1, lines_n2, salida);

	/*libero memoria*/
	free(field->indicador1);
	free(field->indicador2);
	free(field);
	for(i = 0; i<lines_n1 + lines_n2; i++)
		free(lines[i]);
	free(lines);
}