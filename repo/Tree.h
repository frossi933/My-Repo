#ifndef __TREE_H__
#define __TREE_H__

#include "Common.h"
#include "Hash.h"

#define 	name(node)			((node)->name)
#define	children(node)		((node)->children)
#define	hash_extra(s)			(*((Hash *)((s)->extra)))
#define 	hash_sthash(s)		(((St_Hash *)((s)->extra))->hash)
#define 	status(s)				(((St_Hash *)((s)->extra))->st)
#define 	OPTION_PRINT_ON		1
#define 	OPTION_PRINT_OFF		0


/* devuelvo el nodo equivalente a "node" (comparado por nombres) en los hijos de "tree" */
TreeNode *get_node_eq_in_childrens(TreeNode *tree, TreeNode *node);

char 	*generate_path_complete( char *element);

/* retorna 1 si el nombre de "node1" es igual al de "node2"
	   0 en caso contrario */
int 		name_compare_treenode(void *node1, void *node2);

/* Busco y muestro por pantalla, las modificaciones y diferencias entre los arboles "old_tree" y "current_tree" */
void 	search_and_show_mod_in_tree(TreeNode *old_tree, TreeNode *current_tree, int old_rev);

/* Retorno un puntero al nodo que contiene un nombre igual a "path" */
TreeNode *find_treenode_from_path(TreeNode *root, char *path);

/* genera el arbol correspondiente al sistema de archivos de "path" */
TreeNode *tree_dir_new (int is_dir, char *name, char *contents_path, ConstructionType type);

/* Agrego el nodo "child" a los hijos de "root" */
void	tree_node_add_child (TreeNode *root, char *path_father, TreeNode *child);

/* aplica la funcion "visit" tomando como parametro cada nodo del arbol apuntado por "root" y "extra_data", segun lo indica "type" */
void 	tree_node_foreach_node (TreeNode *root, VisitorFunc visit, VisitType type, void *extra_data);

/* libera la memoria ocupada por "tree" */
void	tree_node_destroy (TreeNode *tree);

/* remueve al nodo cuyo nombre es "name" del arbol "root" */
void	tree_node_remove (TreeNode *root, char *name);

/* retorna un arbol creado a partir del archivo "path_file", en caso de que "path_file" no sea el archivo contents, el path debe ser pasado como argumento
	si "type" == WITH_ST_HASH			devuelvo un TreeNode *  con extra == St_Hash
	si "type" == WITH_CURRENT_HASH  	devuelvo un TreeNode *  con extra == Hash, recalculando el hash actual de c/nodo
	si "type" == WITH_HASH 			devuelvo un TreeNode *	con extra == Hash
	si "type" == WITHOUT_EXTRA			devuelvo un TreeNode *	con extra == NULL	
 En caso de error retorna NULL */
TreeNode *file_to_tree(char *path_file, char *contents_path, ConstructionType type);

/* seteo el campo "status" de todos los nodos de "dir_tree", comparandolo con sus equivalentes
 en "hash_tree" y los muestro en pantalla si el valor de "print_option" es OPTION_PRINT_ON */
void 	set_and_print_status(TreeNode *dir_tree, TreeNode *hash_tree, char *contents_path, int print_option);

/* Comparo los arboles "tree1" y "tree2", mostrando las diferencias entre ellos */
void 	compare_trees(TreeNode *tree1, TreeNode *tree2, char *index_path1, char *index_path2);

#endif	/* __TREE_H__ */