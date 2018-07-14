#include "list.h"
#include <stdio.h>
#include <stdlib.h>

void traverse(node* head, callback f)
{
    node* cursor = head;
    while (cursor != NULL) {
        f(cursor);
        cursor = cursor->next;
    }
}

node* create(node** head_ref, void* new_data)
{

    node* new_node = (node*)malloc(sizeof(node));

    if (new_node == NULL) {
        printf("Error creating a new node.\n");
        exit(0);
    }

    new_node->data = new_data;
    new_node->next = *head_ref;

    // Change head pointer as new node is added at the beginning
    (*head_ref) = new_node;

    return new_node;
}

//Note that different data types need different specifier in printf() */
void printList(node* node, void (*fptr)(void*))
{
    while (node != NULL) {
        //if (node->var == 'i')
        (*fptr)(node->data);
        node = node->next;
    }
    printf("\n");
}
// Function to print an integer
void printInt(void* n)
{
    printf(" %d", *(int*)n);
}

// Function to print a char
void printChar(void* f)
{
    printf(" %c", *(char*)f);
}

int count(node* head)
{
    short count = 0;
    node* cursor = head;
    while (cursor != NULL) {
        count++;
        cursor = cursor->next;
    }
    printf("%d\n", count);
}

void addend(node* head, void* new_data)
{
    node* cursor = head;
    while (cursor->next != NULL)
        cursor = cursor->next;

    node* tmp = (node*)malloc(sizeof(node));
    tmp->next = NULL;
    tmp->data = new_data;

    cursor->next = tmp;
}

node* search(node* head, void* data)
{
    node* cursor = head;
    while (cursor != NULL) {
        if (*(int*)cursor->data == *(int*)data)
            return cursor;
        cursor = cursor->next;
    }
    return NULL;
}

node* searchch(node* head, void* data)
{
    node* cursor = head;
    while (cursor != NULL) {
        if (*(char*)cursor->data == *(char*)data)
            return cursor;
        cursor = cursor->next;
    }
    return NULL;
}

/*
    insert a new node after the prev node
*/
node* insert_after(node* head, void* new_data, node* prev)
{
    if (head == NULL || prev == NULL)
        return NULL;
    /* find the prev node, starting from the first node*/
    node* cursor = head;
    while (cursor != prev)
        cursor = cursor->next;

    if (cursor != NULL) {
        node* tmp = (node*)malloc(sizeof(node));
        tmp->next = cursor->next;
        tmp->data = new_data;
        cursor->next = tmp;
        return head;
    }
    else {
        return NULL;
    }
}

/*
    insert a new node before the nxt node
*/
node* insert_before(node* head, void* new_data, node* nxt)
{
    if (nxt == NULL || head == NULL)
        return NULL;

    if (head == nxt) {
        node* new_node = (node*)malloc(sizeof(node));
        new_node->data = new_data;
        new_node->next = head;
        head = new_node;
        //printf("%d", *(int*)head->data);
        return head;
    }

    /* find the prev node, starting from the first node*/
    node* cursor = head;
    while (cursor != NULL) {
        if (cursor->next == nxt)
            break;
        cursor = cursor->next;
    }

    if (cursor != NULL) {

        node* new_node = (node*)malloc(sizeof(node));
        new_node->data = new_data;
        new_node->next = cursor->next;
        cursor->next = new_node;
        return head;
    }
    else {
        return NULL;
    }
}

node* insertion_sort(node* head)
{
    node *x, *y, *e;

    x = head;
    head = NULL;

    while (x != NULL) {
        e = x;
        x = x->next;
        if (head != NULL) {
            if (e->data > head->data) {
                y = head;
                while ((y->next != NULL) && (e->data > y->next->data)) {
                    y = y->next;
                }
                e->next = y->next;
                y->next = e;
            }
            else {
                e->next = head;
                head = e;
            }
        }
        else {
            e->next = NULL;
            head = e;
        }
    }
    return head;
}

node* reverse(node* head)
{
    node* prev = NULL;
    node* current = head;
    node* next;
    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    head = prev;
    return head;
}

node* remove_back(node* head)
{
    if (head == NULL)
        return NULL;

    node* cursor = head;
    node* back = NULL;
    while (cursor->next != NULL) {
        back = cursor;
        cursor = cursor->next;
    }
    if (back != NULL)
        back->next = NULL;

    /* if this is the last node in the list*/
    if (cursor == head)
        head = NULL;

    free(cursor);

    return head;
}
void dispose(node* head)
{
    node *cursor, *tmp;

    if (head != NULL) {
        cursor = head->next;
        head->next = NULL;
        while (cursor != NULL) {
            tmp = cursor->next;
            free(cursor);
            cursor = tmp;
        }
    }
}

node* remove_front(node* head)
{
    if (head == NULL)
        return NULL;
    node* front = head;
    head = head->next;
    front->next = NULL;
    if (front == head)
        head = NULL;
    free(front);
    return head;
}

void testint()
{
    node* head = NULL;
    node* tmp = NULL;
    unsigned int a = 123;
    int b = 234;
    int c = 321;
    int d = 321;
    int e = 999;
    printf("Add 123\n");
    create(&head, &a);
    printf("Add 234\n");
    create(&head, &b);
    printf("LIST Print\n");
    printList(head, printInt);
    printf("Add 321\n");
    create(&head, &c);
    printf("Count\n");
    count(head);
    printf("Addend 234\n");
    addend(head, &b);
    tmp = search(head, &d);
    printf("Add after 321 value 999\n");
    head = insert_after(head, &e, tmp);
    printf("LIST Print\n");
    printList(head, printInt);
    printf("Add before 321 value 999\n");
    head = insert_before(head, &e, tmp);
    printf("LIST Print\n");
    printList(head, printInt);
    printf("SORT\n");
    head = insertion_sort(head);
    printf("LIST Print\n");
    printList(head, printInt);
    printf("Reverse\n");
    head = reverse(head);
    printf("LIST Print\n");
    printList(head, printInt);
    printf("Remove front\n");
    head = remove_front(head);
    printf("LIST Print\n");
    printList(head, printInt);
    printf("Dispose list\n");
    dispose(head);
}

void testchar()
{
    node* head = NULL;
    node* tmp = NULL;
    char a = 'a';
    char b = 'b';
    char c = 'c';
    char d = 'd';
    char e = 'e';
    printf("Add a\n");
    create(&head, &a);
    printf("Add b\n");
    create(&head, &b);
    printf("LIST Print\n");
    printList(head, printChar);
    printf("Add c\n");
    create(&head, &c);
    printf("Count\n");
    count(head);
    printf("Addend b\n");
    addend(head, &b);
    tmp = searchch(head, &c);
    printf("Add after c value e\n");
    head = insert_after(head, &e, tmp);
    printf("LIST Print\n");
    printList(head, printChar);
    printf("Add before c value e\n");
    head = insert_before(head, &e, tmp);
    printf("LIST Print\n");
    printList(head, printChar);
    printf("SORT\n");
    head = insertion_sort(head);
    printf("LIST Print\n");
    printList(head, printChar);
    printf("Reverse\n");
    head = reverse(head);
    printf("LIST Print\n");
    printList(head, printChar);
    printf("Remove front\n");
    head = remove_front(head);
    printf("LIST Print\n");
    printList(head, printChar);
    printf("Dispose list\n");
    dispose(head);
}

/*
    display the menu
*/
void menu()
{
    printf("0.menu\n");
    printf("1.create list\n");
    printf("2.append an element\n");
    printf("3.print list\n");
    printf("4.count an element\n");
    printf("5.search for an element\n");
    printf("6.insert after an element\n");
    printf("7.insert before an element\n");
    printf("8.remove front node\n");
    printf("9.remove back node\n");
    printf("10.sort the list\n");
    printf("11.Reverse the linked list\n");
    printf("-1.quit\n");
}

void start()
{
    int command = 0;
    int data;
    node* head = NULL;
    node* tmp = NULL;
    while (1) {
        printf("\nEnter a command(0-10,-1 to quit):");
        if(scanf("%d", &command));

        if (command == -1)
            break;
        switch (command) {
            case 0:
                menu();
                break;
            case 1:
                printf("Please enter a number to prepend:");
                if(scanf("%d", &data));
                create(&head, &data);
                break;
            case 2:
                if(scanf("%d", &data));
                addend(head, &data);
                break;
            case 3:
                printList(head, printInt);
                break;
            case 4:
                count(head);
                break;
            case 5:
                printf("Please enter a number to search:");
                if(scanf("%d", &data));
                tmp = search(head, &data);
                if (tmp != NULL) {
                    printf("Element with value %d found.", data);
                }
                else {
                    printf("Element with value %d not found.", data);
                }
                break;

            case 6:
                printf("Enter the element value where you want to insert after:");
                if(scanf("%d", &data));
                tmp = search(head, &data);
                if (tmp != NULL) {
                    printf("Enter the element value to insert after:");
                    if(scanf("%d", &data));
                    head = insert_after(head, &data, tmp);
                }
                else {
                    printf("Element with value %d not found.", data);
                }
                break;
            case 7:
                printf("Enter the element value where you want to insert before:");
                if(scanf("%d", &data));
                tmp = search(head, &data);
                if (tmp != NULL) {
                    printf("Enter the element value to insert before:");
                    if(scanf("%d", &(data)));
                    head = insert_before(head, &data, tmp);
                }
                else {
                    printf("Element with value %d not found.", data);
                }
                break;
            case 8:
                head = remove_front(head);
                if (head != NULL)
                    break;
            case 9:
                head = remove_back(head);
                if (head != NULL)
                    break;
            case 10:
                head = insertion_sort(head);
                if (head != NULL)
                    break;
            case 11:
                head = reverse(head);
                if (head != NULL)
                    break;
        }
    }
    dispose(head);
}
