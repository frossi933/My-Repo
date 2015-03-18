#include <stdlib.h>
#include <math.h>

#include "SList.h"
#include "Common.h"

SList *slist_append(SList *list, void *data){
      SList *newNode = (SList *)malloc(sizeof(SList));
      SList *node;
      newNode->data = data;
      newNode->next = NULL;
      if (list == NULL) {
         return newNode;
      }
      node = list;
      while (slist_next(node) != NULL) {
            node = slist_next(node);
      }
      node->next = newNode;
      return list;
}

SList *slist_prepend(SList *list, void *data){
      SList *newNode = (SList *)malloc(sizeof(SList));
      newNode->next = list;
      newNode->data = data;
      return newNode;
}

void  slist_destroy(SList *list){
      SList *nodeToDelete;
      while (list != NULL) {
            nodeToDelete = list;
            list = slist_next(list);
            free(nodeToDelete);
      }
}

void  slist_foreach(SList *list, VisitorFunc visit, void *extra_data){
      if(visit == NULL || list == NULL)
		return;
      SList *node = list;
      while (node != NULL) {
            visit(slist_data(node), extra_data);
            node = slist_next(node);
      }
}

SList *slist_remove (SList *list, void *data){

	SList *result = NULL;
	SList *tmp = list;

	while(list){
		if(slist_data(list) != data)
			result = slist_append(result, slist_data(list));

		list = slist_next(list);	
	}
	slist_destroy(tmp);
	return result;
}

size_t slist_length (SList *list){
	size_t len;
	SList *aux = list;
	for (len = 0; list != NULL; len++, list = slist_next(list));
	list = aux;
	return len;
}