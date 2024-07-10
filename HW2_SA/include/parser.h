#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <set>

#include "struct.h"

using namespace std;

void parser(string yal_name, vector<Module *> &ModuleArray, map<string, Net *> &NetArray);

void parse_modules(ifstream &yal_file, vector<Module *> &ModuleArray, map<string, Net *> &NetArray);

void parse_netlist(ifstream &yal_file, vector<Module *> &ModuleArray, map<string, Net *> &NetArray);

#endif