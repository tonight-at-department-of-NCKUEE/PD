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

double desired_aspect_ratio = 0;
double whitespace = 0;
double A = 0;
double chip_width = 0;
double chip_height = 0;
double Alpha = 0.000000005;
double Beta = 0;
double max_hpwl = 0;
//         0           1    2 3  4              5               6
// ./SA_E24096530 ami33.yal 1 10 ami33_1_10.txt ami33_1_10.out ami33_1_10.m
int main(int argc, char *argv[])
{
    int start_time = time(NULL);

    if (!strcmp(argv[1], "ami33.yal"))
    {
        // Alpha = 0.000001;
        Alpha = 0.000000005;
        if (!strcmp(argv[2], "1") && !strcmp(argv[3], "15"))
            max_hpwl = 52000;
        if (!strcmp(argv[2], "1") && !strcmp(argv[3], "10"))
            max_hpwl = 55000;
        if (!strcmp(argv[2], "2") && !strcmp(argv[3], "15"))
            max_hpwl = 57000;
        if (!strcmp(argv[2], "2") && !strcmp(argv[3], "10"))
            max_hpwl = 57000;
        if (!strcmp(argv[2], "3") && !strcmp(argv[3], "10"))
            max_hpwl = 60000;
        if (!strcmp(argv[2], "3") && !strcmp(argv[3], "15"))
            max_hpwl = 57000;
    }
    else if (!strcmp(argv[1], "ami49.yal"))
    {
        if (!strcmp(argv[2], "1") && !strcmp(argv[3], "15"))
            max_hpwl = 1300000;
        if (!strcmp(argv[2], "1") && !strcmp(argv[3], "10"))
            max_hpwl = 1400000;
        if (!strcmp(argv[2], "2") && !strcmp(argv[3], "15"))
            max_hpwl = 1300000;
        if (!strcmp(argv[2], "2") && !strcmp(argv[3], "10"))
            max_hpwl = 1350000;
        if (!strcmp(argv[2], "3") && !strcmp(argv[3], "10"))
            max_hpwl = 1350000;
        if (!strcmp(argv[2], "3") && !strcmp(argv[3], "15"))
            max_hpwl = 1300000;
        Alpha = 0.000000005;
    }
    else if (!strcmp(argv[1], "apte.yal"))
    {
        if (!strcmp(argv[2], "1") && !strcmp(argv[3], "15"))
            max_hpwl = 210000;
        if (!strcmp(argv[2], "1") && !strcmp(argv[3], "10"))
            max_hpwl = 230000;
        if (!strcmp(argv[2], "2") && !strcmp(argv[3], "15"))
            max_hpwl = 210000;
        if (!strcmp(argv[2], "2") && !strcmp(argv[3], "10"))
            max_hpwl = 210000;
        if (!strcmp(argv[2], "3") && !strcmp(argv[3], "15"))
            max_hpwl = 250000;
        Alpha = 0.000000005;
    }

    cout << "Program will stop after obtaining hpwl < " << max_hpwl << endl;
    int random_seed = time(NULL);
    srand(time(NULL));
    // srand(1700363563);

    vector<Module *> ModuleArray;
    map<string, Net *> NetArray;
    BStarTree tree;
    vector<m_data> data;
    vector<net_data> nets;

    // parse
    parser(argv[1], ModuleArray, NetArray);

    // init
    init_constant(argv, ModuleArray);
    init_tree(tree, ModuleArray);
    tree.packing(tree.root);
    save_init_data(data, ModuleArray, NetArray, nets);

    // SA
    simulated_annealing(tree, ModuleArray, nets);

    // output file
    int end_time = time(NULL);
    int hpwl = (int)HPWL(ModuleArray, nets);
    plot_mfile(argv[6], ModuleArray);
    write_outfile(argv[5], ModuleArray, data);
    write_txtfile(argv, ModuleArray, end_time - start_time, hpwl);

    cout << "seed: " << random_seed << endl;
    cout << "HPWL = " << hpwl << endl;
    return 0;
}
