#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <pthread.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define closesocket close
#endif
#include "parser.h"

#define REQ_SIZE  524288
#define JSON_SIZE 4194304

void node_to_json(Node* node, char* buf, int* pos) {
    if (!node || !node->tag) return;
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
    if (body) return body + 4;
    body = strstr(request, "\n\n");
    if (body) return body + 2;
    return NULL;
}

void send_response(SOCKET client_fd, const char* status, const char* body) {
    int body_len = strlen(body);
    char* response = (char*)malloc(body_len + 512);
    int total = sprintf(response,
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
    int sent = 0;
    while (sent < total) {
        int n = send(client_fd, response + sent, total - sent, 0);
        if (n <= 0) break;
        sent += n;
    }
    free(response);
}

void handle_client(SOCKET client_fd) {
    char* request = (char*)malloc(REQ_SIZE);
    if (!request) { closesocket(client_fd); return; }
    memset(request, 0, REQ_SIZE);

    int bytes_recv   = 0;
    int content_length = 0;
    int headers_end  = 0;
    while (bytes_recv < REQ_SIZE - 1) {
        int n = recv(client_fd, request + bytes_recv, REQ_SIZE - 1 - bytes_recv, 0);
        if (n <= 0) break;
        bytes_recv += n;
        if (!headers_end) {
            char* hend = strstr(request, "\r\n\r\n");
            if (hend) {
                headers_end = (int)(hend + 4 - request);
                char* cl = strstr(request, "Content-Length: ");
                if (cl) content_length = atoi(cl + 16);
            }
        }
        if (headers_end > 0 && bytes_recv >= headers_end + content_length) break;
    }

    printf("Istek [Thread]: %.30s...\n", request);

    if (strncmp(request, "OPTIONS", 7) == 0) {
        send_response(client_fd, "200 OK", "{}");

    } else if (strncmp(request, "POST /parse", 11) == 0) {
        char* html = extract_body(request);
        if (html && strlen(html) > 0) {
            DOM* dom = dom_parse(html);
            char* json = (char*)malloc(JSON_SIZE);
            int pos = 0;
            node_to_json(dom->root, json, &pos);
            json[pos] = '\0';
            dom_free(dom);
            send_response(client_fd, "200 OK", json);
            free(json);
        } else {
            send_response(client_fd, "400 Bad Request", "{\"error\":\"HTML bos\"}");
        }

    } else if (strncmp(request, "POST /search", 12) == 0) {
        char* body = extract_body(request);
        if (body && strlen(body) > 0) {
            char* newline = strchr(body, '\n');
            if (newline) {
                char qtype[64] = {0}, qvalue[512] = {0};
                int qlen = newline - body;
                char qline[580] = {0};
                if (qlen < 579) {
                    strncpy(qline, body, qlen);
                    char* colon = strchr(qline, ':');
                    if (colon) {
                        int tlen = colon - qline;
                        strncpy(qtype, qline, tlen);
                        strcpy(qvalue, colon + 1);
                    }
                }
                char* html = newline + 1;
                if (strlen(html) > 0 && strlen(qtype) > 0 && strlen(qvalue) > 0) {
                    DOM* dom = dom_parse(html);
                    char* json = (char*)malloc(JSON_SIZE);
                    int pos = 0;
                    if (strcmp(qtype, "id") == 0) {
                        Node* node = dom_get_by_id(dom, qvalue);
                        pos += sprintf(json + pos, "[");
                        if (node) node_to_json(node, json, &pos);
                        pos += sprintf(json + pos, "]");
                    } else if (strcmp(qtype, "class") == 0) {
                        int count = 0;
                        Node** nodes = dom_get_by_class(dom, qvalue, &count);
                        pos += sprintf(json + pos, "[");
                        for (int i = 0; i < count; i++) {
                            if (i > 0) pos += sprintf(json + pos, ",");
                            node_to_json(nodes[i], json, &pos);
                        }
                        pos += sprintf(json + pos, "]");
                        free(nodes);
                    } else {
                        strcpy(json, "[]"); pos = 2;
                    }
                    json[pos] = '\0';
                    dom_free(dom);
                    send_response(client_fd, "200 OK", json);
                    free(json);
                } else {
                    send_response(client_fd, "400 Bad Request", "{\"error\":\"Gecersiz sorgu\"}");
                }
            } else {
                send_response(client_fd, "400 Bad Request", "{\"error\":\"Sorgu satiri eksik\"}");
            }
        } else {
            send_response(client_fd, "400 Bad Request", "{\"error\":\"Govde bos\"}");
        }

    } else if (strncmp(request, "POST /analyze", 13) == 0) {
        char* html = extract_body(request);
        if (html && strlen(html) > 0) {
            DOM* dom = dom_parse(html);
            int depth       = dom_depth(dom->root);
            int node_total  = dom_subtree_node_count(dom->root);
            int bfs_count = 0, dfs_count = 0;
            Node** bfs_nodes = dom_bfs(dom, &bfs_count);
            Node** dfs_nodes = dom_dfs(dom, &dfs_count);
            char* json = (char*)malloc(JSON_SIZE);
            int pos = 0;
            pos += sprintf(json + pos,
                "{\"depth\":%d,\"total_nodes\":%d,\"bfs_order\":[", depth, node_total);
            int lim = bfs_count < 15 ? bfs_count : 15;
            for (int i = 0; i < lim; i++) {
                if (i > 0) pos += sprintf(json + pos, ",");
                if (bfs_nodes[i] && bfs_nodes[i]->tag)
                    pos += sprintf(json + pos, "\"%s\"", bfs_nodes[i]->tag);
            }
            pos += sprintf(json + pos, "],\"dfs_order\":[");
            lim = dfs_count < 15 ? dfs_count : 15;
            for (int i = 0; i < lim; i++) {
                if (i > 0) pos += sprintf(json + pos, ",");
                if (dfs_nodes[i] && dfs_nodes[i]->tag)
                    pos += sprintf(json + pos, "\"%s\"", dfs_nodes[i]->tag);
            }
            pos += sprintf(json + pos, "]}");
            json[pos] = '\0';
            free(bfs_nodes);
            free(dfs_nodes);
            dom_free(dom);
            send_response(client_fd, "200 OK", json);
            free(json);
        } else {
            send_response(client_fd, "400 Bad Request", "{\"error\":\"HTML bos\"}");
        }

    } else {
        send_response(client_fd, "200 OK", "{\"status\":\"DOM Tree Server calisiyor\"}");
    }

    free(request);
    closesocket(client_fd);
    printf("Istek islendi.\n");
}

#ifdef _WIN32
DWORD WINAPI client_thread(LPVOID arg) {
    SOCKET client_fd = (SOCKET)(intptr_t)arg;
    handle_client(client_fd);
    return 0;
}
#else
void* client_thread(void* arg) {
    SOCKET client_fd = (SOCKET)(intptr_t)arg;
    handle_client(client_fd);
    return NULL;
}
#endif

int main() {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Winsock baslatilamadi.\n");
        return 1;
    }
#endif

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 64);

    printf("Sunucu hazir (multi-thread). http://localhost:8080 adresini dinliyor...\n");

    while (1) {
        SOCKET client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == INVALID_SOCKET) continue;

#ifdef _WIN32
        HANDLE t = CreateThread(NULL, 0, client_thread, (LPVOID)(intptr_t)client_fd, 0, NULL);
        if (t) CloseHandle(t);
#else
        pthread_t t;
        pthread_create(&t, NULL, client_thread, (void*)(intptr_t)client_fd);
        pthread_detach(t);
#endif
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
