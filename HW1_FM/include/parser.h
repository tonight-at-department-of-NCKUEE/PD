#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <set>

#include "struct.h"

using namespace std;

void parser(string node_file, string net_file,
            map<string, Cell *> &cellArray, vector<Net *> &netArray);

void readNodeFile(string node_file, map<string, Cell *> &cellArray, set<string> &fix_nodes);

void readNetFile(string net_file, map<string, Cell *> &cellArray, vector<Net *> &netArray, set<string> &fix_nodes);

#endif