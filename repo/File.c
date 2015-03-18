#include "../Diff.h"
#include "Common.h"
#include "Messages.h"
#include "Path.h"
#include "Tree.h"
#include "File.h"

#include <stdio.h>
#include <stdlib.h>

#define TMP_FILE		".tmp"

void make_patch(int from, int to, char *path){

	char diffs_path[LENGTH_PATH], source1[LENGTH_PATH], source2[LENGTH_PATH], *father;
	char comm_patch[MAX_LENGTH_OF_LINE];
	int i;
	FILE *i_diff, *tmp_file;

	father = get_father_element(path);
	for(i = from; i <= to; i++){
			/* obtengo el path de las diffs de revisiones anteriores */
			sprintf(diffs_path, FMT_COMMIT(father, i, get_element_from_path(path)));
		
			if((i_diff = fopen(diffs_path, "r")) == NULL)	//si no existe el fichero continuo probando...
				continue;

			/* aplico el parche de las diferencias q estan en "prev_diff" */
			strcpy(source1, TMP_FILE);		//archivo donde voy a ir armando el del ultimo commit
			strcpy(source2, diffs_path);
			sprintf(comm_patch, "patch %s %s>/dev/null", source1, source2);	//lo guarda en source1
			/* realizamos el patch */
			system(comm_patch);
			fclose(i_diff);
		}
	free(father);
}

void create_diffs(void *_node, void *_new_rev){

	TreeNode *node = (TreeNode *)_node;

	if(node->is_dir || status(node) == UPTOD)
		/* si el nodo es un dir o esta en su version actual, no hago nada */
		return;

	int new_rev = *(int *)_new_rev;

	if(status(node) == MOD){

		char res_diff_path[LENGTH_PATH], new_diff_path[LENGTH_PATH], *father;
		FILE *tmp, *res_diff, *new_diff, *file;

		/* realizo las parches para obtener el archivo del commit anterior, el resultado esta en TMP_FILE */
		make_patch(1, new_rev - 1, name(node));

		/* obtengo el path del nuevo archivo de diferencias */
		father = get_father_element(name(node));
		sprintf(new_diff_path, FMT_COMMIT(father, new_rev, get_element_from_path(name(node))));
		
		/*en res_diff tengo el archivo del commit anterior */
		res_diff = fopen(TMP_FILE, "r");		//resultado de make_patch
		new_diff = fopen(new_diff_path, "w+");	//nuevo archivo de diff q creamos
		file = fopen(name(node), "r");		//archivo actual del nodo

		if(res_diff == NULL || new_diff == NULL)
			/* en caso de algun fallo */
			fprintf(stderr, "ALERTA! error de patch. %s\n", name(node));
		else{
			my_diff(res_diff, file, new_diff);
			fclose(res_diff);
			fclose(new_diff);
		}
		remove(TMP_FILE);
		fclose(file);
		free(father);
	} else {
	/* status(node) == UNT */
		char diff_path[LENGTH_PATH], *father;
		FILE *diff_file, *file, *empty;

		father = get_father_element(name(node));
		sprintf(diff_path, FMT_COMMIT(father, new_rev, get_element_from_path(name(node))));

		diff_file = fopen(diff_path, "w+");	// creamos el archivo de diff
		file = fopen(name(node),"r");		// abrimos el archivo del nodo
		empty = fopen("/dev/null","r");		// archivo vacio con el cual comparar

		if(diff_file == NULL || file == NULL)
			/* en caso de algun fallo */
			fprintf(stderr, "ALERTA! error de patch. %s\n", name(node));
		else {
			my_diff(empty, file, diff_file);
			fclose(file);
			fclose(diff_file);
		}
		fclose(empty);
		free(father);
	}
}

void show_diffs(char *path, int old_rev){

	int i;
	FILE *res_patch, *current_file;

	/* creo el archivo temporal para el patch */
	FILE *tmp = fopen(TMP_FILE, "w+");
	fclose(tmp);

	make_patch(1, old_rev, path);	// resultado en TMP_FILE
	res_patch = fopen(TMP_FILE, "r");
	current_file = fopen(path, "r");
	
	message_print_diffs(res_patch, current_file, path);

	fclose(res_patch);
	fclose(current_file);
	remove(TMP_FILE);
}

void compare_show_files(void *_node, void *_index_path){

	TreeNode *node = (TreeNode *)_node;
	char *index_path = (char *)_index_path;

	/* Obtengo el path relativo del archivo y lo muestro */
	char relative[LENGTH_PATH];
	get_relative_path(name(node), index_path, relative);
	if(strcmp(name(node), index_path))
		printf("Solo en %s: %s\n", index_path, relative);
}