#ifndef __ERROR_H__
#define __ERROR_H__

#include "./repo/Common.h"

typedef enum {
	REPO_EXISTS,
	NOT_WITHIN_A_REPO,
	ELEM_IS_IN_THE_REPO,
	ELEM_IS_NOT_IN_THE_REPO,
	PARENT_DIR_IS_NOT_IN_THE_REPO,
	ELEM_IS_THE_ROOT,
	ELEM_DOES_NOT_EXIST,
	DIR_IS_NOT_IN_THE_REPO,
	DIR_IS_NOT_THE_ROOT,
	DIR_IS_NOT_A_REPO,
	INCORRECT_COMMAND,
	CAN_NOT_OPEN_FILE,
	COULD_NOT_CREATE_FILE,
	REPO_NOT_CHANGED,
	HIDDEN_ELEM,
	WRONG_REV_NUMBER,
	OTHER
} ErrorType;
	
/* muestra en pantalla los detalles del uso de cada comando */
void 	print_usage(char *comando);

/* muestra en pantalla un mensaje de error, con su descripcion corrspondiente */
void 	error(ErrorType type, char *comando, char *extra);

#endif	/* __ERROR_H__ */
