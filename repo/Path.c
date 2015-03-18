#include "Common.h"
#include "Tree.h"
#include "SList.h"
#include "Path.h"
#include "../Error.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>


int	exist_index_up(char *path, char *result){

	char *aux_path, tmp_path[LENGTH_PATH];
	aux_path = getenv("PWD");
	getcwd(tmp_path, LENGTH_PATH);

	/* verificamos que no existe en alguno de sus directorios padres */
	while(strcmp(tmp_path, "/")){
		chdir("..");
		getcwd(tmp_path, LENGTH_PATH);

		DIR *dir = opendir(tmp_path);
		struct dirent *direntp;

		while((direntp = readdir(dir)) != NULL){
			if(!strcmp(direntp->d_name, INDEX)){
				strcpy(result, tmp_path);
				closedir(dir);
				return 1;
			}
		}
		closedir(dir);
	}
	/* termino sin haber encontrado el directorio INDEX */
	chdir(aux_path);
	result = NULL;
	return 0;
}


int	exist_index_down(char *path, char *result){
	 
	DIR *current_dir = opendir(path);
	if(current_dir == NULL)
		return 0;

	struct dirent *direntp;
	/* verificamos que exista en los archivos contenidos en "path" y en cada subdirectorio */
	while((direntp = readdir(current_dir)) != NULL){
		if(!strcmp(direntp->d_name, INDEX)){
			strcpy(result, path);
			closedir(current_dir);
			return 1;
		}
		
		char new_path[LENGTH_PATH];
		sprintf(new_path, "%s/%s", path, direntp->d_name);

		if(is_dir(new_path)){
			if(!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, ".."))
				continue;

			if(exist_index_down(new_path, result)){
				closedir(current_dir);
				return 1;
			}
		}
	}
	/* terminamos sin haber encontrado dicho directorio */
	closedir(current_dir);
	result = NULL;
	return 0;
}

inline char *get_element_from_path(char *path_complete){
	return strrchr(path_complete,'/') + 1;
}

inline char *name_of_son(TreeNode *parent){
	return name((TreeNode *)slist_data(children(parent)));
}

char *get_father_element(char *path_comp){
	char *ret = (char *)calloc(LENGTH_PATH, sizeof(char));
	char element[LENGTH_PATH];
	strcpy(element, get_element_from_path(path_comp));
	
	/* copio en ret la cantidad de caracteres de path_comp menos los de element, y resto uno mas para no copiar el caracter '/' */
	memcpy(ret, path_comp, strlen(path_comp) - strlen(element) - 1);
	return ret;
}

int	get_path_complete(char *element, char *result){

	char *current;
	char *path_father;

	current = getenv("PWD");
	if(strcmp(element, ".")) {
		if(strcmp(element, "..")){
			if(element[0] == '.'){
				error(HIDDEN_ELEM, "add", element);
				return 0;
			} else
				sprintf(result, "%s/%s", current, element);		
		} else {
			path_father = get_father_element(current);
			strcpy(result, path_father);
			free(path_father);
		}
	} else 
		strcpy(result, current);
	return 1;
}

int exist_elem(char *path){
	if(is_dir(path)){
		DIR *dir = opendir(path);
		if(dir == NULL)
			return 0;
		else {
			closedir(dir);
			return 1;
		}
	} else {
		FILE *file = fopen(path, "r");
		if(file == NULL)
			return 0;
		else {
			fclose(file);
			return 1;
		}
	}
}

inline void get_relative_path(char *path_complete, char *index_path, char *result){
	int n = strlen(index_path);
	/* copio en result la cadena path_complete pero comenzando desde el caracter numero n+1 para obtener el path relativo */
	strcpy(result, path_complete + n + 1);
}


void *verify_and_get_paths(char *index_path, char *contents_path, char *rev_path){

	char *path;
	path = getenv("PWD");

	/*verifico que exista INDEX */
	if(!exist_index_up(path, index_path)){
		if(!exist_index_down(path, index_path)){
			return NULL;
		}
	}

	/*obtengo la ruta completea del fichero contents y rev */
	sprintf(contents_path, "%s/%s/contents", index_path, INDEX);
	sprintf(rev_path, "%s/%s/rev", index_path, INDEX);
}

inline void get_rev_path(int nro_rev, char *index_path, char *result){
	sprintf(result, FMT_REV(index_path, nro_rev));
}

inline int is_dir(const char *path){
	struct stat bf;
	if(stat(path, &bf) != 0)
		return 0;
	
	if(S_ISDIR(bf.st_mode))
		return 1;
	else
		return 0;
}