#ifndef __COMMON_H__
#define __COMMON_H__

#define		HASH_SIZE			(20/sizeof(unsigned char))
#define 		MAX_LENGTH_OF_LINE	300
#define 		LENGTH_PATH			256

#ifndef _WIN32
		#define	INDEX			".index"
#else
		#define	INDEX			"_index"
#endif

#define FMT_COMMIT(father, rev, elem)	"%s/.%d_%s", father, rev, elem		//(father)/.(n°rev)_(element)
#define FMT_REV(index, rev)			"%s/%s/.%d", index, INDEX, rev		//(index_path)/INDEX/.(n°rev)


/* Definiciones de tipos de funciones, estructuras y enumeraciones utilizadas a lo largo del programa */
typedef void (*VisitorFunc) (void *data, void *extra_data);
typedef void (*StatusVisitor) (void *_node, void *_type);

typedef enum {
	CHILD_FIRST_ORDER,		// Son visitados primero los nodos hijos, y se recorre el arbol en forma ascendente 
	NODE_FIRST_ORDER		// El arbol es recorrido a partir de su raiz, con un orden descendente y en profundidad
} VisitType;

typedef enum {
	WITH_ST_HASH,
	WITH_CURRENT_HASH,
	WITH_HASH,
	WITHOUT_EXTRA
} ConstructionType;

typedef enum {
	UNT = 0,			//	No se encuentra bajo control de versiones
	MOD,			//	Se modifico desde el ultimo commit
	UPTOD,			//	No se modifico desde el ultimo commit
	NOC			//	No fue commiteado por primera vez
} StatusType;

typedef struct _SList {
        void   		*data;
        struct _SList 	*next;
} SList;

typedef unsigned char Hash[HASH_SIZE];

typedef struct {
	Hash		hash;
	StatusType	st;
} St_Hash;

typedef struct {
	int			is_dir;			//1 si es un directorio, 0 si es un archivo
	char 		name[LENGTH_PATH];
	SList		*children;
	void		*extra;			//puede ser NULL, *Hash o *St_Hash
} TreeNode;


#endif /* __COMMON_H__ */