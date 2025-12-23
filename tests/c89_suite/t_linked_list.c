/*
 * Test: Linked list operations
 * Tests: insert, delete, search, reverse, complex pointer manipulation
 */
#include <stdio.h>

struct node {
    int data;
    struct node *next;
};

/* Static node pool (no malloc in SubC tests) */
struct node node_pool[20];
int pool_index;

struct node *alloc_node(int data) {
    struct node *n;
    if (pool_index >= 20) {
        return 0;
    }
    n = &node_pool[pool_index++];
    n->data = data;
    n->next = 0;
    return n;
}

void reset_pool(void) {
    pool_index = 0;
}

/* Insert at head */
struct node *insert_head(struct node *head, int data) {
    struct node *n;
    n = alloc_node(data);
    if (n == 0) return head;
    n->next = head;
    return n;
}

/* Insert at tail */
struct node *insert_tail(struct node *head, int data) {
    struct node *n, *curr;
    n = alloc_node(data);
    if (n == 0) return head;
    if (head == 0) return n;
    curr = head;
    while (curr->next != 0) {
        curr = curr->next;
    }
    curr->next = n;
    return head;
}

/* Insert sorted (ascending) */
struct node *insert_sorted(struct node *head, int data) {
    struct node *n, *curr, *prev;
    n = alloc_node(data);
    if (n == 0) return head;
    if (head == 0 || data < head->data) {
        n->next = head;
        return n;
    }
    prev = head;
    curr = head->next;
    while (curr != 0 && curr->data < data) {
        prev = curr;
        curr = curr->next;
    }
    prev->next = n;
    n->next = curr;
    return head;
}

/* Delete first occurrence */
struct node *delete_node(struct node *head, int data) {
    struct node *curr, *prev;
    if (head == 0) return 0;
    if (head->data == data) {
        return head->next;
    }
    prev = head;
    curr = head->next;
    while (curr != 0) {
        if (curr->data == data) {
            prev->next = curr->next;
            return head;
        }
        prev = curr;
        curr = curr->next;
    }
    return head;
}

/* Search for value */
struct node *search(struct node *head, int data) {
    struct node *curr;
    curr = head;
    while (curr != 0) {
        if (curr->data == data) {
            return curr;
        }
        curr = curr->next;
    }
    return 0;
}

/* Count nodes */
int count_nodes(struct node *head) {
    int count;
    struct node *curr;
    count = 0;
    curr = head;
    while (curr != 0) {
        count++;
        curr = curr->next;
    }
    return count;
}

/* Sum all values */
int sum_list(struct node *head) {
    int sum;
    struct node *curr;
    sum = 0;
    curr = head;
    while (curr != 0) {
        sum += curr->data;
        curr = curr->next;
    }
    return sum;
}

/* Reverse list */
struct node *reverse_list(struct node *head) {
    struct node *prev, *curr, *next;
    prev = 0;
    curr = head;
    while (curr != 0) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

/* Get nth element (0-indexed) */
struct node *get_nth(struct node *head, int n) {
    struct node *curr;
    int i;
    curr = head;
    for (i = 0; i < n && curr != 0; i++) {
        curr = curr->next;
    }
    return curr;
}

/* Check if list is sorted */
int is_sorted(struct node *head) {
    struct node *curr;
    if (head == 0) return 1;
    curr = head;
    while (curr->next != 0) {
        if (curr->data > curr->next->data) {
            return 0;
        }
        curr = curr->next;
    }
    return 1;
}

int main(void) {
    struct node *list, *found, *n;
    int passed, count, sum;

    passed = 1;
    reset_pool();

    /* Build list: 1 -> 2 -> 3 -> 4 -> 5 */
    list = 0;
    list = insert_tail(list, 1);
    list = insert_tail(list, 2);
    list = insert_tail(list, 3);
    list = insert_tail(list, 4);
    list = insert_tail(list, 5);

    /* Test count */
    count = count_nodes(list);
    if (count != 5) {
        printf("FAIL: count = %d (expected 5)\n", count);
        passed = 0;
    }

    /* Test sum */
    sum = sum_list(list);
    if (sum != 15) {
        printf("FAIL: sum = %d (expected 15)\n", sum);
        passed = 0;
    }

    /* Test search */
    found = search(list, 3);
    if (found == 0 || found->data != 3) {
        printf("FAIL: search(3) failed\n");
        passed = 0;
    }

    found = search(list, 99);
    if (found != 0) {
        printf("FAIL: search(99) should return NULL\n");
        passed = 0;
    }

    /* Test get_nth */
    n = get_nth(list, 0);
    if (n == 0 || n->data != 1) {
        printf("FAIL: get_nth(0) = %d (expected 1)\n", n ? n->data : -1);
        passed = 0;
    }

    n = get_nth(list, 4);
    if (n == 0 || n->data != 5) {
        printf("FAIL: get_nth(4) = %d (expected 5)\n", n ? n->data : -1);
        passed = 0;
    }

    /* Test is_sorted */
    if (!is_sorted(list)) {
        printf("FAIL: list should be sorted\n");
        passed = 0;
    }

    /* Test insert_head */
    list = insert_head(list, 0);
    if (list->data != 0 || count_nodes(list) != 6) {
        printf("FAIL: insert_head(0)\n");
        passed = 0;
    }

    /* Test delete */
    list = delete_node(list, 3);
    count = count_nodes(list);
    if (count != 5) {
        printf("FAIL: after delete(3): count = %d\n", count);
        passed = 0;
    }
    found = search(list, 3);
    if (found != 0) {
        printf("FAIL: 3 should be deleted\n");
        passed = 0;
    }

    /* Test reverse */
    reset_pool();
    list = 0;
    list = insert_tail(list, 1);
    list = insert_tail(list, 2);
    list = insert_tail(list, 3);
    
    list = reverse_list(list);
    if (list->data != 3) {
        printf("FAIL: after reverse, head = %d (expected 3)\n", list->data);
        passed = 0;
    }
    n = get_nth(list, 2);
    if (n == 0 || n->data != 1) {
        printf("FAIL: after reverse, tail = %d (expected 1)\n", n ? n->data : -1);
        passed = 0;
    }

    /* Test insert_sorted */
    reset_pool();
    list = 0;
    list = insert_sorted(list, 5);
    list = insert_sorted(list, 2);
    list = insert_sorted(list, 8);
    list = insert_sorted(list, 1);
    list = insert_sorted(list, 6);

    if (!is_sorted(list)) {
        printf("FAIL: insert_sorted list not sorted\n");
        passed = 0;
    }

    if (list->data != 1) {
        printf("FAIL: sorted list head = %d (expected 1)\n", list->data);
        passed = 0;
    }

    n = get_nth(list, 4);
    if (n == 0 || n->data != 8) {
        printf("FAIL: sorted list tail = %d (expected 8)\n", n ? n->data : -1);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_linked_list\n");
    }
    return passed ? 0 : 1;
}
