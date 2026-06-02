#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "parser.h"

// Yazar: Mustafa Ozturk
#define BUFFER_SIZE 65536

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

char* extract_body(char* request) {
    char* body = strstr(request, "\r\n\r\n");
    if (!body) body = strstr(request, "\n\n");
    if (!body) return NULL;
    return body + 4;
}

void send_response(SOCKET client_fd, const char* status, const char* body) {
    char response[BUFFER_SIZE];
    int body_len = strlen(body);
    sprintf(response,
        "HTTP/1.1 %s\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        status, body_len, body
    );
    send(client_fd, response, strlen(response), 0);
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);

    printf("Sunucu hazir. http://localhost:8080 adresini dinliyor...\n");

    while (1) {
        SOCKET client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == INVALID_SOCKET) continue;

        char request[BUFFER_SIZE];
        memset(request, 0, BUFFER_SIZE);
        recv(client_fd, request, BUFFER_SIZE - 1, 0);

        printf("Istek: %.20s...\n", request);

        if (strncmp(request, "OPTIONS", 7) == 0) {
            send_response(client_fd, "200 OK", "{}");
        } else if (strncmp(request, "POST /parse", 11) == 0) {
            char* html = extract_body(request);
            if (html && strlen(html) > 0) {
                DOM* dom = dom_parse(html);
                char json[BUFFER_SIZE];
                int pos = 0;
                node_to_json(dom->root, json, &pos);
                dom_free(dom);
                send_response(client_fd, "200 OK", json);
            } else {
                send_response(client_fd, "400 Bad Request", "{\"error\":\"HTML bos\"}");
            }
        } else {
            send_response(client_fd, "200 OK", "{\"status\":\"DOM Tree Server calisiyor\"}");
        }

        closesocket(client_fd);
        printf("Istek islendi.\n");
    }

    WSACleanup();
    return 0;
}