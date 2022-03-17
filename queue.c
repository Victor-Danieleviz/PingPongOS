#include "queue.h"
#include <stdio.h>


int queue_size(queue_t *queue){

    int count = 0;

    if(queue == NULL) return count;

    queue_t * start = queue;

    do {
        count ++;
        queue = queue->next;
    } while (queue != start);

    return count;
}

void queue_print (char *name, queue_t *queue, void print_elem (void*) ){

    if (queue == NULL){
        printf("%s: []\n", name);
        return;
    }    

    queue_t* start = queue;
    
    printf("%s: [", name);

    do{
        if(queue != start) printf(" ");
        print_elem(queue);
        queue = queue->next;
    } while(queue != start); 
    printf("]\n");

}

int queue_append (queue_t **queue, queue_t *elem){

    if (elem == NULL || elem->next != NULL || elem->prev != NULL) return -1;

    if((*queue) == NULL) {
        elem->next = elem;
        elem->prev = elem;
		(*queue) = elem;
        return 0;
    }

	(*queue)->prev->next = elem;
	elem->next = (*queue);
	elem->prev = (*queue)->prev;
	(*queue)->prev = elem;

    return 0;
}

int queue_remove (queue_t **queue, queue_t *elem){

    if ((*queue) == NULL || queue_size((*queue)) == 0 || elem == NULL) return -1;

    queue_t* start = (*queue);

   if(queue_size((*queue)) == 1 && (*queue) == elem){
		(*queue)->prev = NULL;
		(*queue)->next = NULL;
		(*queue) = NULL;
		return 0;
	}

    do {
        if ((*queue) == elem){

            queue_t* aux = (*queue);
            (*queue)->prev->next = (*queue)->next;
            (*queue)->next->prev = (*queue)->prev;

            if ((*queue) == start) (*queue) = (*queue)->next; 
            else (*queue) = start;
    
            aux->next = NULL;
            aux->prev = NULL;
            
            return 0;
         }

        (*queue) = (*queue)->next;

    } while ((*queue) != start);

    return 0;
}
