#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <set>
#include <sstream>
#include <climits>
#include <algorithm>

#include "struct.h"

using namespace std;

void parser(char *command[], DataBase &db);

void parse_shapes(string filename, DataBase &db, set<string> &nonRect);

void parse_nodes(string filename, DataBase &db, set<string> &nonRect);

void parse_pl(string filename, DataBase &db);

void parse_nets(string filename, DataBase &db);

void parse_scl(string filename, DataBase &db);

void parse_route(string filename, DataBase &db);

void init_edge_supply(Edge &edge, DataBase &db, vector<double> &MinWireWidth, vector<double> &MinWireSpacing);

double overlapLength(Edge &edge, Blockage &blockage);

bool segmentsOverlap(double a_start, double a_end, double b_start, double b_end);

#endif