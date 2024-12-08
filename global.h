#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>

extern bool BtnState;
void* StartSiren(void* args);
void EndSiren(int gpio1, int gpio2);

#endif