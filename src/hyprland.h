#ifndef HYPRLAND_H
#define HYPRLAND_H

#include "cJSON.h"

int ConnectHyprlandSocket(void);
void SendCommand(int sock, const char *cmd);
char *GetReply(int sock);
char *GetAddressByTitle(char *JSONBuffer, char *windowTitle);
int *GetPosByAddress(char *JSONBuffer, char *windowAddress);

#endif
