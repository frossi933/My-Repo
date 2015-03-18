#include <stdio.h>

#include "Diff.h"
#include "Imprimir.h"
#include "LCS.h"
#include "Error.h"

int main(int argc, char **argv){

	FILE	*original, *new;

	if(argc <= 2){
		error(INCORRECT_COMMAND, "diff", NULL);
		return 1;
	}

	/* abro primer archivo */
	if((original = fopen(argv[1], "r")) == NULL){ 
		printf("No se puede abrir el primer archivo\nTexto incorrecto... Ingrese nuevamente el archivo original: ");
		scanf("%s", argv[1]);
		if((original = fopen(argv[1], "r")) == NULL){
			error(CAN_NOT_OPEN_FILE, "diff", argv[1]);
			return 1;
		}
	}

	/* abro segundo archivo */
	if((new = fopen(argv[2],"r")) == NULL){	
		printf("No se puede abrir el segundo archivo\nTexto incorrecto... Ingrese nuevamente el archivo nuevo: ");
		scanf("%s", argv[2]);
		if((new = fopen(argv[2], "r")) == NULL){
			error(CAN_NOT_OPEN_FILE, "diff", argv[2]);
			fclose(original);
			return 1;
		}
	}

	/* verifico el comando */
	if(argc == 3)
		my_diff(original, new, stdout);
	else if(argc == 4) {
		FILE *salida = fopen(argv[3],"w+");
		if(salida){
			my_diff(original, new, salida);
			fclose(salida);
		} else
			/* por defecto uso la salida estandar para mostrar las diferencias */
			my_diff(original, new, stdout);
	} else {
		error(INCORRECT_COMMAND, "diff", NULL);
		fclose(original);
		fclose(new);
		return 1;
	}		

	fclose(original);
	fclose(new);
	return 0;
}