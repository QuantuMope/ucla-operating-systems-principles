/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
    // If list is only the header.
    if ((list->next) == list) {
        // TODO
        ;
    }
    SortedList_t *curr = list->next;
    SortedList_t *temp;
    while (*(curr->next->key) != NULL) {
        if (*(curr->next->key) >= *(element->key)) {
            temp = curr->prev;
            curr->prev = element;
            element->next = curr;
            element->prev = temp;
            temp->next = element;
            break;
        }
        curr = curr->next;
    }
}

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *	The specified element will be removed from whatever
 *	list it is currently in.
 *
 *	Before doing the deletion, we check to make sure that
 *	next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 */
int SortedList_delete( SortedListElement_t *element) {
    if ((element->next->prev) != element || (element->prev->next) != element) {
        fprintf(stderr, "Corrupted prev/next pointers.\n");
        return 1;
    }
    element->next->prev = element->prev;
    element->prev->next = element->next;

    return 0;
}

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *	The specified list will be searched for an
 *	element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
    SortedList_t *curr = list->next;
    while (*(curr->key) != NULL) {
        if (*(curr->key) == *key)  {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

/**
 * SortedList_length ... count elements in a sorted list
 *	While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *	   -1 if the list is corrupted
 */
int SortedList_length(SortedList_t *list) {
    SortedList_t *curr = list->next;
    int length = 0;
    while (*(curr->key) != NULL) {
        if ((curr->next->prev) != curr || (curr->prev->next) != curr) {
            fprintf(stderr, "Corrupted prev/next pointer detected.\n");
            return -1;
        }
        curr = curr->next;
        length++;
    }
    return length;
}
