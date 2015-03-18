#include "Tree.h"
#include "Path.h"
#include "File.h"
#include "Messages.h"

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define name_of_child(child)		(name((TreeNode *)slist_data(child)))
#define is_contents(path)			(strcmp(get_element_from_path(path), "contents")) ? 0 : 1

static SList *list_of_arch(char *path, char *contents_path, ConstructionType type);

static int selecc_arch(const struct dirent *entry){	//no selecciona los ocultos
      if (entry->d_name[0] == '.')
      	return 0;
      else
      	return 1;
}

int name_compare_treenode(void *node1, void *node2){
	if(!strcmp(name((TreeNode *)node1), name((TreeNode *)node2)))
		return 1;
	return 0;
}
	
TreeNode *get_node_eq_in_childrens(TreeNode *tree, TreeNode *node){
	SList *children = children(tree);
	/* busco entre los hijos de "tree", el nodo que sea equivalente a "node", es decir que tengo el mismo "name" */
	while(children){
		if(name_compare_treenode(slist_data(children), node))
			return (TreeNode *)slist_data(children);
		children = slist_next(children);
	}
	/* no encontre el nodo equivalente */
	return NULL;
}

/* crea el arbol de directorios recursivamente */
static TreeNode *tree_dir_new_recursive(int is_dir, char *path, char *contents_path, ConstructionType type){
	TreeNode *result = (TreeNode *)calloc(1, sizeof(TreeNode));
	result->is_dir = is_dir;
	strcpy(name(result), path);

	if(is_dir){
		children(result) = list_of_arch(path, contents_path, type);
		if(type == WITHOUT_EXTRA)
			result->extra = NULL;
	} else {
		children(result) = NULL;
		if(type == WITHOUT_EXTRA)
			result->extra = NULL;
	}
	return result;
}

static SList *list_of_arch(char *path, char *contents_path, ConstructionType type){
	SList *list = NULL;
	char path_complete[LENGTH_PATH];
	DIR *current_dir = opendir(path);
	struct dirent *direntp;

	while((direntp = readdir(current_dir)) != NULL){
		if(direntp->d_name[0] == '.')
			/* ignoro los archivos ocultos */
			continue;
		sprintf(path_complete, "%s/%s", path, direntp->d_name);
		list = slist_append(list, tree_dir_new_recursive(is_dir(path_complete), path_complete, contents_path, type) );
	}
	closedir(current_dir);
	return list;
}

TreeNode *tree_dir_new (int is_dir, char *path, char *contents_path, ConstructionType type){
	TreeNode *root = tree_dir_new_recursive(is_dir, path, contents_path, type);
	if(type == WITH_ST_HASH)
		tree_node_foreach_node(root, set_current_sthash, CHILD_FIRST_ORDER, contents_path);
	else if (type == WITH_CURRENT_HASH)
		tree_node_foreach_node(root, set_current_extrahash, CHILD_FIRST_ORDER, contents_path);	
	return root;
}

TreeNode *find_treenode_from_path(TreeNode *root, char *path){
     	if(!strcmp(name(root),path))
		return root;

	if(!root->is_dir)
		return NULL;
	else {
		SList *children = children(root);
		while (children) {
			TreeNode *child = (TreeNode*)slist_data(children);
			TreeNode *node_ret = find_treenode_from_path(child, path);
			if(node_ret)
				return node_ret;
			else 
				children = slist_next(children);
		}
	}
	return NULL;
}

void tree_node_remove (TreeNode *root, char *name) {
	TreeNode *removed_parent, *removed_node;
	char *father;	

	father = get_father_element(name);
	/* obtengo el nodo a remover y el pader del mismo */
	removed_parent = find_treenode_from_path(root, father);
	removed_node = find_treenode_from_path(removed_parent, name);

	/* quito dicho nodo y libero la memoria correspondiente */
	children(removed_parent) = slist_remove(children(removed_parent), removed_node);
	tree_node_destroy(removed_node);
	free(father);
}

void inline tree_node_add_child (TreeNode *root, char *path_father, TreeNode *child){
	/* Obtengo el nodo del padre de "child", es decir de "path_father", y le agrego a sus hijos el nuevo nodo "child" */
	TreeNode *father = find_treenode_from_path(root, path_father);
	children(father) = slist_append(children(father), child);
}

/* Crea un nuevo TreeNode con los datos pasados, y devuelve un puntero al mismo */
static TreeNode *tree_node_new (int is_dir, char *path){
	TreeNode *node = (TreeNode *)calloc(1,sizeof(TreeNode));
	node->is_dir = is_dir;
	strcpy(name(node), path);
	children(node) = NULL;
	return node;
}

void tree_node_foreach_node (TreeNode *root, VisitorFunc visit, VisitType type, void *extra_data){
	if(type == NODE_FIRST_ORDER){
		visit(root,extra_data);

		if(root->is_dir){
			SList *tmp = children(root);
			while(tmp){
				tree_node_foreach_node(slist_data(tmp),
							visit,
							NODE_FIRST_ORDER,
							extra_data);
				tmp = slist_next(tmp);
			}
		}

	} else {
	/*type == CHILD_FIRST_ORDER */
		if(!root->is_dir){
			visit(root, extra_data);
			return;
		}
		SList *tmp = children(root);
		while(tmp){
			tree_node_foreach_node(slist_data(tmp), visit, CHILD_FIRST_ORDER, extra_data);
			tmp = slist_next(tmp);
		}
		visit(root, extra_data);
	}
}

static inline void set_st(void *_node , void *_type ){
	status((TreeNode *)_node) = *(StatusType *)_type;
}

void set_and_print_status(TreeNode *dir_tree, TreeNode *hash_tree, char *contents_path, int print_option){
	StatusType uptod = UPTOD;
	StatusType mod = MOD;
	StatusType unt = UNT;
	StatusType noc = NOC;

	/* seteo el nodo actual de "dir_tree" y muestro por pantalla si "print_option" esta activada */
	if(elem_in_contents(dir_tree->name, contents_path) != -1){
		if( hash_equals(hash_sthash(dir_tree), hash_extra(hash_tree)) ){
			if(print_option)
				set_and_print_st(dir_tree, &uptod);
			else
				set_st(dir_tree, &uptod);
		} else {
			if(print_option)
				set_and_print_st(dir_tree, &mod);
			else
				set_st(dir_tree, &mod);
		}
	} else {
		if(print_option)
			tree_node_foreach_node( dir_tree, set_and_print_st, NODE_FIRST_ORDER, &unt);
		else
			tree_node_foreach_node( dir_tree, set_st, NODE_FIRST_ORDER, &unt);
		return;
	}
	
	if(dir_tree->is_dir) {
	/* si es un directorio recorro cada uno de sus hijos */
		SList *children = children(dir_tree);
		while(children){
			TreeNode *child_dir_tree = slist_data(children);
			TreeNode *child_hash_tree = get_node_eq_in_childrens(hash_tree, child_dir_tree);

			if(child_hash_tree == NULL){
			/* Si no encontre el nodo equivalente */
				if(elem_in_contents(name(child_dir_tree), contents_path) == -1){
				/* No se encuentra bajo control de versiones */
					if(child_dir_tree->is_dir){
						if(print_option)
							tree_node_foreach_node( child_dir_tree,
										set_and_print_st,
										NODE_FIRST_ORDER,
										&unt);
						else
							tree_node_foreach_node( child_dir_tree,
										set_st,
										NODE_FIRST_ORDER,
										&unt);
					} else {
					/* es un fichero */
						if(print_option)
							set_and_print_st( child_dir_tree, &unt);
						else
							set_st(child_dir_tree, &unt);
					}
				} else {
				/* Se encuentran bajo control de versiones pero tdv no fueron commiteados */
					if(child_dir_tree->is_dir){
						if(print_option)
							tree_node_foreach_node( child_dir_tree,
										set_and_print_st,
										NODE_FIRST_ORDER,
										&noc);
						else
							tree_node_foreach_node( child_dir_tree,
										set_st,
										NODE_FIRST_ORDER,
										&noc);
						
					} else {
					/* es un fichero */
						if(print_option)
							set_and_print_st(child_dir_tree, &noc);	
						else
							set_st(child_dir_tree, &noc);
					}
				}
			} else
			/* encontre el nodo equivalente */
				set_and_print_status(child_dir_tree, child_hash_tree, contents_path, print_option);
			children = slist_next(children);
		}
	}
}

void search_and_show_mod_in_tree(TreeNode *old_tree, TreeNode *current_tree, int old_rev){
	if(current_tree == NULL) {
		/* El archivo de "old_tree" no tiene equivalente en "current_tree", entonces es un archivo que ha sido removido */
		tree_node_foreach_node(old_tree, message_old_file, NODE_FIRST_ORDER, NULL);
		return;
	} else if(hash_equals(hash_extra(old_tree), hash_extra(current_tree))){
		/* si son iguales vuelvo */
		return;
	} else {
		/* ha sido modificado */
		if(old_tree->is_dir){
			/* busco recursivamente mas nodos equivalentes */
			SList *children = children(old_tree);
			while(children){
				/* obtengo el nodo equivalente y vuelvo a aplicar la misma funcion */
				TreeNode *child_current = get_node_eq_in_childrens(current_tree, slist_data(children));
				search_and_show_mod_in_tree(slist_data(children), child_current, old_rev);
				if(child_current != NULL)
					/* si encuentro el nodo equivalente lo remuevo */
					tree_node_remove(current_tree, child_current->name);
				children = slist_next(children);
			}
			/* Ahora recorro los nodos de "current_tree" que me quedaron sin nodos enquivalentes en "old_tree" */
			children = children(current_tree);
			while(children){
				/* archivos que se crearon luego */
				tree_node_foreach_node((TreeNode *)slist_data(children), message_current_file, NODE_FIRST_ORDER, NULL);
				children = slist_next(children);
			}

		} else
		/* es un archivo, entonces muestro las diferencias */
			show_diffs(name(old_tree), old_rev);
	}
}

TreeNode *file_to_tree(char *path_file, char *_contents_path, ConstructionType type) {
	/* Abro "path_file" */
	FILE *file;
	if((file = fopen(path_file,"rw")) == NULL)
		return NULL;

	TreeNode *root; 
	char line[MAX_LENGTH_OF_LINE], contents_path[LENGTH_PATH];
	char *path_line;

	/*descarto la primer linea que contiene la hora y fecha*/
	fgets(line, MAX_LENGTH_OF_LINE, file);

	if(_contents_path != NULL){
		/*es un archivo de revision y entonces descarto la segunda linea que contiene el mensaje*/
		fgets(line, MAX_LENGTH_OF_LINE, file);
		strcpy(contents_path, _contents_path);
	} else
		strcpy(contents_path, path_file);

	if(type == WITH_HASH){
		/* Nodos con hashes del archivo */
		Hash *hash = (Hash *)malloc(sizeof(Hash));
		char str_hash[MAX_LENGTH_OF_LINE];

		fscanf(file, "%s %s\n", line, str_hash);
		string_to_hash(str_hash, *hash);

		/* creo la raiz del arbol */
		root = tree_node_new(is_dir(line), line); 
		root->extra = hash;
	
		while(fscanf(file, "%s %s\n", line, str_hash) != EOF){
			/* para cada linea de "file" creo su nodo correspondiente */
			TreeNode *node, *father;
			char *path_father;
			
			if(!exist_elem(line))
				/* en caso de que el archivo haya sido eliminado y ya no exista, lo ignoro */
				continue;

			node = tree_node_new(is_dir(line), line);
			path_father = get_father_element(line);
			tree_node_add_child(root, path_father, node);

			Hash *hash_node = (Hash *)malloc(sizeof(Hash));
			string_to_hash(str_hash, *hash_node);
			node->extra = hash_node;

			free(path_father);
		}
	} else {
		/*creo el nodo del directorio raiz*/
		fscanf(file, "%s\n", line);
		root = tree_node_new(1, line);

		while(fscanf(file, "%s\n", line) != EOF){
			/* para cada linea de "file" creo su nodo correspondiente */
			TreeNode *node, *father;
			char *path_father;

			if(!exist_elem(line))
				/* en caso de que el archivo haya sido eliminado y ya no exista, lo ignoro */
				continue;

			node = tree_node_new(is_dir(line), line);
			path_father = get_father_element(line);
			tree_node_add_child(root, path_father, node);		

			free(path_father);
		}		
		/* una vez armado todo el arbol, establezco el hash de cada nodo si es necesario */
		if(type == WITH_CURRENT_HASH)
			tree_node_foreach_node(root, set_current_extrahash, CHILD_FIRST_ORDER, contents_path);
		else if(type == WITH_ST_HASH)
			tree_node_foreach_node(root, set_current_sthash, CHILD_FIRST_ORDER, contents_path);		
	}
	fclose(file);
	return root;
}

void tree_node_destroy (TreeNode *tree){
	if(tree == NULL)
		return;

	SList *children = children(tree);
	SList *NodeNext;

	if(tree->is_dir){
		while(children){
			/* libero la memoria de cada hijo */
			tree_node_destroy(slist_data(children));
			NodeNext = slist_next(children);
			children = NodeNext;
		}
		/* libero el campo "children" de la estructura */
		slist_destroy(children(tree));
	}

	if(tree->extra!=NULL)
		/* en caso de tener algo en "extra" lo libero tambien */
		free(tree->extra);
	/* por ultimo libero los datos de nodo */
	free(tree);
}

void compare_trees(TreeNode *tree1, TreeNode *tree2, char *index_path1, char *index_path2){
	SList *child1 = children(tree1);
	while(child1){
		SList *child2 = children(tree2);
		if(is_dir( name_of_child(child1) )){
		/* child1 es un directorio */
			while(child2){
				if( is_dir( name_of_child(child2) ) ){
					/* si child2 es un directorio, obtengo los paths relativos y los comparo */
					char relative_path1[LENGTH_PATH];
					char relative_path2[LENGTH_PATH];
					get_relative_path(name((TreeNode *)slist_data(child1)), index_path1, relative_path1);
					get_relative_path(name((TreeNode *)slist_data(child2)), index_path2, relative_path2);

					if( !strcmp(relative_path1, relative_path2) ){
					/* encontramos el nodo equivalente, entonces volvemos a comparar dichos subarboles */
						compare_trees((TreeNode *)slist_data(child1), (TreeNode *)slist_data(child2), name_of_child(child1), name_of_child(child2));
						break;
					}
				}
				child2 = slist_next(child2);
			}
			if(child2 == NULL){
			/* finalizo el while sin encontrar el dir equivalente */
				/* entonces el nodo de child1 solo esta en tree1 */
				tree_node_foreach_node(slist_data(child1), compare_show_files, NODE_FIRST_ORDER, index_path1);
			} else
			/* finalizo el while encontrando el dir equivalente, entonces lo elimino de tree2 */
				tree_node_remove(tree2, name_of_child(child2));
		} else {
		/* child1 es un archivo */
			while(child2){
				if(!is_dir( name_of_child(child2)  )){
					/* Si es un fichero, obtengo los paths relativos y los comparo */
					char relative_path1[LENGTH_PATH];
					char relative_path2[LENGTH_PATH];
					char path1[LENGTH_PATH], path2[LENGTH_PATH];
					strcpy(path1, name_of_child(child1));
					strcpy(path2, name_of_child(child2));
						
					get_relative_path(path1, index_path1, relative_path1);
					get_relative_path(path2, index_path2, relative_path2);
					
					if(!strcmp(relative_path1, relative_path2)){
					/* encontramos el archivo equivalente */
						if(!hash_equals(hash_of((TreeNode *)slist_data(child1)), hash_of((TreeNode *)slist_data(child2)))){
						/* si no son iguales, muestro las diferencias entre ellos */
							printf("my-diff %s %s\n", relative_path1, relative_path2);
							FILE *file1 = fopen(path1, "r");
							FILE *file2 = fopen(path2, "r");
							my_diff(file1, file2, stdout);
							fclose(file1);
							fclose(file2);
						}
						break;
					}
				}
				child2 = slist_next(child2);
			}
			if(child2 == NULL){
			/* finalizo el while sin encontrar el archivo equivalente */
				/* entonces el nodo de child1 solo esta en tree1 */
				compare_show_files(slist_data(child1), index_path1);
			} else
			/*finalizo el while encontrando el archivo equivalente,entonces lo elimino de tree2*/
				tree_node_remove(tree2, name_of_child(child2));
		}
		child1 = slist_next(child1);
	}
	/* Recorro lo que me queda de "tree2" para mostrar los archivos */
	tree_node_foreach_node(tree2, compare_show_files, NODE_FIRST_ORDER, index_path2);
}