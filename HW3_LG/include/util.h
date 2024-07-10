#ifndef UTIL_H
#define UTIL_H

#include <fstream>
#include <map>
#include <ctime>
#include "struct.h"
#include "func.h"
#include <cmath>
#include <algorithm>
#include <queue>
#include <climits>

void init_subrows(DataBase &db);

void Abacus(DataBase &db);

bool PlaceRow(Subrow *subrow, Cell *cell);

double total_hpwl(vector<Net *> &NetArray);

void write_pl(DataBase &db, char *filename);

#endif // !UTIL_H
