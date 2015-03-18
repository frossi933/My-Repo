#include "Tree.h"
#include "Hash.h"
#include "Common.h"

#include <string.h>
#include <stdio.h>
#include <openssl/sha.h>

int  hash_equals(const Hash a, const Hash b){
	int i;
	for(i = 0; i < HASH_SIZE; i++){
		if(a[i] ^ b[i])
			return 0;
	}
	return 1;
}

void file_hash(unsigned char *path, Hash *hash){
	SHA_CTX	hash_struc;
	FILE *file;

	unsigned char line[MAX_LENGTH_OF_LINE];
	unsigned long line_len;

	file = fopen(path,"r");	
	SHA1_Init(&hash_struc);
	while(fgets(line, MAX_LENGTH_OF_LINE, file)){
		line_len = strlen(line);
		SHA1_Update(&hash_struc, line, line_len);
	}

	SHA1_Final(*hash, &hash_struc);
	fclose(file);
}

int hash_is_not_zero(Hash hash){
	int i;
	for(i = 0; i < HASH_SIZE; i++){
		if(hash[i])
			return 1;
	}
	return 0;
}

static int hash_compare(const void *a, const void *b){
	int res;
	int i;
	for(i = 0; i < HASH_SIZE; i++){
		res = (*(Hash *)a)[i] - (*(Hash *)b)[i];
		if(res != 0)
			return res;
	}
	return 0;
}

static void cpy_hash(Hash src, Hash *dst){
	int i;
	for(i = 0; i< HASH_SIZE; i++)
		(*dst)[i] = src[i];
}

void dir_hash(SList *childrens, Hash *hash){
	SHA_CTX	hash_struc;
	int nonzero_hashes = 0;
	
	SHA1_Init(&hash_struc);
	int length  = slist_length(childrens);
	int j = 0;
	
	Hash *hashes = (Hash *)malloc(sizeof(Hash) * length);
	SList *aux = childrens;
	int i;	
	while(aux){
		cpy_hash(hash_of((TreeNode *)slist_data(aux)), &hashes[j]);
		j++;
		aux = slist_next(aux);
	}		
	
	qsort(hashes, length, sizeof(Hash), hash_compare);
	
	for( i = 0; i < length; i++){
		if(hash_is_not_zero(hashes[i])){
			SHA1_Update(&hash_struc, hashes[i], HASH_SIZE);
			nonzero_hashes++;
		}
	}

	if(nonzero_hashes)
		SHA1_Final(*hash, &hash_struc);
	else {
		for(i = 0; i < HASH_SIZE; i++)
			(*hash)[i] = 0;
	}
	free(hashes);
}

Hash *get_hash_from_line(char *line){
	Hash	*ret;
	ret = (Hash *)malloc(sizeof(Hash));
	char str_hash[MAX_LENGTH_OF_LINE];
	char *str = strchr(line,' ') + 1 ;
	strcpy(str_hash, str);
	string_to_hash(str_hash, *ret);
	return ret;
}

void string_to_hash (char *str, Hash hash){
	size_t i;
	unsigned int num;

	for (i =0 ; str[i * 2] != '\0'; i++) {
		sscanf(str + i * 2, "%02x", &num);
		hash[i] = (unsigned char) num;
	}
}

void inline update_hash(TreeNode *node){
	if(is_a_tree_with_St_Hash(node))
		dir_hash(children(node), &hash_sthash(node));
	else
		dir_hash(children(node), &hash_extra(node));
}

void set_current_sthash(void *_node, void *_extra){
	TreeNode *node = (TreeNode *)_node;
	char *contents_path = (char *)_extra;

	St_Hash *sth = (St_Hash *)calloc(1, sizeof(St_Hash));
	node->extra = sth;

	if(elem_in_contents(node->name, contents_path) == -1)
		/* si el elemento no esta en el repositorio vuelvo, dejando su hash en cero */
		return;
	
	if(node->is_dir)
		dir_hash(children(node), &hash_sthash(node));
	else
		file_hash(name(node), &hash_sthash(node));
}

void set_current_extrahash(void *_node, void *_extra){
	TreeNode *node = (TreeNode *)_node;
	char *contents_path = (char *)_extra;

	Hash *new_hash = (Hash *)calloc(1, sizeof(Hash));
	node->extra = new_hash;
	
	if(elem_in_contents(node->name, contents_path) == -1)
		/* si el elemento no esta en el repositorio vuelvo, dejando su hash en cero */
		return;
	
	if(node->is_dir)
		dir_hash(children(node), &hash_extra(node));
	else
		file_hash(name(node), &hash_extra(node));
}