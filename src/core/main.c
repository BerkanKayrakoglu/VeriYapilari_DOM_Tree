#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// Yazar: Mustafa Ozturk
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

    printf("\n--- JSON Ciktisi ---\n");
    char json_buf[65536];
    int pos = 0;
    node_to_json(dom->root, json_buf, &pos);
    printf("%s\n", json_buf);

    dom_free(dom);
    return 0;
}