#include "Common.h"
#include "Messages.h"
#include "Hash.h"
#include "Tree.h"
#include "Content.h"

#include <stdio.h>
#include <string.h>

#define FMT_STATES(state, path)		"%s  -->  %s\n", (state), (path)
#define NUM_OF_STATES				4
#define MAX_LENGTH_OF_STATES		20

char estados[NUM_OF_STATES][MAX_LENGTH_OF_STATES] = { "Untracked  ", "Modified   ", "Up To Date ", "No commited" };

inline void message_init_done(void){
	printf("--> Init realizado con exito.\n");
}

inline void message_add_done(void){
	printf("--> Add realizado con exito.\n");
}
inline void message_adding_node(void *_node, void *extra){
	if(elem_in_contents(name((TreeNode *)_node), (char *)extra) == -1)
		printf("--> Agregando \"%s\"...\n", name((TreeNode *)_node));
}

inline void message_rm_done(void *_node, void *extra){
	printf("--> Se ha removido \"%s\" con exito.\n", name((TreeNode *)_node));
}

inline void message_commit_done(void){
	printf("--> Commit realizado con exito.\n");
}

inline void message_repos_equals(void){
	printf("--> Los repositorios son iguales\n");
}

inline void set_and_print_st(void *_node , void *_type ){
	status((TreeNode *)_node) = *((StatusType *)_type);
	printf(FMT_STATES(estados[*((StatusType *)_type)],name((TreeNode *)_node)));
}

void message_print_status(void *_node, void *_contents){
	if(elem_in_contents(name((TreeNode *)_node), (char *)_contents) != -1){
		StatusType noc = NOC;
		set_and_print_st(_node, &noc);
	} else {
		StatusType unt = UNT;
		set_and_print_st(_node, &unt);
	}
} 

void message_print_diffs(FILE *res_patch, FILE *current_file, char *path){
	printf("--> Diferencias del archivo %s\n", path);
	my_diff(res_patch, current_file, stdout);
	printf("\n\n");
}

void message_current_file(void *_node, void *_extra){
	printf("Archivo nuevo: %s\n", ((TreeNode *)_node)->name);
}

void message_old_file(void *_node, void *_extra){
	printf("Archivo anterior: %s\n", ((TreeNode *)_node)->name);
}

void show_time_and_message(FILE *file, int rev){
	char line[MAX_LENGTH_OF_LINE];
	fgets(line, MAX_LENGTH_OF_LINE, file);
	printf("--> Id: %d\t--> Fecha: %s", rev, line);
	fgets(line, MAX_LENGTH_OF_LINE, file);
	printf("--> Mensaje: %s", line);
	puts(" ");
}