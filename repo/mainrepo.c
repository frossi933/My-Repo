#include "../Error.h"
#include "my-repo.h"

#include <stdio.h>
#include <stdlib.h>

#define NUM_OF_COMMAND		8		//cant de comandos que acepta my-repo
#define MAX_SIZE_OF_COMMAND	15		//tamaño maximo del largo de los comandos

int main (int argc, char **argv){

	char comandos[NUM_OF_COMMAND][MAX_SIZE_OF_COMMAND] = {
		"init",
		"add",
		"rm",
		"status",
		"commit",
		"log",
		"compare",
		"help"
	};

	if(argc <= 1){
		printf("==> Comando Incorrecto! Ingrese help para mas informacion.\n");
		return 1;
	}

	if(!strcmp(argv[1], comandos[0])){
		if(argc == 2){
			my_repo_init();
			return 0;
		} else {
			error(INCORRECT_COMMAND, "init", NULL);
			return 1;
		}
	}

	if(!strcmp(argv[1], comandos[1])){
		if(argc == 2){
			error(INCORRECT_COMMAND, "add", NULL);
			return 1;
		} else {
			int i;
			for(i = 2; i < argc; i++)
				my_repo_add(argv[i]);
			return 0;
		}
	}

	if(!strcmp(argv[1], comandos[2])){
		if(argc == 2){
			error(INCORRECT_COMMAND, "remove", NULL);
			return 1;
		} else {
			int i;
			for(i = 2; i < argc; i++)
				my_repo_rm(argv[i]);
			return 0;
		}
	}

	if(!strcmp(argv[1],comandos[3])){
		if(argc == 2){
			my_repo_st();
			return 0;
		} else {
			error(INCORRECT_COMMAND, "status", NULL);
			return 1;
		}
	}

	if(!strcmp(argv[1], comandos[4])){
		if(argc == 4){	
			/* verifico que el comando haya sido ingresado correctamente */
			if(strcmp(argv[2], "-m")){
				error(INCORRECT_COMMAND, "commit", NULL);
				return 1;
			}
			
			my_repo_co(argv[3]);
			return 0;
		} else {
			error(INCORRECT_COMMAND, "commit", NULL);
			return 1;
		}
	}

	if(!strcmp(argv[1], comandos[5])){
		if(argc == 2){
			my_repo_log(NULL);
			return 0;
		} else if(argc == 3) {
			my_repo_log(argv[2]);
			return 0;
		} else {
			error(INCORRECT_COMMAND, "log", NULL);
			return 1;
		}
	}

	if(!strcmp(argv[1], comandos[6])){
		if(argc == 4){
			my_repo_comp(argv[2], argv[3]);
			return 0;
		} else {
			error(INCORRECT_COMMAND, "compare", NULL);
			return 1;
		}
	}

	if(!strcmp(argv[1], comandos[7])){
		int i;
		for(i = 0; i < NUM_OF_COMMAND; i++){
			print_usage(comandos[i]);
			printf("\n");
		}
		return 0;
	}

	printf("==> Comando Incorrecto! Ingrese help para mas informacion.\n");
	return 1;
}
