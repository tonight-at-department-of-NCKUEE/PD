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

void mark_section(DataBase &db, Cell *cell, vector<vector<double>> &cut_sections);

void select_subrows(DataBase &db, Cell *cell, vector<Subrow *> &subrows, double window_height, double window_width);

bool can_insert(Cell *cell, Subrow *subrow);

void store_row(Subrow *subrow, vector<cluster_data> &RowData);

void insert_cell(Cell *cell, Subrow *subrow);

void AddCell(Cluster &cluster, Cell *cell);

void Collapse(Cluster &cluster, Subrow *subrow, int clust_idx);

void AddCluster(Cluster &pre, Cluster &current);

double cost_func(Cell *cell);

double net_hpwl(Net *net);

void RemoveCell(Subrow *subrow, vector<cluster_data> &RowData, bool is_collapsed);

bool overlap(Cell *cell, Cluster cluster);

#endif
