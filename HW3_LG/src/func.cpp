#include "../include/func.h"
#include "func.h"

void mark_section(DataBase &db, Cell *cell, vector<vector<double>> &cut_sections)
{
    if (cell->notRect == true)
        return;

    double start = (cell->y - db.chip_y1) / db.rowheight;
    double end = (cell->Top() - db.chip_y1) / db.rowheight;

    for (int i = floor(start); i < ceil(end); i++)
    {
        cut_sections[i].push_back(cell->x);
        cut_sections[i].push_back(cell->Right());
    }
}

void select_subrows(DataBase &db, Cell *cell, vector<Subrow *> &subrows, double window_height, double window_width)
{
    double start = (cell->y - db.chip_y1) / db.rowheight - window_height;
    double end = (cell->Top() - db.chip_y1) / db.rowheight + window_height;

    double window_x1 = cell->x - db.chip_width / window_width;
    double window_x2 = cell->Right() + db.chip_width / window_width;

    if (window_x1 < db.chip_x1)
        window_x1 = db.chip_x1;
    if (window_x2 < db.chip_x2)
        window_x2 = db.chip_x2;

    for (int i = floor(start); i < ceil(end); i++)
    {
        if (i < 0)
            continue;
        if (i >= db.rows.size())
            break;

        for (int j = 0; j < db.rows[i].subrows.size(); j++)
        {
            if (db.rows[i].subrows[j].Right() <= window_x1)
                continue;
            if (db.rows[i].subrows[j].x >= window_x2)
                continue;
            subrows.push_back(&db.rows[i].subrows[j]);
        }
    }
}

bool can_insert(Cell *cell, Subrow *subrow)
{
    if (subrow->cells_size + cell->width > subrow->width)
        return false;
    else
        return true;
}

void store_row(Subrow *subrow, vector<cluster_data> &RowData)
{
    for (int i = 0; i < subrow->clusters.size(); i++)
    {
        cluster_data cd;
        cd.ec = subrow->clusters[i].ec;
        cd.wc = subrow->clusters[i].wc;
        cd.xc = subrow->clusters[i].xc;
        cd.qc = subrow->clusters[i].qc;
        cd.first_cell = subrow->clusters[i].cells[0]->name;
        RowData.push_back(cd);
    }
}

void insert_cell(Cell *cell, Subrow *subrow)
{
    subrow->cells_size += cell->width;
    cell->y = subrow->y;

    if (cell->x < subrow->x)
        cell->x = subrow->x;

    if (cell->x + cell->width > subrow->Right())
        cell->x = subrow->Right() - cell->width;
}

void AddCell(Cluster &cluster, Cell *cell)
{
    cluster.ec += 1;
    cluster.qc += cell->x - cluster.wc;
    cluster.wc += cell->width;
    cluster.cells.push_back(cell);
}

void Collapse(Cluster &cluster, Subrow *subrow, int clust_idx)
{
    cluster.xc = cluster.qc / cluster.ec;

    if (cluster.xc < subrow->x)
        cluster.xc = subrow->x;

    if (cluster.xc > subrow->Right() - cluster.wc)
        cluster.xc = subrow->Right() - cluster.wc;

    int pre_idx = clust_idx - 1;
    if (pre_idx < 0)
        return;

    if (subrow->clusters[pre_idx].xc + subrow->clusters[pre_idx].wc > cluster.xc)
    {
        AddCluster(subrow->clusters[pre_idx], cluster);
        subrow->clusters.erase(subrow->clusters.begin() + clust_idx);
        Collapse(subrow->clusters[pre_idx], subrow, pre_idx);
    }
}

void AddCluster(Cluster &pre, Cluster &current)
{
    pre.ec += current.ec;
    pre.qc += current.qc - pre.wc;
    pre.wc += current.wc;

    for (int i = 0; i < current.cells.size(); i++)
    {
        pre.cells.push_back(current.cells[i]);
    }
}

double cost_func(Cell *cell)
{
    double sum = 0;
    for (int i = 0; i < cell->nets.size(); i++)
    {
        sum += net_hpwl(cell->nets[i]);
    }

    return sum;
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

void RemoveCell(Subrow *subrow, vector<cluster_data> &RowData, bool is_collapsed)
{
    Cell *cell = subrow->clusters.back().cells.back();
    subrow->cells_size -= cell->width;

    if (is_collapsed == false)
    {
        subrow->clusters.pop_back();
        return;
    }

    vector<Cell *> cells;
    for (int i = 0; i < subrow->clusters.size(); i++)
    {
        for (int j = 0; j < subrow->clusters[i].cells.size(); j++)
        {
            cells.push_back(subrow->clusters[i].cells[j]);
        }
    }

    // cout << cells.back()->name << endl;
    cells.pop_back();

    subrow->clusters.clear();
    int cell_idx = 0;
    for (int i = 0; i < RowData.size(); i++)
    {
        Cluster cluster;
        cluster.ec = RowData[i].ec;
        cluster.xc = RowData[i].xc;
        cluster.qc = RowData[i].qc;
        cluster.wc = RowData[i].wc;
        int x = cluster.xc;
        while (1)
        {
            if (cell_idx == cells.size())
                break;
            if (i != RowData.size() - 1 && cells[cell_idx]->name == RowData[i + 1].first_cell)
                break;
            cells[cell_idx]->x = x;
            x += cells[cell_idx]->width;
            cluster.cells.push_back(cells[cell_idx]);
            cell_idx++;
        }
        subrow->clusters.push_back(cluster);
    }
}

bool overlap(Cell *cell, Cluster cluster)
{
    if (cell->x >= cluster.xc + cluster.wc)
        return false;

    if (cell->Right() <= cluster.xc)
        return false;

    return true;
}
