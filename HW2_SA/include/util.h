#ifndef UTIL_H
#define UTIL_H

#include <fstream>
#include <map>
#include <ctime>
#include "struct.h"
#include <cmath>
#include <algorithm>
#include <queue>
#include <climits>

void init_constant(char *argv[], vector<Module *> &ModuleArray);

void init_tree(BStarTree &tree, vector<Module *> &ModuleArray);

void update_contour(BStarTree &tree, Module *M);

void simulated_annealing(BStarTree &tree, vector<Module *> &ModuleArray, vector<net_data> &nets);

void load_tree(vector<Module *> &s, BStarTree &tree, vector<m_data> &data);

void store_tree(vector<Module *> &s, BStarTree &tree, vector<m_data> &data);

void rotate(vector<Module *> &ModuleArray);

void exchange(vector<Module *> &ModuleArray, BStarTree &tree);

void delete_insert(vector<Module *> &ModuleArray, BStarTree &tree);

double cost(BStarTree &tree, vector<Module *> &ModuleArray);

double current_aspect_ratio(Contour *c);

double HPWL(vector<Module *> &ModuleArray, vector<net_data> &nets);

double calc_H(Contour *c);

double calc_W(Contour *c);

void plot_mfile(string filename, vector<Module *> &ModuleArray);

void write_outfile(string filename, vector<Module *> &ModuleArray, vector<m_data> &data);

void write_txtfile(char *argv[], vector<Module *> &ModuleArray, int run_time, int hpwl);

void save_init_data(vector<m_data> &data, vector<Module *> &ModuleArray, map<string, Net *> NetArray, vector<net_data> &nets);

#endif // !UTIL_H
