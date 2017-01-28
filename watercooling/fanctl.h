#pragma once

extern void setup_fanctl(); /* fanctl.c */
extern void fanctl_setspeed(uint8_t speed);
extern void fanctl_cmd(char *cmd, int len);

