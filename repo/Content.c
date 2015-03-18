#include "Content.h"
#include "Path.h"
#include "Common.h"
#include "../Error.h"

#include <string.h>
#include <stdio.h>
#include <time.h>


void get_number_of_rev(char *rev_path, int *rev){
	FILE *rev_file;
	rev_file = fopen(rev_path, "r");
	fscanf(rev_file, "%d", rev);
	fclose(rev_file);
}

int 	elem_in_contents(char *path, char *contents_path){
	int number_of_line = 0;
	char line[MAX_LENGTH_OF_LINE];
	char aux_path[LENGTH_PATH];
	sprintf(aux_path, "%s\n", path);	//agrego el fin de linea a path para poder compararlo
	FILE *file = fopen(contents_path, "r");

	while(fgets(line, MAX_LENGTH_OF_LINE, file)){
		if(!strcmp(aux_path, line)){
			fclose(file);
			return number_of_line;
		}
		number_of_line++;
	}	
	fclose(file);
	return -1;
}

static void rev_print_node(void *_tree_node, void *_rev){
	TreeNode *tree_node = (TreeNode *)_tree_node;
	FILE *rev = (FILE *)_rev;

	int i;
	/* Imprimo el nombre del archivo del nodo correspondiente */
	fprintf(rev, "%s ", name(tree_node));
	/* Imprimo el hash del nodo correspondiente, en base hexadecimal */
	for(i = 0; i < HASH_SIZE; i++)
		fprintf(rev, "%02x", hash_of(tree_node)[i]);
	fprintf(rev, "\n");
}

static inline void content_print_node(void *_tree_node,void *_contents){
	/* Imprimo el nombre del archivo del nodo correspondiente */
	fprintf((FILE *)_contents, "%s\n", name((TreeNode *)_tree_node));
}

static void print_time(FILE *contents){
	time_t	now;
	struct tm *tiempo;
	now = time(NULL);
	tiempo = gmtime(&now);
	fprintf(contents, "%s", asctime(tiempo));
}

int 	content_init(char *path) {

	FILE *contents, *rev_file;

	/*creo el fichero "contents"*/
	if((contents = fopen("contents","w+")) == NULL){
		error(COULD_NOT_CREATE_FILE, "init", "contents");
		chdir("..");
		remove(INDEX);
		return 1;
	}

	/*escribo la hora y fecha de creacion en "contents"*/
	print_time(contents);

	/*escribo la raiz*/
	fprintf(contents,"%s\n",path);
	
	fclose(contents);

	/* creo el fichero "rev" que contendra el numero de revision actual */
	if((rev_file = fopen("rev", "w+")) == NULL){
		error(COULD_NOT_CREATE_FILE, "init", "rev");
		remove("contents");
		chdir("..");
		remove(INDEX);
		return 1;
	}

	/* escribo el numero de revision actual: 0 */
	fprintf(rev_file, "0");	
	fclose(rev_file);
	return 0;
}

int 	print_tree(TreeNode *root, char *path_file, ConstructionType type, char *mes){
	
	if(type == WITHOUT_EXTRA){
	/* file = contents */
		FILE *file = fopen(path_file, "r");
		if(file == NULL){
			fprintf(stderr, "ALERTA! problemas para abrir el archivo %s.\n", path_file);
			return 1;
		}

		/*obtengo la hora de creacion q se encuentra en el archivo anterior de contents */
		char time[MAX_LENGTH_OF_LINE];
		fgets(time, MAX_LENGTH_OF_LINE, file);
		fclose(file);

		/*sobreescribo el mismo archivo "contents"*/
		FILE *new_contents = fopen(path_file, "w+");
		fprintf(new_contents, "%s", time);

		/* Imprimo el nombre de cada archivo en el arbol */
		tree_node_foreach_node(root, content_print_node, NODE_FIRST_ORDER, new_contents);
		fclose(new_contents);

		return 0;
	} else {
	/* file es un archivo de revision */
		FILE *new_rev = fopen(path_file, "w+");
	
		/* Imprimo el tiempo y el mensaje en el archivo "new_rev" */
		print_time(new_rev);
		fprintf(new_rev, "%s\n", mes);
				
		/* Imprimo el nombre de cada archivo en el arbol */
		tree_node_foreach_node(root, rev_print_node, NODE_FIRST_ORDER, new_rev);
		fclose(new_rev);
		return 0;
	}
}

void update_rev(char *rev_path, int new_rev){
	FILE *rev_file = fopen(rev_path, "w+");
	char rev_str[10];

	sprintf(rev_str, "%d", new_rev);
	fprintf(rev_file, "%s", rev_str);
	fclose(rev_file);
}