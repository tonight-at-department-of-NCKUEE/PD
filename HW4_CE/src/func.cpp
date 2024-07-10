#include "../include/func.h"
#include "func.h"

void connect_two_pin(Branch &p1, Branch &p2, DataBase &db)
{
    bool edge_is_horizontal = (p1.y == p2.y);
    bool edge_is_vertical = (p1.x == p2.x);
    double demand = 2;

    if (p1.x == p2.x && p1.y == p2.y)
        return;

    if (edge_is_vertical == true)
    {
        Edge edge(p1.x, p1.y, p2.x, p2.y);
        update_demand_down(edge, db.DownEdges, db, demand);
        return;
    }

    if (edge_is_horizontal == true)
    {
        Edge edge(p1.x, p1.y, p2.x, p2.y);
        update_demand_left(edge, db.LeftEdges, db, demand);
        return;
    }

    demand /= 2;
    Edge edge1(p1.x, p1.y, p1.x, p2.y);
    update_demand_down(edge1, db.DownEdges, db, demand);
    Edge edge2(p1.x, p2.y, p2.x, p2.y);
    update_demand_left(edge2, db.LeftEdges, db, demand);
    Edge edge3(p1.x, p1.y, p2.x, p1.y);
    update_demand_left(edge3, db.LeftEdges, db, demand);
    Edge edge4(p2.x, p1.y, p2.x, p2.y);
    update_demand_down(edge4, db.DownEdges, db, demand);
}

void update_demand_down(Edge &edge, vector<Edge *> &DownEdges, DataBase &db, double d)
{
    int start = floor(edge.x1 / db.grid_width) * db.VerticalNum + floor(edge.y1 / db.grid_height);
    int end = floor(edge.x1 / db.grid_width) * db.VerticalNum + floor(edge.y2 / db.grid_height);

    for (int i = start + 1; i <= end; i++)
    {
        DownEdges[i]->demand += d;
    }
}

void update_demand_left(Edge &edge, vector<Edge *> &LeftEdges, DataBase &db, double d)
{
    int start = floor(edge.y1 / db.grid_height) * db.HorizontalNum + floor(edge.x1 / db.grid_width);
    int end = floor(edge.y1 / db.grid_height) * db.HorizontalNum + floor(edge.x2 / db.grid_width);

    for (int i = start + 1; i <= end; i++)
    {
        LeftEdges[i]->demand += d;
    }
}

double net_hpwl(Net *net)
{
    double xmax = INT_MIN, xmin = INT_MAX, ymax = INT_MIN, ymin = INT_MAX;
    for (int i = 0; i < net->pins.size(); i++)
    {
        Pin p = net->pins[i];
        double px = p.cell->mid_x() + p.Xoffset;
        double py = p.cell->mid_y() + p.Yoffset;
        if (xmax < px)
            xmax = px;
        if (xmin > px)
            xmin = px;
        if (ymax < py)
            ymax = py;
        if (ymin > py)
            ymin = py;
    }

    return xmax - xmin + ymax - ymin;
}
