#include "Error.h"

#include <stdio.h>

void print_usage(char *comando){
	if(!strcmp(comando, "diff")){
		printf("Uso: <my-diff>\nDescripcion: Compara los archivos \"file1\" y \"file2\" linea por linea, mostrando sus diferencias.\"file3\" indica al lugar donde imprimir dichas diferencias, en caso de oitirse, se toma stdout por defecto.\nComando: my-diff <file1> <file2> [<file3>]\n");
		return;
	}

	if(!strcmp(comando,"init")){
		printf("Uso: <init>\nDescripcion: inicializa el directorio actual para control de versiones bajo my-repo.\nComando: my-repo init\n");
		return;
	}

	if(!strcmp(comando,"add")){
		printf("Uso: <add>\nDescripcion: agrega el o los archivos \"file\" al repositorio(los archivos deben ser ingresados siempre de forma relativa).\nComando: my-repo add <file1> [ <file2> [ ... ] ]\n");
		return;
	}

	if(!strcmp(comando,"rm")){
		printf("Uso: <rm>\nDescripcion: remueve el o los archivos \"file\" del repositorio(los archivos deben ser ingresados siempre de forma relativa).\nComando: my-repo rm <file1> [ <file2> [ ... ] ]\n");
		return;
	}

	if(!strcmp(comando,"status")){
		printf("Uso: <status>\nDescripcion: detalla el estado actual de cada archivo.\nComando: my-repo status\n");
		return;
	}

	if(!strcmp(comando,"commit")){
		printf("Uso: <commit>\nDescripcion: actualiza los cambios hecho en el repositorio.\nComando: my-repo commit -m <message>\n");
		return;
	}

	if(!strcmp(comando,"log")){
		printf("Uso: <log>\nDescripcion: en caso de no introducir argumento muestra el historial de las revisiones. En caso contrario muestra las diferencias entre la revision \"rev\" y la actual.\nComando: my-repo log [ <rev> ]\n");
		return;
	}

	if(!strcmp(comando,"compare")){
		printf("Uso: <compare>\nDescripcion: muestra las diferencias entre cada uno de las archivos de \"rep1\" con los de \"rep2\"(los archivos deben ser ingresados siempre de forma relativa).\nComando: my-repo compare <rep1> <rep2>\n");
		return;
	}
}

void error(ErrorType type, char *comando, char *extra){

	fprintf(stderr, "==> Error\n==> ");
	switch(type){
		case REPO_EXISTS:	
			fprintf(stderr, "Ya existe un repositorio, con raiz en \"%s\".\n", extra);
			break;
		case NOT_WITHIN_A_REPO: 
			fprintf(stderr, "No se encuentra dentro de un repositorio.\n");
			break;
		case ELEM_IS_NOT_IN_THE_REPO:
			fprintf(stderr, "El elemento \"%s\" NO se encuentra bajo control de versiones.\n", extra);
			break;
		case ELEM_IS_IN_THE_REPO: 
			fprintf(stderr, "El elemento \"%s\" ya se encuentra bajo control de versiones.\n", extra);
			break;
		case PARENT_DIR_IS_NOT_IN_THE_REPO: 
			fprintf(stderr, "El elemento \"%s\" NO pertence a un directorio bajo control de versiones.\n", extra);
			break;
		case ELEM_IS_THE_ROOT: 
			fprintf(stderr, "El elemento a remover es la raiz del repositorio.\n");
			break;
		case COULD_NOT_CREATE_FILE:
			fprintf(stderr, "No se pudo crear el archivo \"%s\".\n", extra);
		case DIR_IS_NOT_IN_THE_REPO: 
			fprintf(stderr, "El directorio actual NO se encuentra bajo control de versiones.\n");
			break;
		case ELEM_DOES_NOT_EXIST: 
			fprintf(stderr, "El elemento \"%s\" no existe.\n", extra);
			break;
		case DIR_IS_NOT_THE_ROOT: 
			fprintf(stderr, "El directorio actual NO es la raiz del repositorio.\n");
			break;
		case INCORRECT_COMMAND:
			fprintf(stderr, "El comando \"%s\" ingresado no es correcto.\n", comando);
			fprintf(stderr, "==> %s abortado.\n", comando);
			print_usage(comando);
			return;
		case CAN_NOT_OPEN_FILE:
			fprintf(stderr, "No se puede abrir el archivo \"%s\"\nTexto incorrecto...\n", extra);
			break;
		case REPO_NOT_CHANGED:
			fprintf(stderr, "El repositorio no ha sufrido cambios.\n");
			break;
		case HIDDEN_ELEM:
			fprintf(stderr, "%s no ha sido agregado ya que no se puede agregar elementos ocultos al repositorio.\n", extra);
			break;
		case WRONG_REV_NUMBER:
			fprintf(stderr, "El numero de revision ingresado no corresponde a uno perteneciente a este repositorio.\n");
			break;
		case DIR_IS_NOT_A_REPO:
			fprintf(stderr, "El directorio \"%s\" no es la raiz de un repositorio valido.\n", extra);
			break;
		case OTHER:
			fprintf(stderr, "Problema de origen desconocido.\n");
			break;
	}

	fprintf(stderr, "==> %s abortado.\n", comando);
}
