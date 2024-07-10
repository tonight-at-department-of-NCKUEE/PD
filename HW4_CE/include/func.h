#ifndef FUNC_H
#define FUNC_H

#include <fstream>
#include <map>
#include <ctime>
#include "struct.h"
#include <cmath>
#include <algorithm>
#include <queue>
#include <climits>

extern "C"
{
#include "../flute-3.1/flute.h"
}

void connect_two_pin(Branch &p1, Branch &p2, DataBase &db);

void update_demand_down(Edge &edge, vector<Edge *> &DownEdges, DataBase &db, double d);

void update_demand_left(Edge &edge, vector<Edge *> &LeftEdges, DataBase &db, double d);

double net_hpwl(Net *net);

#endif
