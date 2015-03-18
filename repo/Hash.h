#ifndef __HASH_H__
#define __HASH_H__

#include <openssl/sha.h>		//-lcrypto al final del gcc... 

#include "SList.h"
#include "Common.h"

/* devuelve 1 en caso de que el nodo "tree" contenga el campo extra de su estructura del tipo St_Hash */
#define is_a_tree_with_St_Hash(tree)		(sizeof(Hash) == sizeof(*((tree)->extra))) ? 0 : 1
#define hash_of(node)					( (is_a_tree_with_St_Hash(node)) ? (hash_sthash(node)) : (hash_extra(node)) )

/* devuelve 1 si "hash" no esta formado por todos ceros
	    0 en caso contrario */
int 		hash_is_not_zero(Hash hash);

/* retorna 1 en caso de que "a" sea igual a "b"
	   0 en caso de que "a" sea distinto a "b"*/
int 		hash_equals(const Hash a, const Hash b);

/* calcula el hash del directorio que contiene a "childrens" a partir de los hashes de sus ficheros, y lo guarda en lo apuntado por "hash"*/
void 	dir_hash(SList *childrens, Hash *hash);

/* calcula el hash del archivo nombrado por path (con SHA1) y lo guarda en lo apuntado por hash */
void 	file_hash(unsigned char *path, Hash *hash);

/* devuelve el Hash contenido en "line"	*/
Hash 	*get_hash_from_line(char *line);

/* convierte la cadena "str" a tipo Hash, y lo guarda en "hash"	*/
void 	string_to_hash (char *str, Hash hash);

/* actualiza el hash del nodo apuntado por "node" */
void 	update_hash(TreeNode *node);

/* crea el extra de "_node" de tipo St_Hash y actualiza al hash actual de "_node"	*/
void 	set_current_sthash(void *_node, void *_extra);

/* crea el extra de "_node" de tipo Hash y actualiza al hash actual de "_node"	*/
void 	set_current_extrahash(void *_node, void *_extra);

#endif	/* __HASH_H__ */