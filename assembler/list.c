#include "List.h"

#define NOT_IMPLEMENTED() assert(false && "not implemented\n")

void List_defaultPrintFunc(void* item){
    printf("%p", item);
}

typedef struct node_t{
    void* item;
    struct node_t* next;
    struct node_t* prev;
} Node; 

Node* Node_create(void* item, List_copyFunction copy){
    Node* res = (Node*)malloc(sizeof(*res));
    if(!res){
        return NULL;
    }
    if(copy != NULL){
        res->item = copy(item);
    }
    else{
        res->item = item;
    }
    res->next = NULL;
    res->prev = NULL;
    return res;
}

void Node_destroy(Node* list, List_destroyFunction destroy){
    if(!list){
        return;
    }
    if(destroy != NULL){
        destroy(list->item);
        list->item = NULL;
    }
    free(list);
}

struct List_t{
    size_t size;
    Node* head;
    Node* tail;

    List_copyFunction copy;
    List_destroyFunction detroy;
    List_printFunction print;
    List_comperFunction comper;
};

List* List_create(List_copyFunction copy, 
                List_destroyFunction destroy, 
                List_printFunction print, 
                List_comperFunction comper){
    
    List* res = (List*)malloc(sizeof(*res));
    if(!res){
        return NULL;
    }

    // If memory ownership is defined, we will require consistency
    if((copy && !destroy) || (!copy && destroy)){
        free(res);
        return NULL;
    }

    if(!print){
        res->print = List_defaultPrintFunc;
    }
    else{
        res->print = print;
    }

    res->copy = copy;
    res->detroy = destroy;
    res->comper = comper;

    res->size = 0;
    res->head = NULL;
    res->tail = NULL;

    return res;
}

void List_destroy(List* list){
    List_clear(list);
    free(list);
    list = NULL;
}

List* List_copy(List* list){
    NOT_IMPLEMENTED();
    if(!list){
        return NULL;
    }
    List* res = List_create(list->copy, list->detroy, list->print, list->comper);
    if(!res){
        return NULL;
    }
    if(!res->head){
        return res;
    }

    Node* curr_list = list->head;
    Node* curr_res = NULL;
    bool is_res_head = true;
    while(curr_list){
        Node* curr_res = Node_create(curr_list->item, list->copy);
        if(!curr_res){
            return NULL;
        }
        if(is_res_head){
            is_res_head = false;
            res->head = curr_res;
        }
        curr_list = curr_list->next;
        curr_res = curr_res->next;
    }

    res->size = list->size;
    return res;
}

size_t List_getSize(List* list){
    if(!list){
        return 0;
    }
    return list->size;
}

bool List_empty(List* list){
    if(!list){
        return true;
    }
    return (List_getSize(list) == 0);
}

// not coping the memory!
void* List_at(List* list, int index){
    if(!list){
        return NULL;
    }
    if(index < 0 || index >= List_getSize(list)){
        return NULL;
    }
    Node* curr = list->head;
    int iteration = 0;
    while(iteration < index){
        curr = curr->next;
        iteration++;
    }
    return curr->item;
}

int List_pushBack(List* list, void* item){
    if(!list){
        return List_INVALID_INPUT;
    }

    Node* newNode = Node_create(item, list->copy);
    if(!newNode){
        return List_NOMEM;
    }

    if(List_getSize(list) == 0){
        list->head = newNode;
        list->tail = newNode;
    }

    else if(List_getSize(list) == 1){
        list->tail = newNode;
        list->head->next = newNode;
        newNode->prev = list->head;
    }

    else{
        Node* oldTail = list->tail;
        list->tail = newNode;
        oldTail->next = newNode;
        newNode->prev = oldTail;
    }

    list->size++;
    return List_OK;
}

int List_pushFront(List* list, void* item){
    NOT_IMPLEMENTED();
}

int List_clear(List* list){
    if(!list){
        return List_INVALID_INPUT;
    }
    Node* curr = list->head;
    Node* doDestroy = NULL;
    while(curr){
        doDestroy = curr;
        curr = curr->next;
        Node_destroy(doDestroy, list->detroy);
        doDestroy = NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return List_OK;
}

void List_print(List* list){
    if(!list){
        printf("NULL\n");
        return;
    }
    bool first = true;
    printf("[");
    Node* curr = list->head;
    while(curr){
        if (!first) printf(" ,");
        list->print(curr->item);
        curr = curr->next;
        first = false;
    }
    printf("]\n");
}