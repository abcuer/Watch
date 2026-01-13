#ifndef __DRAW_H
#define __DRAW_H

#include "cst816.h"
#include "stdint.h"

void CST816_Test();
void SetLineThickness(uint8_t thickness);
void Touch_DrawLine(void);

extern CST816_Info CST816_Instance;

#endif // __DRAW_H