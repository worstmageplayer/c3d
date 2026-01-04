#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "cJSON.h"
#include "hyprland.h"
#include "error.h"

int ConnectHyprlandSocket(void) {
    const char *runtime = getenv("XDG_RUNTIME_DIR");
    const char *hyprland = getenv("HYPRLAND_INSTANCE_SIGNATURE");

    if (!runtime || !hyprland) {
        fprintf(stderr, "Hyprland environment variables not set\n");
        return -1;
    }

    char path[512];
    snprintf(path, sizeof(path), "%s/hypr/%s/.socket.sock", runtime, hyprland);

    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;

    if (strlen(path) >= sizeof(addr.sun_path)) {
        perror("strlen");
        close(sock);
        return -1;
    }
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(sock);
        return -1;
    }

    return sock;
}

void SendCommand(int sock, const char *cmd) {
    if (sock < 0 || !cmd) return;

    size_t len = strlen(cmd);
    size_t written = 0;
    size_t n;

    while (written < len) {
        n = write(sock, cmd + written, len - written);
        if (n == -1) {
            if (errno == EINTR) continue;
            perror("write");
            return;
        }
        written += (size_t)n;
    }
}

char *GetReply(int sock) {
    if (sock < 0) return NULL;

    size_t bufferSize = 8192;
    char *buffer = malloc(bufferSize);
    size_t n;
    size_t total = 0;

    while (total < bufferSize - 1) {
        n = read(sock, buffer + total, bufferSize - total);
        if (n > 0) {
            total += n;
        } else if (n == 0) {
            break;
        } else {
            perror("read");
            free(buffer);
            break;
        }
        if (buffer[total - 1] == '\0') break;
    }
    buffer[total] = '\0';

    return buffer;
}

char *GetAddressByTitle(char *JSONBuffer, char *windowTitle) {
    if (!JSONBuffer || !windowTitle) return NULL;

    cJSON *root = cJSON_Parse(JSONBuffer);
    if (!root || !cJSON_IsArray(root)) {
        fprintf(stderr, "Invalid JSON (expected array)\n");
        cJSON_Delete(root);
        return NULL;
    }

    char *address = NULL;
    cJSON *client = NULL;
    cJSON_ArrayForEach(client, root) {
        cJSON *title = cJSON_GetObjectItemCaseSensitive(client, "title");

        if (!cJSON_IsString(title) || !title->valuestring) continue;
        if (!(strcmp(title->valuestring, windowTitle) == 0)) continue;

        cJSON *addr = cJSON_GetObjectItemCaseSensitive(client, "address");
        if (cJSON_IsString(addr) && addr->valuestring) {
            address = strdup(addr->valuestring);
        }
        break;
    }

    cJSON_Delete(root);
    return address;
}

int *GetPosByAddress(char *JSONBuffer, char *windowAddress) {
    if (!JSONBuffer || !windowAddress) return NULL;

    cJSON *root = cJSON_Parse(JSONBuffer);
    if (!root || !cJSON_IsArray(root)) {
        fprintf(stderr, "Invalid JSON (expected array)\n");
        cJSON_Delete(root);
        return NULL;
    }

    int *pos = (int*)malloc(sizeof(int)*2);
    cJSON *client = NULL;
    cJSON_ArrayForEach(client, root) {
        cJSON *address = cJSON_GetObjectItemCaseSensitive(client, "address");

        if (!cJSON_IsString(address) || !address->valuestring) continue;
        if (!(strcmp(address->valuestring, windowAddress) == 0)) continue;

        cJSON *at = cJSON_GetObjectItemCaseSensitive(client, "at");
        if (cJSON_IsArray(at)) {
                pos[0] = cJSON_GetArrayItem(at, 0)->valueint;
                pos[1] = cJSON_GetArrayItem(at, 1)->valueint;
        }
        break;
    }

    cJSON_Delete(root);
    return pos;
}
