#ifndef STRUCT_H
#define STRUCT_H

#include <iostream>
#include <string>
#include <vector>
#include <list>

using namespace std;

extern double desired_aspect_ratio;
extern double whitespace;
extern double A;
extern double chip_width;
extern double chip_height;
extern double Alpha;
extern double Beta;
extern double max_hpwl;

struct Net;
class Module;

struct m_data
{
    string name = "null";
    double width = 0, height = 0;
};

struct net_data
{
    string name;
    vector<string> modules;
};

struct Net
{
    string name;
    vector<Module *> connectedModules;
};

class Module
{
public:
    Module();
    ~Module();

    string name;
    double x, y;
    double width, height;
    vector<Net *> connectedNets;
    Module *left = nullptr;
    Module *right = nullptr;
    Module *parent = nullptr;

    void set_coordinate(int x1, int y1);
    void put_right(Module *M);
    void put_top(Module *M);
};

bool comparator(const Module *lhs, const Module *rhs);

struct Contour
{
    double x1, x2;
    double y;
    Contour *prev = nullptr;
    Contour *next = nullptr;
};

class BStarTree
{
private:
    /* data */
public:
    BStarTree();
    ~BStarTree();

    Module *root = nullptr;
    Contour *contour = nullptr;

    void packing(Module *node);
    void reset_contour();
};

#endif // !STRUCT_H
