#include "parser.h"
#include "stack.h"
#include "queue.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>


// HTML Parser implementasyonu

// Bosluk temizle
static void trim(char* str) {
    int len = strlen(str);
    while (len > 0 && isspace(str[len-1])) str[--len] = '\0';
    int start = 0;
    while (str[start] && isspace(str[start])) start++;
    if (start > 0) memmove(str, str + start, len - start + 1);
}

// id="..." veya class="..." degerini cek
static char* extract_attr(const char* tag, const char* attr) {
    char search[64];
    snprintf(search, sizeof(search), "%s=\"", attr);
    const char* pos = strstr(tag, search);
    if (!pos) return NULL;
    pos += strlen(search);
    const char* end = strchr(pos, '"');
    if (!end) return NULL;
    int len = end - pos;
    char* result = (char*)malloc(len + 1);
    strncpy(result, pos, len);
    result[len] = '\0';
    return result;
}

// Tag adini cek (ornek: "div id=..." -> "div")
static char* extract_tag_name(const char* tag) {
    int i = 0;
    while (tag[i] && !isspace(tag[i])) i++;
    char* name = (char*)malloc(i + 1);
    strncpy(name, tag, i);
    name[i] = '\0';
    return name;
}

DOM* dom_parse(const char* html) {
    DOM* dom = (DOM*)malloc(sizeof(DOM));
    dom->ht = (HashTable*)malloc(sizeof(HashTable));
    hash_init(dom->ht);
    dom->root = NULL;

    Stack s;
    stack_init(&s);

    Node* root = create_node("document");
    dom->root = root;
    stack_push(&s, root);

    const char* p = html;
    while (*p) {
        if (*p == '<') {
            p++;
            // Kapanis etiketi
            if (*p == '/') {
                p++;
                while (*p && *p != '>') p++;
                if (!stack_is_empty(&s)) stack_pop(&s);
            }
            // Yorum satiri
            else if (strncmp(p, "!--", 3) == 0) {
                const char* end = strstr(p, "-->");
                if (end) p = end + 2;
            }
            // Acilis etiketi
            else {
                char tag_buf[512];
                int i = 0;
                int self_closing = 0;
                while (*p && *p != '>') {
                    tag_buf[i++] = *p++;
                }
                tag_buf[i] = '\0';

                // Self-closing kontrol (<br/>, <img/>)
                if (i > 0 && tag_buf[i-1] == '/') {
                    tag_buf[i-1] = '\0';
                    self_closing = 1;
                }

                trim(tag_buf);
                char* tag_name = extract_tag_name(tag_buf);
                Node* node = create_node(tag_name);
                free(tag_name);

                // id ve class ata
                node->id = extract_attr(tag_buf, "id");
                node->class_name = extract_attr(tag_buf, "class");

                // Hash table'a ekle
                if (node->id) {
                    hash_insert(dom->ht, node->id, node);
                }

                // Parent'a bagla
                Node* parent = stack_peek(&s);
                if (parent) add_child(parent, node);

                if (!self_closing) {
                    stack_push(&s, node);
                }
            }
        }
        p++;
    }

    return dom;
}

void dom_free(DOM* dom) {
    if (!dom) return;
    hash_free(dom->ht);
    free(dom->ht);
    free_tree(dom->root);
    free(dom);
}

Node* dom_get_by_id(DOM* dom, const char* id) {
    return hash_get(dom->ht, id);
}

// BFS ile class arama
Node** dom_get_by_class(DOM* dom, const char* class_name, int* count) {
    Node** result = (Node**)malloc(sizeof(Node*) * 1000);
    *count = 0;

    Queue q;
    queue_init(&q);
    queue_enqueue(&q, dom->root);

    while (!queue_is_empty(&q)) {
        Node* curr = queue_dequeue(&q);
        if (curr->class_name && strcmp(curr->class_name, class_name) == 0) {
            result[(*count)++] = curr;
        }
        for (int i = 0; i < curr->child_count; i++) {
            queue_enqueue(&q, curr->children[i]);
        }
    }
    return result;
}

int dom_depth(Node* node) {
    if (!node || node->child_count == 0) return 0;
    int max = 0;
    for (int i = 0; i < node->child_count; i++) {
        int d = dom_depth(node->children[i]);
        if (d > max) max = d;
    }
    return max + 1;
}

void dom_print(Node* root, int level) {
    if (!root) return;
    for (int i = 0; i < level; i++) printf("  ");
    printf("<%s", root->tag);
    if (root->id) printf(" id=\"%s\"", root->id);
    if (root->class_name) printf(" class=\"%s\"", root->class_name);
    printf(">\n");
    for (int i = 0; i < root->child_count; i++) {
        dom_print(root->children[i], level + 1);
    }
}

// BFS traversal
Node** dom_bfs(DOM* dom, int* count) {
    Node** result = (Node**)malloc(sizeof(Node*) * 1000);
    *count = 0;

    Queue q;
    queue_init(&q);
    queue_enqueue(&q, dom->root);

    while (!queue_is_empty(&q)) {
        Node* curr = queue_dequeue(&q);
        result[(*count)++] = curr;
        for (int i = 0; i < curr->child_count; i++) {
            queue_enqueue(&q, curr->children[i]);
        }
    }
    return result;
}

// DFS traversal (iteratif)
Node** dom_dfs(DOM* dom, int* count) {
    Node** result = (Node**)malloc(sizeof(Node*) * 1000);
    *count = 0;

    Stack s;
    stack_init(&s);
    stack_push(&s, dom->root);

    while (!stack_is_empty(&s)) {
        Node* curr = stack_pop(&s);
        result[(*count)++] = curr;
        for (int i = curr->child_count - 1; i >= 0; i--) {
            stack_push(&s, curr->children[i]);
        }
    }
    return result;
}