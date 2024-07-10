#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>
#include <ctime>
#include <random>
#include <cmath>
#include <cstring>

#include "../include/parser.h"
#include "../include/struct.h"
#include "../include/util.h"

using namespace std;

void BuildTrees(DataBase &db, vector<Tree> &trees);

bool comparator_h(const Grid &lhs, const Grid &rhs)
{
    return lhs.h_overflow > rhs.h_overflow;
};

bool comparator_v(const Grid &lhs, const Grid &rhs)
{
    return lhs.v_overflow > rhs.v_overflow;
};
//         0       1      2                                 3        4
// ./CE_E24096530 –bench benchmarks/adaptec1/adaptec1.aux –output CE_E24096530_adaptec1.txt
int main(int argc, char *argv[])
{
    int start_time = time(NULL);

    DataBase db;
    parser(argv, db);

    vector<Tree> trees;
    BuildTrees(db, trees);

    build_congestion_map(trees, db);

    sort(db.Grids.begin(), db.Grids.end(), &comparator_v);
    plot_matlab(argv, db, "_v.m");

    sort(db.Grids.begin(), db.Grids.end(), &comparator_h);
    plot_matlab(argv, db, "_h.m");

    write_output(string(argv[4]), db, start_time);

    cout << "run time: " << time(NULL) - start_time << "s" << endl;
    return 0;
}

void BuildTrees(DataBase &db, vector<Tree> &trees)
{
    readLUT();
    for (int i = 0; i < db.NetArray.size(); i++)
    {
        Net *net = db.NetArray[i];
        int pins_size = net->pins.size();

        if (pins_size <= 1)
            continue;

        int degree = 0;
        int *x = new int[pins_size];
        int *y = new int[pins_size];
        for (int j = 0; j < pins_size; j++)
        {
            x[degree] = net->pins[j].cell->mid_x() + net->pins[j].Xoffset;
            y[degree] = net->pins[j].cell->mid_y() + net->pins[j].Yoffset;
            degree++;
        }
        // readLUT();
        trees.push_back(flute(degree, x, y, ACCURACY));
        delete[] x;
        delete[] y;
    }
}