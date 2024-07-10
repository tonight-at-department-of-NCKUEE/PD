#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>
#include <ctime>

#include "../include/parser.h"
#include "../include/struct.h"
#include "../include/util.h"

using namespace std;

double maxCellSize = 0;
double W = 0;
int Pmax = 0;
string outfile_name = "0";

int main(int argc, char *argv[])
{
    outfile_name = argv[3];
    double start_time = time(NULL);
    // to execute, enter ./FM_XXXXXXXXX adaptec1.nodes adaptec1.nets adaptec1.out
    map<string, Cell *> cellArray;
    vector<Net *> netArray;
    parser(argv[1], argv[2], cellArray, netArray);

    bucketlist groupA(-Pmax, Pmax), groupB(-Pmax, Pmax);

    initial(groupA, groupB, cellArray);

    vector<string> cut_set;
    find_cutset(cut_set, netArray);
    cout << "Pmax = " << Pmax << endl;
    cout << "initial cutsize:" << cut_set.size() << endl;
    cout << "net num = " << netArray.size() << endl;
    cout << "cell num = " << cellArray.size() << endl;
    pair<vector<Cell *>, int> move_record;

    int iteration = 0;
    while (true)
    {
        move_record.second = 0;
        move_record.first.clear();
        iteration++;
        cout << "iteration #" << iteration << endl;
        if (FM(groupA, groupB, cellArray, move_record) == false)
        {
            break;
        }

        load_best_result(move_record, cellArray, groupA, groupB);
    }

    // load final result
    load_best_result(move_record, cellArray, groupA, groupB);

    generate_output(start_time, netArray, cellArray);

    double sumAsize = 0, sumBsize = 0;
    for (auto const &i : cellArray)
    {
        if (i.second->group == 'A')
        {
            sumAsize += i.second->size;
        }
        else if ((i.second->group == 'B'))
        {
            sumBsize += i.second->size;
        }
    }

    int pinNum = 0;
    for (int i = 0; i < netArray.size(); i++)
    {
        pinNum += netArray[i]->connectedCells.size();
    }
    cout << "pin num:" << pinNum << endl;

    cout << "A:" << groupA.lowbound << "~" << groupA.upbound << endl;
    cout << "A size:" << sumAsize << endl;
    cout << "A num:" << groupA.size << endl;
    cout << endl;
    cout << "B:" << groupB.lowbound << "~" << groupB.upbound << endl;
    cout << "B size:" << sumBsize << endl;
    cout << "B num:" << groupB.size << endl;
    cout << endl;

    cout << "total size:" << int(sumAsize + sumBsize) << " " << int(W) << endl;

    return 0;
}
