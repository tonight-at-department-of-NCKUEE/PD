#ifndef STRUCT_H
#define STRUCT_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

using namespace std;

class Cell;
class Net;

struct Pin
{
    double Xoffset = 0;
    double Yoffset = 0;
    Cell *cell;
};

class Net
{
private:
public:
    Net();
    Net(string name);
    ~Net();

    string name;
    vector<Pin> pins;
};

class Cell
{
private:
public:
    Cell();
    Cell(string name, double x, double y, double w, double h);
    ~Cell();

    string name;
    double x;
    double y;
    double width;
    double height;
    bool notRect = false;

    vector<Net *> nets;

    double Top();
    double Right();
    double mid_x();
    double mid_y();
};

bool cell_comparator(const Cell *lhs, const Cell *rhs);

class Subrow;

class Row
{
private:
public:
    Row();
    ~Row();
    vector<Subrow> subrows;
    friend class Subrow;
};

class Cluster
{
private:
public:
    Cluster();
    ~Cluster();

    double ec = 0;
    double qc = 0;
    double wc = 0;
    int xc;

    vector<Cell *> cells;
};

class Subrow
{
private:
public:
    Subrow();
    ~Subrow();

    double x;
    double y;
    double width;
    double height;
    double cells_size = 0;
    vector<Cluster> clusters;

    double Top();
    double Right();

    friend class Row;
};

bool row_comparator(const Row &lhs, const Row &rhs);

struct cluster_data
{
    double qc;
    double ec;
    double wc;
    int xc;
    string first_cell;
};

class DataBase
{
private:
public:
    DataBase();
    ~DataBase();

    double chip_x1;
    double chip_y1;
    double chip_x2;
    double chip_y2;
    double rowheight;
    double chip_width;
    double chip_height;

    vector<Cell *> movables;
    vector<Cell *> terminals;
    vector<Cell *> terminal_NIs;
    vector<Cell *> nonRects;

    vector<Net *> NetArray;
    vector<Row> rows;

    map<string, Cell *> CellArray;
};

#endif // !STRUCT_H
