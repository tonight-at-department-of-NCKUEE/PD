#include "../include/struct.h"
#include "struct.h"

Cell::Cell()
{
}

Cell::Cell(string name, double x, double y, double w, double h)
{
    this->name = name;
    this->x = x;
    this->y = y;
    this->width = w;
    this->height = h;
}

Cell::~Cell()
{
}

double Cell::Top()
{
    return y + height;
}

double Cell::Right()
{
    return x + width;
}

double Cell::mid_x()
{
    return x + width / 2;
}

double Cell::mid_y()
{
    return y + height / 2;
}

Net::Net()
{
}

Net::Net(string name)
{
    this->name = name;
}

Net::~Net()
{
}

DataBase::DataBase()
{
}

DataBase::~DataBase()
{
}

Row::Row()
{
}

Row::~Row()
{
}

Subrow::Subrow()
{
}

Subrow::~Subrow()
{
}

double Subrow::Top()
{
    return y + height;
}

double Subrow::Right()
{
    return x + width;
}

bool row_comparator(const Row &lhs, const Row &rhs)
{
    return lhs.subrows[0].y < rhs.subrows[0].y;
}

bool cell_comparator(const Cell *lhs, const Cell *rhs)
{
    return lhs->x < rhs->x;
}

Cluster::Cluster()
{
}

Cluster::~Cluster()
{
}

Grid::Grid()
{
}

Grid::~Grid()
{
}

Grid::Grid(double x, double y, double width, double height)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    LeftEdge.x1 = x;
    LeftEdge.x2 = x;
    LeftEdge.y1 = y;
    LeftEdge.y2 = y + height;
    DownEdge.x1 = x;
    DownEdge.x2 = x + width;
    DownEdge.y1 = y;
    DownEdge.y2 = y;
}

Edge::Edge()
{
}

Edge::~Edge()
{
}

Edge::Edge(double x1, double y1, double x2, double y2)
{
    if (x1 < x2 || y1 < y2)
    {
        this->x1 = x1;
        this->x2 = x2;
        this->y1 = y1;
        this->y2 = y2;
        return;
    }
    else if (x1 > x2 || y1 > y2)
    {
        this->x1 = x2;
        this->x2 = x1;
        this->y1 = y2;
        this->y2 = y1;
        return;
    }
    cout << "This edge is a point, wtf?" << endl;
}

double Edge::Length()
{
    double delta_x = x1 - x2;
    double delta_y = y1 - y2;
    return sqrt(delta_x * delta_x + delta_y * delta_y);
}

double Edge::calc_overflow()
{
    if (supply > demand)
        return 0;
    else
        return demand - supply;
}
