#define MG_ENABLE_HTTP 1
#include "mongoose.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        if (mg_match(hm->uri, mg_str("/"), NULL)) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"message\": \"CS121 API is running\"}\n");
        } else {
            mg_http_reply(c, 404, "Content-Type: application/json\r\n", "{\"error\": \"Not Found\"}\n");
        }
    }
}

int main(void) {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    const char *port = getenv("PORT");
    if (port == NULL) port = "3000";

    char listen_addr[64];
    snprintf(listen_addr, sizeof(listen_addr), "http://0.0.0.0:%s", port);

    printf("Server listening on %s\n", listen_addr);
    mg_http_listen(&mgr, listen_addr, fn, NULL);

    for (;;) mg_mgr_poll(&mgr, 1000);
    mg_mgr_free(&mgr);
    return 0;
}