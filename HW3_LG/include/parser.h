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

#include "struct.h"

using namespace std;

void parser(char *command[], DataBase &db);

void parse_shapes(string filename, DataBase &db, set<string> &nonRect);

void parse_nodes(string filename, DataBase &db, set<string> &nonRect);

void parse_legal(string filename, DataBase &db);

void parse_nets(string filename, DataBase &db);

void parse_scl(string filename, DataBase &db);

#endif