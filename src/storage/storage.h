#ifndef STORAGE_H
#define STORAGE_H

#include "../models.h"

int save_data(const char* filename);
void load_data(const char* filename);
int generate_next_id();

#endif // STORAGE_H
