#ifndef STRUCT_H
#define STRUCT_H

#include <iostream>
#include <string>
#include <vector>
#include <list>

using namespace std;

extern double maxCellSize;
extern double W;
extern int Pmax;
extern string outfile_name;

struct Cell;
struct Net;

struct Net
{
    string name;
    vector<Cell *> connectedCells;
    int cells_in_A;
    int cells_in_B;
};

struct Cell
{
    char group;
    string name;
    double size;
    int gain;
    vector<Net *> connectedNets;
    bool lock;
    Cell *next;
    Cell *prev;
};

class bucketlist
{
private:
    vector<Cell *> bucket;
    int offset;

public:
    double size;
    double upbound;
    double lowbound;
    int max_gain;

    bucketlist(int lower, int upper)
    {
        offset = -lower;
        for (int i = 0; i < upper - lower + 1; i++)
        {
            bucket.push_back(nullptr);
        }
    };
    ~bucketlist(){

    };

    Cell *&operator[](int index)
    {
        return bucket[index + offset];
    };
};

#endif // !STRUCT_H
