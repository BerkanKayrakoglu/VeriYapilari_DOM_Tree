#ifndef N_ARY_TREE_H
#define N_ARY_TREE_H

// DOM Düğümünü (Node) temsil edecek yapı
typedef struct Node {
    char* tag;
    char* id;
    char* class_name;
    struct Node* parent;
    struct Node** children;
    int child_count;
    int child_capacity;
} Node;

// Ağaç fonksiyonlarının prototipleri
Node* create_node(const char* tag);
void add_child(Node* parent, Node* child);
void free_tree(Node* root);

#endif // N_ARY_TREE_H
