#include <stdio.h>
#include <stdlib.h>

typedef struct nodeTag
{
    int loc;
    int size;
    void* mem;
    int data;
    int count;
    struct nodeTag* next;
    struct nodeTag* fields[5];
    int numFields;
} Node;

void* heap;
Node* freeList, * allocList;

void print(Node* list)
{
    Node* ptr = list;
    printf("Location\tBlock size\tData\n");
    while(ptr != NULL)
    {
        printf("%d\t\t\t%d\t\t\t%d\n", ptr->loc, ptr->size, ptr->data);
        Node* curr = ptr;
        ptr = ptr->next;
    }
    printf("\n");
}

void printLists()
{
    printf("\n\n");
    printf("Free list\n");
    print(freeList);
    printf("Allocated list\n");
    print(allocList);

}

void init()
{
    heap = malloc(1024);
    
    freeList = (Node*) malloc(sizeof(Node)), allocList = NULL;
    freeList->next = NULL;
    freeList->loc = 0;
    freeList->size = 1024;    
}

void cleanUp()
{
    free(heap);

    Node* ptr = freeList;
    while(ptr != NULL)
    {
        Node* temp = ptr;
        ptr = ptr->next;
        free(temp);
    }

    ptr = allocList;
    while(ptr != NULL)
    {
        Node* temp = ptr;
        ptr = ptr->next;
        free(temp);
    }

}

void* allocate(int req)
{
    Node* ptr = freeList, * prev = NULL;
    while(ptr != NULL && req > ptr->size)
    {
        prev = ptr;
        ptr = ptr->next;
    }


    if(ptr->size > req)
    {
        Node* nptr = (Node*) malloc(sizeof(Node));
        nptr->next = NULL;
        nptr->size = req;
        ptr->size -= req;
        
        if(prev == NULL)
        {
            nptr->loc = ptr->loc;
            nptr->next = freeList;
            ptr->loc = nptr->loc + req;
            freeList = nptr;
        }
        else
        {            
            prev->next = nptr;
            nptr->next = ptr;            
            nptr->loc = ptr->loc;
            ptr->loc = ptr->loc + req;
        }
        ptr = nptr;
    }


    
    if(ptr == freeList)
        freeList = ptr->next;
    else
        prev->next = ptr->next;
    
    ptr->next = NULL;
    
    if(allocList == NULL)
        allocList = ptr;
    else
    {
        Node* temp = allocList;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = ptr;
    }

    ptr->mem = heap + ptr->loc;
    return ptr->mem;
}


void coalesce()
{
    Node* ptr = freeList, * prev = NULL;

    while(ptr->next != NULL)
    {
        if(ptr->loc + ptr->size == ptr->next->loc)
        {
            ptr->size += ptr->next->size;
            Node* temp = ptr->next;
            ptr->next = ptr->next->next;
            free(temp);
        }
        else
        {
            prev = ptr;
            ptr = ptr->next;
        }

        // handles the edge case where the current node has the same size as the previous node
        if(prev != NULL &&  prev->loc + prev->size == ptr->loc)
        {
            prev->size += ptr->size;
            Node* temp = ptr;
            prev->next = ptr->next;
            free(temp);
        }
        
    }
}

void deallocate(void* mem)
{
    Node* ptr1 = allocList, * prev1 = NULL;

    while(ptr1 != NULL && ptr1->mem != mem)
    {
        prev1 = ptr1;
        ptr1 = ptr1->next;
    }

    Node* ptr2 = freeList, * prev2 = NULL;
    while(ptr2 != NULL && ptr2->loc < ptr1->loc)
    {
        prev2 = ptr2;
        ptr2 = ptr2->next;
    }


    if(ptr1 == allocList)
    {
        allocList = (allocList)->next;
    }
    else
    {
        prev1->next = ptr1->next;
    }

    
    if(ptr2 == freeList)
    {
        ptr1->next = freeList;
        freeList = ptr1;
    }
    else
    {
        prev2->next = ptr1;
        ptr1->next = ptr2;
    }

    coalesce();

}


Node* prev;
Node* curr;

int ind = 0;

void traverse(Node* ptr, int matrix[11][11])
{
    for(int i = 0; i < ptr->numFields; i++)
    {
        matrix[ptr->data][ptr->fields[i]->data] = 1;
        traverse(ptr->fields[i], matrix);
    }
}

void adjacency(Node* root)
{
    int matrix[11][11];
    for (int i = 0; i < 11; i++)
        for (int j = 0; j < 11; j++)
            matrix[i][j] = 0;

    traverse(root, matrix);

    printf("    0 1 2 3 4 5 6 7 8 9 10\n");
    printf("  --------------------------\n");
    
    for (int i = 0; i < 11; i++)
    {
        printf("%d | ", i);
    
        for (int j = 0; j < 11; j++)
            printf("%d ", matrix[i][j]);
        printf("\n");
    }
}


void connect(Node** ptr1, Node** ptr2)
{
    *ptr1 = *ptr2;
    (*ptr2)->count++;
}

void gc()
{

    Node* ptr = allocList;
    while (ptr != NULL)
    {
        if (ptr->count == 0)
        {
            for (int i = 0; i< ptr->numFields; i++)
                ptr->fields[i]->count--;
    
            Node* temp = ptr;
            ptr = ptr->next;
            printf("freeing %d\n", temp->data);
            deallocate(temp->mem);
        }
        else
            ptr = ptr->next;
    }

}

int main()
{
    init();
    printLists();


    void* k;
    for (int i = 0; i < 8; i++)
    {
        k = allocate(1);
    }


    //printLists();

    Node* ptrs[11];
    Node* ptr = allocList;

    ptrs[0] = NULL;
    
    ptrs[1] = ptr;
    ptr = ptr->next;
    
    ptrs[2] = ptr;
    ptr = ptr->next;

    ptrs[3] = ptr;
    ptr = ptr->next;

    ptrs[4] = NULL;
    
    ptrs[5] = ptr;
    ptr = ptr->next;

    ptrs[6] = NULL;
    
    ptrs[7] = ptr;
    ptr = ptr->next;

    ptrs[8] = ptr;
    ptr = ptr->next;

    ptrs[9] = ptr;
    ptr = ptr->next;

    ptrs[10] = ptr;


    connect(&ptrs[1]->fields[0], &ptrs[2]);
    connect(&ptrs[1]->fields[1], &ptrs[9]);
    connect(&ptrs[1]->fields[2], &ptrs[10]);
    ptrs[1]->numFields = 3;
    ptrs[1]->data = 1;

    ptrs[2]->data = 2;

    connect(&ptrs[3]->fields[0], &ptrs[8]);
    connect(&ptrs[3]->fields[1], &ptrs[10]);
    ptrs[3]->numFields = 2;
    ptrs[3]->data = 3;

    connect(&ptrs[5]->fields[0], &ptrs[1]);
    ptrs[5]->numFields = 1;
    ptrs[5]->data = 5;

    connect(&ptrs[7]->fields[0], &ptrs[1]);
    connect(&ptrs[7]->fields[1], &ptrs[8]);
    ptrs[7]->numFields = 2;
    ptrs[7]->data = 7;

    connect(&ptrs[8]->fields[0], &ptrs[9]);
    ptrs[8]->numFields = 1;
    ptrs[8]->data = 8;

    
    ptrs[9]->data = 9;
    
    ptrs[10]->data = 10;



    Node* root1, * root2;
    
    connect(&root1, &ptrs[5]);
    connect(&root2, &ptrs[1]);

    adjacency(root1);
    adjacency(root2);

    for (int i = 0; i < 11; i++)
        if (ptrs[i] != NULL)
            printf("node %d: count: %d\n", ptrs[i]->data, ptrs[i]->count);
    gc();
    
    
    printLists();

    
    cleanUp();
    
}
