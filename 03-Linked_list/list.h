
#ifndef LIST1_LIST_H
#define LIST1_LIST_H

typedef struct node {
    void* data;
    struct node* next;
} node;

typedef void (*callback)(node* data);

void traverse(node* head, callback f);
/**
 * create() - appends one element to the end of the list.
 * @head_ref :	Head our list in which we add node.
 * @new_data:	New data for our node.
 */
node* create(node** head_ref, void* new_data);
/**
 * printList() - function for print elements of list.
 * @node :	    Head our list in which we add node.
 * @fptr():     function for print(int or char).
 */
void printList(node* node, void (*fptr)(void*));
/**
 * printint() - function for print integer elements.
 * @n:	        Date for print.
 */
void printInt(void* n);
/**
 * printchar() - function for print char elements.
 * @n:	        Date for print.
 */
void printChar(void* f);
/**
 * count() - function count of elements.
 * @head:	        Head our list.
 */
int count(node* head);
/**
 * addend() - function add element to end of list
 * @head:	        Head our list in which we add node.
 * @new_data:	New data for our node.
*/
void addend(node* head, void* new_data);
/**
 * search() - function search elements in list(position)
 * @head:	    Head our list in which we add node.
 * @new_data:	Search data
*/
node* search(node* head, void* data);
/**
 * searchch() - function search elements(char) in list(position)
 * @head:	    Head our list.
 * @new_data:	Search data
*/
node* searchch(node* head, void* data);
/**
 * insert_after() - function insert element after the position of the found element
 * @head:	        Head our list.
 * @new_data:	    Search data
 * @prev:           Position after add new elements
*/
node* insert_after(node* head, void* new_data, node* prev);
/**
 * insert_before() - function insert element before the position of the found element
 * @head:	        Head our list.
 * @new_data:	    Search data
 * @prev:           Position before add new elements
*/
node* insert_before(node* head, void* new_data, node* nxt);
/**
 * insertion_sort() - function sort elements in list
 * @head:	        Head our list.
*/
node* insertion_sort(node* head);
/**
 * reverse() - function reverse elements in list
 * @head:	        Head our list.
*/
node* reverse(node* head);
/**
 * remove_back() - function remove last elements in list
 * @head:	        Head our list.
*/
node* remove_back(node* head);
/**
 * remove_front() - function remove first elements in list
 * @head:	        Head our list.
*/
node* remove_front(node* head);
/**
 * dispose() - function dispose last elements in list
 * @head:	        Head our list.
*/
void dispose(node* head);
/**
 * menu() - menu our lib
*/
void menu();
void start();
void testint();
void testchar();

#endif //LIST1_LIST_H
