#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// Ana program - HTTP isteklerini karsilar, DOM agaci olusturur, JSON doner

// DOM agacini JSON formatina donustur
void node_to_json(Node* node, char* buf, int* pos) {
    *pos += sprintf(buf + *pos, "{\"tag\":\"%s\"", node->tag);
    if (node->id) *pos += sprintf(buf + *pos, ",\"id\":\"%s\"", node->id);
    if (node->class_name) *pos += sprintf(buf + *pos, ",\"class\":\"%s\"", node->class_name);
    
    *pos += sprintf(buf + *pos, ",\"children\":[");
    for (int i = 0; i < node->child_count; i++) {
        if (i > 0) *pos += sprintf(buf + *pos, ",");
        node_to_json(node->children[i], buf, pos);
    }
    *pos += sprintf(buf + *pos, "]}");
}

int main() {
    // Test HTML
    const char* html = 
        "<html>"
            "<head>"
                "<title></title>"
            "</head>"
            "<body id=\"main\">"
                "<div class=\"container\">"
                    "<h1 id=\"baslik\"></h1>"
                    "<p class=\"container\"></p>"
                "</div>"
            "</body>"
        "</html>";

    printf("=== DOM Agaci Parser Test ===\n\n");

    DOM* dom = dom_parse(html);

    printf("--- Agac Yapisi ---\n");
    dom_print(dom->root, 0);

    printf("\n--- getElementById('baslik') ---\n");
    Node* found = dom_get_by_id(dom, "baslik");
    if (found) printf("Bulundu: <%s>\n", found->tag);
    else printf("Bulunamadi\n");

    printf("\n--- getElementsByClass('container') ---\n");
    int count = 0;
    Node** nodes = dom_get_by_class(dom, "container", &count);
    printf("%d dugum bulundu\n", count);
    for (int i = 0; i < count; i++) {
        printf("  <%s>\n", nodes[i]->tag);
    }
    free(nodes);

    printf("\n--- Agac Derinligi ---\n");
    printf("Derinlik: %d\n", dom_depth(dom->root));

    printf("\n--- Alt Agac Analizi (Subtree Count) ---\n");
    printf("Toplam dugum sayisi (root): %d\n", dom_subtree_node_count(dom->root));
    Node* body_node = dom_get_by_id(dom, "main");
    if (body_node) {
        printf("body alt agacindaki dugum sayisi: %d\n", dom_subtree_node_count(body_node));
    }

    printf("\n--- Kardes Dugumleri Bulma (Siblings) ---\n");
    Node* baslik_node = dom_get_by_id(dom, "baslik");
    if (baslik_node) {
        int sib_count = 0;
        Node** siblings = dom_get_siblings(baslik_node, &sib_count);
        printf("<%s id=\"%s\"> dugumunun %d kardesi var:\n", baslik_node->tag, baslik_node->id ? baslik_node->id : "", sib_count);
        for (int i = 0; i < sib_count; i++) {
            printf("  kardes: <%s class=\"%s\">\n", siblings[i]->tag, siblings[i]->class_name ? siblings[i]->class_name : "");
        }
        free(siblings);
    }

    printf("\n--- JSON Ciktisi ---\n");
    char json_buf[65536];
    int pos = 0;
    node_to_json(dom->root, json_buf, &pos);
    printf("%s\n", json_buf);

    dom_free(dom);
    return 0;
}