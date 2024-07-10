#include "../include/util.h"
#include "util.h"

void init_subrows(DataBase &db)
{
    sort(db.rows.begin(), db.rows.end(), row_comparator);
    vector<vector<double>> cut_sections;
    for (int i = 0; i < db.rows.size(); i++)
    {
        vector<double> v;
        cut_sections.push_back(v);
    }

    for (int i = 0; i < db.terminals.size(); i++)
    {
        mark_section(db, db.terminals[i], cut_sections);
    }

    for (int i = 0; i < db.nonRects.size(); i++)
    {
        mark_section(db, db.nonRects[i], cut_sections);
    }

    for (int i = 0; i < cut_sections.size(); i++)
    {
        sort(cut_sections[i].begin(), cut_sections[i].end());
    }

    for (int i = 0; i < db.rows.size(); i++)
    {
        Subrow subrow = db.rows[i].subrows[0];
        double x1 = subrow.x, x2, x_end = subrow.Right();

        db.rows[i].subrows.clear();

        for (int j = 0; j < int(cut_sections[i].size()) - 1; j += 2)
        {
            x2 = cut_sections[i][j];
            if (x1 == x2)
            {
                x1 = cut_sections[i][j + 1];
                continue;
            }

            subrow.x = x1;
            subrow.width = x2 - x1;
            db.rows[i].subrows.push_back(subrow);

            x1 = cut_sections[i][j + 1];
        }

        if (x1 != x_end)
        {
            subrow.x = x1;
            subrow.width = x_end - x1;
            db.rows[i].subrows.push_back(subrow);
        }
    }
}

void Abacus(DataBase &db)
{
    cout << "Running Abacus..." << endl;

    double window_height, window_width;
    bool cannot_insert = false;
    for (int i = 0; i < db.movables.size(); i++)
    {
        if (cannot_insert == false)
        {
            window_height = 3;
            window_width = 200;
        }
        else
        {
            window_height *= 2;
            window_width /= 2;
        }
        // cout << i << " " << db.movables[i]->name << endl;
        double cost, best_cost = INT_MAX;
        Cell *cell = db.movables[i];
        Subrow *best_row = nullptr;
        vector<Subrow *> subrows;

        select_subrows(db, cell, subrows, window_height, window_width);

        for (int j = 0; j < subrows.size(); j++)
        {
            if (can_insert(cell, subrows[j]) == false)
                continue;

            int cell_x = cell->x;
            vector<cluster_data> RowData;
            store_row(subrows[j], RowData);
            insert_cell(cell, subrows[j]);
            bool is_collapsed = PlaceRow(subrows[j], cell);

            cost = cost_func(cell);
            // cout << cell->name << " " << cell->x << endl;

            if (cost < best_cost)
            {
                best_cost = cost;
                best_row = subrows[j];
            }

            RemoveCell(subrows[j], RowData, is_collapsed);
            cell->x = cell_x;
        }

        if (best_row == nullptr)
        {
            cannot_insert = true;
            i--;
            continue;
        }
        cannot_insert = false;
        insert_cell(cell, best_row);
        PlaceRow(best_row, cell);
    }
}

bool PlaceRow(Subrow *subrow, Cell *cell)
{
    int last = int(subrow->clusters.size()) - 1;

    if (last == -1 || subrow->clusters[last].xc + subrow->clusters[last].wc <= cell->x)
    {
        Cluster new_cluster;
        new_cluster.xc = cell->x;
        AddCell(new_cluster, cell);
        subrow->clusters.push_back(new_cluster);
        return false;
    }

    AddCell(subrow->clusters[last], cell);
    Collapse(subrow->clusters[last], subrow, last);

    for (int i = 0; i < subrow->clusters.size(); i++)
    {
        int x = int(subrow->clusters[i].xc);
        for (int j = 0; j < subrow->clusters[i].cells.size(); j++)
        {
            subrow->clusters[i].cells[j]->x = x;
            x += subrow->clusters[i].cells[j]->width;
        }
    }

    return true;
}

double total_hpwl(vector<Net *> &NetArray)
{
    double sum = 0;

    for (int i = 0; i < NetArray.size(); i++)
    {
        sum += net_hpwl(NetArray[i]);
    }

    return sum;
}

void write_pl(DataBase &db, char *filename)
{
    const char *path = filename;
    ofstream pl;
    pl.open(path);

    if (!pl.is_open())
    {
        cout << filename << " not open" << endl;
        abort();
    }

    cout << "Writing file: " << filename << endl;

    pl << "UCLA pl 1.0" << endl;
    pl << "#Created : Sep 17 2012" << endl;
    pl << "#User : Natarajan Viswanathan, IBM Corporation, Austin(nviswan @us.ibm.com) " << endl;

    for (int i = 0; i < db.movables.size(); i++)
    {
        pl << db.movables[i]->name << "\t" << db.movables[i]->x << "\t" << db.movables[i]->y << "\t"
           << ":  N\n";
    }

    for (int i = 0; i < db.terminals.size(); i++)
    {
        pl << db.terminals[i]->name << "\t" << db.terminals[i]->x << "\t" << db.terminals[i]->y << "\t"
           << ":  N  /FIXED\n";
    }

    for (int i = 0; i < db.terminal_NIs.size(); i++)
    {
        pl << db.terminal_NIs[i]->name << "\t" << db.terminal_NIs[i]->x << "\t" << db.terminal_NIs[i]->y << "\t"
           << ":  N  /FIXED_NI\n";
    }

    pl.close();
}
