#include "../include/struct.h"

Module::Module()
{
}
Module::~Module()
{
}

void Module::set_coordinate(int x1, int y1)
{
    this->x = x1;
    this->y = y1;
}

void Module::put_right(Module *M)
{
    this->left = M;
    M->parent = this;
}

void Module::put_top(Module *M)
{
    this->right = M;
    M->parent = this;
}

bool comparator(const Module *lhs, const Module *rhs)
{
    return lhs->height > rhs->height;
}

BStarTree::BStarTree()
{
}

BStarTree::~BStarTree()
{
    /*Contour *tmp = contour;
    Contour *tmp1;
    while (tmp != nullptr)
    {
        tmp1 = tmp->next;
        delete tmp;
        tmp = tmp1;
    }*/
}
