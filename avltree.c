#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define max(x, y) (((x) > (y)) ? (x) : (y))

typedef struct avlnode* avltree;

struct avlnode {
    int key;
    int data;
    avltree left;
    avltree right;
    int balance;
    int height;
};

avltree find (avltree head, int key) {
    if(head == NULL || head->key == key) {
        return head;
    }
    if(head->key > key) {
        return find(head->left, key);
    } else {
        return find(head->right, key);
    }
}

int get_data(avltree head, int key, int* data) {
    avltree node = find(head, key);
    if(node == NULL) {
        return 0;
    } else {
        *data = node->data;
        return 1;
    }

    return node? node->data : 0;
}

avltree find_min(avltree head) {
    if(head != NULL) {
        while (head->left != NULL) {
            head = head->left;
        }
    }
    return head;
}

avltree find_max(avltree head) {
    if(head != NULL) {
        while (head->left != NULL) {
            head = head->left;
        }
    }
    return head;
}

static inline int h(avltree head) {
    return head? head->height : 0;
}

static avltree left_single_rotate(avltree head) {
    avltree left = head->left;

    head->left = left->right;
    left->right = head;

    head->height = 1 + max(h(head->left), h(head->right));
    left->height = 1 + max(h(left->left), h(left->right));

    head->balance = h(head->right) - h(head->left);
    left->balance = h(left->right) - h(left->left);

    return left;
}

static avltree right_single_rotate(avltree head) {
    avltree right = head->right;

    head->right = right->left;
    right->left = head;

    head->height = 1 + max(h(head->left), h(head->right));
    right->height = 1 + max(h(right->left), h(right->right));

    head->balance = h(head->right) - h(head->left);
    right->balance = h(right->right) - h(right->left);

    return right;
}

static avltree left_double_rotate(avltree head) {
    head->left = right_single_rotate(head->left);

    head = left_single_rotate(head);

    return head;
}

static avltree right_double_rotate(avltree head) {
    head->right = left_single_rotate(head->right);

    head = right_single_rotate(head);

    return head;
}

avltree insert(avltree head, int key, int data) {
    if(head == NULL) {
        avltree node = malloc(sizeof(struct avlnode));
        node->left = NULL;
        node->right = NULL;
        node->height = 1;
        node->balance = 0;
        node->key = key;
        node->data = data;
        return node;
    }

    if(head->key > key) {
        head->left = insert(head->left, key, data);
        head->height = 1 + max(h(head->left), h(head->right));
        head->balance = h(head->right) - h(head->left);

        if(head->balance <= -2) {
            if(head->left->key > key) {
                head = left_single_rotate(head);
            } else {
                head = left_double_rotate(head);
            }
        }
    } else if (head->key < key){
        head->right = insert(head->right, key, data);
        head->height = 1 + max(h(head->left), h(head->right));
        head->balance = h(head->right) - h(head->left);

        if(head->balance >= 2) {
            if(head->right->key < key) {
                head = right_single_rotate(head);
            } else {
                head = right_double_rotate(head);
            }
        }
    } else {
        head->data = data;
    }

    return head;
}

void print_tree(avltree head, int lvl) {
    if(head == NULL) {
        return;
    }

    for(int i = 0; i < lvl; i++) {
        printf("    ");
    }
    printf("[key = %d data = %d balance = %d h = %d]\n", head->key, head->data, head->balance, head->height);

    print_tree(head->left, lvl + 1);
    print_tree(head->right, lvl + 1);
}

avltree retrieve_min(avltree head, avltree* min) {
    if(head->left == NULL) {
        *min = head;
        return head->right;
    }

    head->left = retrieve_min(head->left, min);
    head->height = 1 + max(h(head->left), h(head->right));
    head->balance = h(head->right) - h(head->left);
    if(head->balance >= 2) {
        if(head->right->balance >= 0) {
            head = right_single_rotate(head);
        } else {
            head = right_double_rotate(head);
        }
    }

    return head;
}

avltree retrieve(avltree head, int key, avltree* elem) {
    if(head == NULL) {
        return NULL;
    }

    if(key < head->key) {
        head->left = retrieve(head->left, key, elem);
        head->height = 1 + max(h(head->left), h(head->right));
        head->balance = h(head->right) - h(head->left);
        if(head->balance >= 2) {
            if(head->right->balance >= 0) {
                head = right_single_rotate(head);
            } else {
                head = right_double_rotate(head);
            }
        }
    } else if (key > head->key) {
        head->right = retrieve(head->right, key, elem);
        head->height = 1 + max(h(head->left), h(head->right));
        head->balance = h(head->right) - h(head->left);
        if(head->balance <= -2) {
            if(head->left->balance <= 0) {
                head = left_single_rotate(head);
            } else {
                head = left_double_rotate(head);
            }
        }
    } else {
        *elem = head;
        if(head->left == NULL || head->right == NULL) {
            if(head->left == NULL) {
                return head->right;
            } else {
                return head->left;
            }
        } else {
            avltree min;
            head->right = retrieve_min(head->right, &min);

            min->right = head->right;
            min->left = head->left;

            min->height = 1 + max(h(head->left), h(head->right));
            min->balance = h(head->right) - h(head->left);
            head = min;
            if(min->balance <= -2) {
                if(min->left->balance <= 0) {
                    min = left_single_rotate(head);
                } else {
                    min = left_double_rotate(head);
                }
            }
        }
    }

    return head;
}

int retrieve_data(avltree* head, int key, int* data) {
    avltree elem;
    *head = retrieve(*head, key, &elem);

    if(elem != NULL) {
        *data = elem->data;
        free(elem);
        return 1;
    } else {
        return 0;
    }
}
int main(void) {
    avltree head = NULL;
    char c = 'N';

    while (c != 'F') {
        int key, data;

        c = getchar();
        switch (c) {
            case 'A':
                scanf("%d %d", &key, &data);
                getchar();
                head = insert(head, key, data);
                break;
            case 'S':
                scanf("%d", &key);
                getchar();
                if(get_data(head, key, &data)) {
                    printf("%d %d ", key, data);
                }
                break;
            case 'D':
                scanf("%d", &key);
                getchar();
                retrieve_data(&head, key, &data);
                break;
            default:
                break;
        }
    }



//    head = insert(head, 1, 10);
//    head = insert(head, 2, 20);
//    head = insert(head, 3, 30);
//    head = insert(head, 4, 40);
//    head = insert(head, 5, 50);
//    head = insert(head, 7, 70);
//    head = insert(head, 8, 80);
//    head = insert(head, 9, 90);
//    head = insert(head, 10, 100);
//
//
//    print_tree(head,0);
//
//    int key;
//
//    key = 10;
//    printf("[retrieved: key = %d, data = %d]\n", key, retrieve_data(&head, key));
//
//    key = 1;
//    printf("[retrieved: key = %d, data = %d]\n", key, retrieve_data(&head, key));
//
//    key = 5;
//    printf("[retrieved: key = %d, data = %d]\n", key, retrieve_data(&head, key));
//
//    print_tree(head,0);
//

    return 0;
}