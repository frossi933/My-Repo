#ifndef __MY_REPO_H__
#define __MY_REPO_H__

/* Comando Init */
void 	my_repo_init (void);

/* Comando Add */
void 	my_repo_add(char *element);

/* Comando Remove */
void 	my_repo_rm(char *element);

/* Comando Status */
void 	my_repo_st(void);

/* Comando Commit */
void 	my_repo_co(char *message);

/* Comando Log */
void 	my_repo_log(char *nro_rev);

/* Comando Compare */
void 	my_repo_comp(char *index1, char *index2);

#endif	/*__MY-REPO_H__*/