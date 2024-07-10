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

void build_congestion_map(vector<Tree> trees, DataBase &db);

void plot_matlab(char *argv[], DataBase &db, string HV);

void write_output(string filename, DataBase &db, int runtime);

double total_hpwl(vector<Net *> &NetArray);

void write_pl(DataBase &db, char *filename);

#endif // !UTIL_H
