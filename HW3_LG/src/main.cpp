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

//         0         1         2                     3            4
// ./LG_E24096530 –benchmark /benchmark/superblue1 –output /out/superblue1.pl
int main(int argc, char *argv[])
{
    int start_time = time(NULL);
    DataBase db;

    parser(argv, db);
    init_subrows(db);

    sort(db.movables.begin(), db.movables.end(), cell_comparator);
    Abacus(db);

    int end_time = time(NULL);

    write_pl(db, argv[4]);

    cout << "hpwl: " << int(total_hpwl(db.NetArray)) << endl;
    cout << "run time: " << end_time - start_time << "s" << endl;

    return 0;
}
