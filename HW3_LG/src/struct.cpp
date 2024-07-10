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
