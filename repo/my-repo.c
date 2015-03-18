#include "../Diff.h"
#include "my-repo.h"
#include "Content.h"
#include "Hash.h"
#include "Tree.h"
#include "Common.h"
#include "Path.h"
#include "../Error.h"
#include "Messages.h"
#include "File.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>


void my_repo_init (){

	char *path;
	char index_path[LENGTH_PATH];
	path = getenv("PWD");			// directorio actual de trabajo

	/*verifico que NO exista INDEX */
	if(exist_index_up(path, index_path)){
		error(REPO_EXISTS, "init", index_path);
		return;
	}
	if(exist_index_down(path, index_path)){	
		error(REPO_EXISTS, "init", index_path);
		return;
	}

	/*creo el directorio INDEX*/
	mkdir(INDEX, S_IRWXU);
	chdir(INDEX);

	/* inicializamos el repositorio */
	if(content_init(path)){
		/* en caso de error */
		chdir(path);
		return;
	}

	chdir(path);
	message_init_done();
}

void my_repo_add(char *element){

	char index_path[LENGTH_PATH], path_complete[LENGTH_PATH], *path_father;
	char contents_path[LENGTH_PATH], rev_path[LENGTH_PATH];
	int pos_elem;

	if(!verify_and_get_paths(index_path, contents_path, rev_path)){
		error(NOT_WITHIN_A_REPO, "add", NULL);
		return;
	}

	if(element[strlen(element) - 1] == '/')
		element[strlen(element) - 1] = '\0';	// en caso de tener un '/' al final, lo elimino
	
	if(!get_path_complete(element, path_complete))
		return;

	if(!exist_elem(path_complete)){
		error(ELEM_DOES_NOT_EXIST, "add", path_complete);
		return;
	}

	/* Busco si "element" se encuentre en contents*/
	pos_elem = elem_in_contents(path_complete, contents_path);

	TreeNode *root = NULL, *new_tree = NULL;

	if(pos_elem == -1){	/* el elemento no se encuentra bajo control de versiones */
		/* verifico que "element" pertenezca a una carpeta bajo control de versiones */
		path_father = get_father_element(path_complete);
		if(elem_in_contents(path_father, contents_path) == -1){
			error(PARENT_DIR_IS_NOT_IN_THE_REPO, "add", path_complete);
			free(path_father);
			return;
		}

		/* creo el arbol de archivos, a partir de contents, con la opcion WITHOUT_EXTRA que hace q el campo "extra" de la estructura
		sea NULL */
		root = file_to_tree(contents_path, NULL, WITHOUT_EXTRA);
		/* creo el arbol de archivos correspondiente al directorio "path_complete", con la misma opcion anterior */
		new_tree = tree_dir_new(is_dir(path_complete), path_complete, contents_path, WITHOUT_EXTRA);

		/*verifico q se hayan creado los arboles  */
		if(root == NULL || new_tree == NULL){
			error(OTHER, "add", NULL);
			tree_node_destroy(root);
			tree_node_destroy(new_tree);
			free(path_father);
			return;
		}
		/* agrego el nuevo nodo */
		tree_node_add_child(root, path_father, new_tree);
		free(path_father);
	} else {	/* el elemento se encuentra bajo control de versiones */
		new_tree = tree_dir_new(is_dir(path_complete), path_complete, contents_path, WITHOUT_EXTRA);
		/* verifico q se haya creado */
		if(new_tree == NULL){
			error(OTHER, "add", NULL);
			return;
		}
		
		if(pos_elem == 1) /* es la raiz del repositorio */
			root = new_tree;
		else { /* no es la raiz, entonces creo el arbol de archivos a partir de "contents", elimino el nodo anterior y agrego el nuevo */
			root = file_to_tree(contents_path, NULL, WITHOUT_EXTRA);
			tree_node_remove(root, path_complete);
			path_father = get_father_element(path_complete);
			tree_node_add_child(root, path_father, new_tree);
			free(path_father);
		}
	}

	tree_node_foreach_node(new_tree, message_adding_node, NODE_FIRST_ORDER, contents_path);

	/* imprimo el arbol ya armado en el archivo "contents" */
	if(print_tree(root, contents_path, WITHOUT_EXTRA, NULL)){
		/* fallo la funcion print_tree */
		error(OTHER, "add", NULL);
		tree_node_destroy(root);
		return;
	}
	
	tree_node_destroy(root);
	message_add_done();
}

void my_repo_rm(char *element){

	char index_path[LENGTH_PATH], path_complete[LENGTH_PATH];
	char contents_path[LENGTH_PATH], rev_path[LENGTH_PATH];

	FILE *contents;

	if(!verify_and_get_paths(index_path, contents_path, rev_path)){
		error(NOT_WITHIN_A_REPO, "remove", NULL);
		return;
	}

	if(element[strlen(element) - 1] == '/')
		element[strlen(element) - 1] = '\0';	// en caso de tener un '/' al final, lo elimino

	if(!get_path_complete(element, path_complete))
		return;

	/* verifico que exista "element" */
	if(!exist_elem(path_complete)){
		error(ELEM_DOES_NOT_EXIST, "remove", path_complete);
		return;
	} 

	TreeNode *tree;

	if( elem_in_contents(path_complete, contents_path) != -1){
	/* "element" esta en contents */
		/* verifico q el elemento a remover no es el dir raiz */
		if(strcmp(index_path, path_complete)){
			tree = file_to_tree(contents_path, NULL, WITHOUT_EXTRA);
			tree_node_remove(tree, path_complete);
			print_tree(tree, contents_path, WITHOUT_EXTRA, NULL);
		} else {
			error(ELEM_IS_THE_ROOT, "remove", NULL);
			return;
		}
	} else {
	/* "element" no esta en contents */
		error(ELEM_IS_NOT_IN_THE_REPO, "remove", element);
		return;
	}

	/* muestro en pantalla los elementos removidos */
	TreeNode *removed_tree = tree_dir_new(is_dir(path_complete), path_complete, contents_path, WITHOUT_EXTRA);
	tree_node_foreach_node(removed_tree, message_rm_done, NODE_FIRST_ORDER, NULL);

	/* libero la memoria de las arboles */
	tree_node_destroy(removed_tree);
	tree_node_destroy(tree);
}

void my_repo_st(void){

	char *current, index_path[LENGTH_PATH], contents_path[LENGTH_PATH], rev_path[LENGTH_PATH];

	if(!verify_and_get_paths(index_path, contents_path, rev_path)){
		error(NOT_WITHIN_A_REPO, "status", NULL);
		return;
	}

	current = getenv("PWD");		// directorio actual de trabajo

	if(elem_in_contents(current, contents_path) < 0){
	/* el dir actual NO se encuentra bajo control de versiones */
		error(DIR_IS_NOT_IN_THE_REPO, "status", NULL);
		return;
	}

	int rev;
	get_number_of_rev(rev_path, &rev);

	if(rev == 0){	/* todavia no se hizo ningun commit, entonces muestro para cada archivo su estado, el cual puede ser NOC o UNT */
		/* creo el arbol de archivos a partir de "contents" */
		TreeNode *dir_tree = tree_dir_new(1, current, contents_path, WITH_ST_HASH);
		tree_node_foreach_node(dir_tree, message_print_status, NODE_FIRST_ORDER, contents_path);
		tree_node_destroy(dir_tree);
		return;
	}

	/* obtengo el path completo del archivo correspondiente al ultimo commit */
	sprintf(rev_path, FMT_REV(index_path, rev));

	/* armo los arboles a partir de los archivos de revision y de contents */
	TreeNode *hash_tree = file_to_tree(rev_path, contents_path, WITH_HASH);		// hashes obtenidos del archivo
	TreeNode *dir_tree = tree_dir_new(1, current, contents_path, WITH_ST_HASH);	// hashes actuales de los archivos

	/* verifico q se hayan creado los arboles */
	if(hash_tree == NULL || dir_tree == NULL){
		error(OTHER, "status", NULL);
		tree_node_destroy(hash_tree);
		tree_node_destroy(dir_tree);
		return;
	}
	
	/* obtengo el estado de cada archivo y lo muestro en pantalla, aplicando la funcion a la raiz del arbol de directorios
		y a su nodo equivalente en el arbol de hashes */
	set_and_print_status(dir_tree, find_treenode_from_path(hash_tree, current), contents_path, OPTION_PRINT_ON);

	/* libero memoria ocupada por los arboles */
	tree_node_destroy(hash_tree);
	tree_node_destroy(dir_tree);
}

void my_repo_co(char *message){

	char *current, index_path[LENGTH_PATH], contents_path[LENGTH_PATH], rev_path[LENGTH_PATH]; 

	if(!verify_and_get_paths(index_path, contents_path, rev_path)){
		error(NOT_WITHIN_A_REPO, "commit", NULL);
		return;
	}

	current = getenv("PWD");

	if(elem_in_contents(current, contents_path) != 1){
	/*no se encuentra en la primera pos, es decir, no es el directorio raiz */
		error(DIR_IS_NOT_THE_ROOT, "commit", NULL);
		return;
	}

	int rev, new_rev;
	char new_commit_path[LENGTH_PATH], prev_commit_path[LENGTH_PATH];

	get_number_of_rev(rev_path, &rev);
	new_rev = rev + 1;

	/* obtengo el path del nuevo archivo de revision */
	sprintf(new_commit_path, FMT_REV(index_path, new_rev));

	TreeNode *current_tree, *prev_tree;

	if(rev == 0){		/* primer commit */
		/*armo el arbol cuyos nodos son los elementos bajo control, calculando los hashes actuales */
		current_tree = file_to_tree(contents_path, NULL, WITH_ST_HASH);

		if(current_tree == NULL){
			error(OTHER, "commit", NULL);
			return;
		}
		/* creo el archivo de diferencias de cada archivo */
		tree_node_foreach_node(current_tree, create_diffs, NODE_FIRST_ORDER, &new_rev);
	} else {
		/*obtengo el path del ultimo archivo de revision */
		sprintf(prev_commit_path, FMT_REV(index_path, rev));

		/*armo el arbol cuyos nodos son los elementos bajo control, calculando los hashes actuales */
		current_tree = file_to_tree(contents_path, NULL, WITH_ST_HASH);
		/*armo el arbol cuyos nodos son los elementos del commit anterior, tomando los hashes de ese momento */
		prev_tree = file_to_tree(prev_commit_path, contents_path, WITH_HASH);

		if(hash_equals(hash_sthash(current_tree), hash_extra(prev_tree))){
			/*el repositorio no cambio*/
			error(REPO_NOT_CHANGED, "commit", NULL);
			tree_node_destroy(prev_tree);
			tree_node_destroy(current_tree);
			return;
		}

		if(current_tree == NULL || prev_tree == NULL){
			error(OTHER, "commit", NULL);
			tree_node_destroy(prev_tree);
			tree_node_destroy(current_tree);
			return;
		}

		/* comparo los nodos de ambos arboles para obtener los cambios q se efectuaron(usando OPTION_PRINT_OFF para no mostrarlos por pantalla),
		 interpretando el estado:	- UNT como un archivo nuevo, recientemente agregado al repositorio
							- MOD como un archivo modificado q debe ser acttualizado
							- UPTOD como un archivo q no sufrio cambios
		*/
		set_and_print_status(current_tree, prev_tree, contents_path, OPTION_PRINT_OFF);
		tree_node_foreach_node(current_tree, create_diffs, NODE_FIRST_ORDER, &new_rev);
		tree_node_destroy(prev_tree);
	}
	/* imprimo el commit en el archivo y actualizo el numero de rev q esta en contents */
	print_tree(current_tree, new_commit_path, WITH_HASH, message);
	update_rev(rev_path, new_rev);
	tree_node_destroy(current_tree);

	message_commit_done();
}

void my_repo_log(char *nro_rev){

	char *current, index_path[LENGTH_PATH], contents_path[LENGTH_PATH], rev_path[LENGTH_PATH]; 

	current = getenv("PWD");

	if(!verify_and_get_paths(index_path, contents_path, rev_path)){
		error(NOT_WITHIN_A_REPO, "log", NULL);
		return;
	}

	if(elem_in_contents(current,contents_path)<0){
	/* el dir actual NO se encuentra bajo control de versiones */
		error(DIR_IS_NOT_IN_THE_REPO, "log", NULL);
		return;
	}

	if(nro_rev == NULL){
		/* debo mostrar todos los commits hechos en el repositorio */
		int i, rev;
		get_number_of_rev(rev_path, &rev);

		for(i = rev; i > 0; i--){
			char i_commit_path[LENGTH_PATH];
			FILE *i_commit;

			/* obtengo el path de cada archivo de revision */
			get_rev_path(i, index_path, i_commit_path);
			i_commit = fopen(i_commit_path, "r");

			if(i_commit == NULL){
				error(CAN_NOT_OPEN_FILE, "log", i_commit_path);
				return;
			}

			show_time_and_message(i_commit, i);
			fclose(i_commit);
		}
	} else {
		/* debo mostrar archivo por archivo las diferencias entre el repositorio como estuvo en el commit dado y el actual */
		char old_commit_path[LENGTH_PATH];
		int old_rev = atoi(nro_rev);
		int last_rev;

		get_number_of_rev(rev_path, &last_rev);

		if(old_rev <= 0 || old_rev > last_rev){
			error(WRONG_REV_NUMBER, "log", NULL);
			return;
		}

		get_rev_path(old_rev, index_path, old_commit_path);

		/* creo el arbol perteneciente al commit del numero de rev dado como parametro, con la opcion WITH_HASH para tener los hashes contenidos en el archivo de revision */
		TreeNode *old_tree = file_to_tree(old_commit_path, contents_path, WITH_HASH);
		/* creo el arbol correspondiente al archivo "contents", usando WITH_CURRENT_HASH para contar con los hashes actuales de cada archivo */
		TreeNode *current_tree = file_to_tree(contents_path, NULL, WITH_CURRENT_HASH);

		if(current_tree == NULL || old_tree == NULL){
			error(OTHER, "commit", NULL);
			tree_node_destroy(old_tree);
			tree_node_destroy(current_tree);
			return;
		}

		/* busco y muestro en pantalla las modificaciones entre los archivos */
		search_and_show_mod_in_tree(old_tree, current_tree, old_rev);

		/* libero memoria */
		tree_node_destroy(old_tree);
		tree_node_destroy(current_tree);
	}
}


void my_repo_comp(char *index1, char *index2){

	char path_complete1[LENGTH_PATH], path_complete2[LENGTH_PATH];
	char index_path1[LENGTH_PATH], index_path2[LENGTH_PATH];
	get_path_complete(index1, path_complete1);
	get_path_complete(index2, path_complete2);

	/*verifico que index1 sea un repositorio */
	if(!exist_index_up(path_complete1, index_path1)){
		if(!exist_index_down(path_complete1, index_path1)){
			error(DIR_IS_NOT_A_REPO, "compare", index1);
			return;
		}
	}

	/*verifico que index2 sea un repositorio */
	if(!exist_index_up(path_complete2, index_path2)){
		if(!exist_index_down(path_complete2, index_path2)){
			error(DIR_IS_NOT_A_REPO, "compare", index2);
			return;
		}
	}
	
	if(!strcmp(index1, index2)){
		message_repos_equals();
		return;
	}

	char contents_path1[LENGTH_PATH], contents_path2[LENGTH_PATH];
	char rev_path1[LENGTH_PATH], rev_path2[LENGTH_PATH];
	int nro_rev1, nro_rev2;
	
	/* obtengo el path del archivo que contiene el numero de revision actual, para cada directorio, y luego guardo dicho numero */
	sprintf(rev_path1, "%s/%s/rev", path_complete1, INDEX);
	sprintf(rev_path2, "%s/%s/rev", path_complete2, INDEX);
	get_number_of_rev(rev_path1, &nro_rev1);
	get_number_of_rev(rev_path2, &nro_rev2);
	
	/* obtengo el path del archivo que corresponde al ultimo commit realizado */
	sprintf(rev_path1, FMT_REV(path_complete1, nro_rev1));
	sprintf(rev_path2, FMT_REV(path_complete2, nro_rev2));

	/* genero los arboles a partir de dichos archivos */
	TreeNode *tree1 = file_to_tree(rev_path1, contents_path1, WITH_HASH);
	TreeNode *tree2 = file_to_tree(rev_path2, contents_path2, WITH_HASH);

	if(hash_equals(hash_extra(tree1), hash_extra(tree2))){
		message_repos_equals();
		tree_node_destroy(tree1);
		tree_node_destroy(tree2);
		return;
	}

	/* comparo cada nodo de las dos arboles, mostrando las diferencias entre los repositorios */
	compare_trees(tree1, tree2, path_complete1, path_complete2);
	
	/* libero memoria */
	tree_node_destroy(tree1);
	tree_node_destroy(tree2);
}