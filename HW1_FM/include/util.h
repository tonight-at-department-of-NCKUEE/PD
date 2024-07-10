#ifndef UTIL_H
#define UTIL_H

#include <fstream>
#include <map>
#include <ctime>
#include "struct.h"

void initial(bucketlist &A, bucketlist &B, map<string, Cell *> &cellArray);

void compute_init_gain(map<string, Cell *> &cellArray);

int calc_FT(Cell *cell, Net *net);

void cells_insert_bucket(map<string, Cell *> &cellArray, bucketlist &A, bucketlist &B);

void insert_cell(Cell *cell, bucketlist &bucket);

bool FM(bucketlist &A, bucketlist &B, map<string, Cell *> &cellArray, pair<vector<Cell *>, int> &move_record);

Cell *find_baseCell(bucketlist &A, bucketlist &B);

void update_gain(Cell *baseCell, bucketlist &A, bucketlist &B);

void do_case(Cell *baseCell, Net *net, bucketlist &A, bucketlist &B);

void update_cell_index(Cell *cell, bucketlist &A, bucketlist &B, int new_index);

void bucket_remove_cell(Cell *cell, bucketlist &bucket);

void move_baseCell(Cell *baseCell, bucketlist &A, bucketlist &B);

void update_maxGain(bucketlist &A, bucketlist &B);

void load_best_result(pair<vector<Cell *>, int> &move_record, map<string, Cell *> &cellArray, bucketlist &A, bucketlist &B);

void generate_output(double start_time, vector<Net *> &netArray, map<string, Cell *> &cellArray);

void find_group(vector<string> &groupA, vector<string> &groupB, map<string, Cell *> &cellArray);

void find_cutset(vector<string> &cut_set, vector<Net *> &netArray);

bool net_is_cut(Net *net);

#endif // !UTIL_H
