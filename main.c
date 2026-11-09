#define MG_ENABLE_HTTP 1
#include "mongoose.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[100];
} Student;

#define MAX_STUDENTS 100
Student students[MAX_STUDENTS];
int student_count = 0;
int next_id = 1;

void init_data() {
    students[0].id = next_id++;
    strcpy(students[0].name, "Awa Diop");
    student_count = 1;
}

static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        // GET /
        if (mg_match(hm->uri, mg_str("/"), NULL)) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"message\": \"CS121 API is running\"}\n");
        }
        // GET /students
        else if (mg_match(hm->uri, mg_str("/students"), NULL) && mg_vcasecmp(&hm->method, "GET") == 0) {
            char buf[4096] = "[";
            for (int i = 0; i < student_count; i++) {
                char item[256];
                snprintf(item, sizeof(item), "%s{\"id\": %d, \"name\": \"%s\"}",
                         (i > 0 ? "," : ""), students[i].id, students[i].name);
                strcat(buf, item);
            }
            strcat(buf, "]\n");
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", buf);
        }
        // POST /students
        else if (mg_match(hm->uri, mg_str("/students"), NULL) && mg_vcasecmp(&hm->method, "POST") == 0) {
            char name[100] = {0};
            mg_json_get_str(hm->body, "$.name", name, sizeof(name));

            if (strlen(name) == 0) {
                mg_http_reply(c, 400, "Content-Type: application/json\r\n", "{\"error\": \"Name is required\"}\n");
                return;
            }

            students[student_count].id = next_id++;
            strncpy(students[student_count].name, name, sizeof(students[student_count].name) - 1);
            student_count++;

            mg_http_reply(c, 201, "Content-Type: application/json\r\n",
                          "{\"id\": %d, \"name\": \"%s\"}\n", students[student_count - 1].id, students[student_count - 1].name);
        }
        // GET /students/:id , PUT /students/:id , DELETE /students/:id
        else if (mg_match(hm->uri, mg_str("/students/*"), NULL)) {
            int target_id = atoi(hm->uri.buf + 10);
            int found_idx = -1;

            for (int i = 0; i < student_count; i++) {
                if (students[i].id == target_id) {
                    found_idx = i;
                    break;
                }
            }

            if (found_idx == -1) {
                mg_http_reply(c, 404, "Content-Type: application/json\r\n", "{\"error\": \"Student not found\"}\n");
                return;
            }

            if (mg_vcasecmp(&hm->method, "GET") == 0) {
                mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                              "{\"id\": %d, \"name\": \"%s\"}\n", students[found_idx].id, students[found_idx].name);
            } 
            else if (mg_vcasecmp(&hm->method, "PUT") == 0) {
                char name[100] = {0};
                mg_json_get_str(hm->body, "$.name", name, sizeof(name));

                if (strlen(name) == 0) {
                    mg_http_reply(c, 400, "Content-Type: application/json\r\n", "{\"error\": \"Name is required\"}\n");
                    return;
                }

                strncpy(students[found_idx].name, name, sizeof(students[found_idx].name) - 1);
                mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                              "{\"id\": %d, \"name\": \"%s\"}\n", students[found_idx].id, students[found_idx].name);
            } 
            else if (mg_vcasecmp(&hm->method, "DELETE") == 0) {
                for (int i = found_idx; i < student_count - 1; i++) {
                    students[i] = students[i + 1];
                }
                student_count--;
                mg_http_reply(c, 204, "", "");
            }
        } 
        else {
            mg_http_reply(c, 404, "Content-Type: application/json\r\n", "{\"error\": \"Not Found\"}\n");
        }
    }
}

int main() {
    init_data();
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    const char *port = getenv("PORT");
    if (port == NULL) port = "3000";

    char listen_addr[64];
    snprintf(listen_addr, sizeof(listen_addr), "http://0.0.0.0:%s", port);

    printf("CS121 API listening on %s\n", listen_addr);
    mg_http_listen(&mgr, listen_addr, fn, NULL);

    for (;;) mg_mgr_poll(&mgr, 1000);

    mg_mgr_free(&mgr);
    return 0;
}