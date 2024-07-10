#include "../include/util.h"
#include "util.h"

void initial(bucketlist &A, bucketlist &B, map<string, Cell *> &cellArray)
{
    A.upbound = 0.4 * W + maxCellSize;
    A.lowbound = 0.4 * W - maxCellSize;
    B.upbound = 0.6 * W + maxCellSize;
    B.lowbound = 0.6 * W - maxCellSize;

    A.size = 0;
    auto index = cellArray.begin();

    for (auto i = cellArray.begin(); i != cellArray.end(); i++)
    {
        i->second->group = 'A';

        for (int j = 0; j < i->second->connectedNets.size(); j++)
        {
            i->second->connectedNets[j]->cells_in_A++;
        }

        A.size += i->second->size;
        if (A.size > A.lowbound && A.size < A.upbound)
        {
            i++;
            index = i;
            break;
        }
    }

    B.size = 0;
    while (index != cellArray.end())
    {
        index->second->group = 'B';

        for (int j = 0; j < index->second->connectedNets.size(); j++)
        {
            index->second->connectedNets[j]->cells_in_B++;
        }

        B.size += index->second->size;
        index++;
    }

    compute_init_gain(cellArray);
    cells_insert_bucket(cellArray, A, B);
}

void compute_init_gain(map<string, Cell *> &cellArray)
{
    for (auto i = cellArray.begin(); i != cellArray.end(); i++)
    {
        Cell *cell = i->second;
        cell->gain = 0;
        for (int j = 0; j < cell->connectedNets.size(); j++)
        {
            cell->gain += calc_FT(cell, cell->connectedNets[j]);
        }
    }
}

int calc_FT(Cell *cell, Net *net)
{
    int countA = 0, countB = 0;
    for (int i = 0; i < net->connectedCells.size(); i++)
    {
        if (net->connectedCells[i]->group == 'A')
        {
            countA++;
        }
        else if (net->connectedCells[i]->group == 'B')
        {
            countB++;
        }
    }

    if (cell->group == 'A' && countA == 1)
    {
        return 1;
    }
    else if (cell->group == 'A' && countB == 0)
    {
        return -1;
    }
    else if (cell->group == 'B' && countB == 1)
    {
        return 1;
    }
    else if (cell->group == 'B' && countA == 0)
    {
        return -1;
    }

    return 0;
}

void cells_insert_bucket(map<string, Cell *> &cellArray, bucketlist &A, bucketlist &B)
{
    for (auto i = cellArray.begin(); i != cellArray.end(); i++)
    {
        Cell *cell = i->second;

        if (cell->group == 'A')
        {
            insert_cell(cell, A);
        }
        else
        {
            insert_cell(cell, B);
        }
    }
}

void insert_cell(Cell *cell, bucketlist &bucket)
{
    if (bucket[cell->gain] == nullptr)
    {
        bucket[cell->gain] = cell;
        cell->next = nullptr;
        cell->prev = nullptr;
        return;
    }

    cell->next = bucket[cell->gain];
    bucket[cell->gain]->prev = cell;
    cell->prev = nullptr;
    bucket[cell->gain] = cell;
}

bool FM(bucketlist &A, bucketlist &B, map<string, Cell *> &cellArray, pair<vector<Cell *>, int> &move_record)
{

    int max_Gk = 0;
    int Gk = 0;
    for (int i = 0; i < cellArray.size(); i++)
    {

        update_maxGain(A, B);

        if (A.max_gain == -Pmax - 1 || B.max_gain == -Pmax - 1)
            break;

        Cell *baseCell = find_baseCell(A, B);

        if (baseCell == nullptr)
            break;

        move_record.first.push_back(baseCell);
        update_gain(baseCell, A, B);

        Gk += baseCell->gain;

        if (baseCell->group == 'A')
        {
            move_baseCell(baseCell, A, B);
        }
        else
        {
            move_baseCell(baseCell, B, A);
        }

        if (Gk > max_Gk)
        {
            move_record.second = move_record.first.size() - 1;
            max_Gk = Gk;
        }
    }

    if (max_Gk > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

Cell *find_baseCell(bucketlist &A, bucketlist &B)
{
    Cell *cellA = A[A.max_gain];
    if (A.size - cellA->size >= A.lowbound)
    {
        cellA->lock = true;
        return cellA;
    }
    else if (A.size + B[B.max_gain]->size <= A.upbound)
    {
        B[B.max_gain]->lock = true;
        return B[B.max_gain];
    }
    else
    {
        return nullptr;
    }
}

void update_gain(Cell *baseCell, bucketlist &A, bucketlist &B)
{
    for (int i = 0; i < baseCell->connectedNets.size(); i++)
    {
        Net *net = baseCell->connectedNets[i];
        do_case(baseCell, net, A, B);
    }
}

void do_case(Cell *baseCell, Net *net, bucketlist &A, bucketlist &B)
{
    int count_from, count_to;

    if (baseCell->group == 'A')
    {
        count_from = net->cells_in_A;
        count_to = net->cells_in_B;
        net->cells_in_A--;
        net->cells_in_B++;
    }
    else
    {
        count_from = net->cells_in_B;
        count_to = net->cells_in_A;
        net->cells_in_A++;
        net->cells_in_B--;
    }

    if (count_to == 0) // case1
    {
        for (int i = 0; i < net->connectedCells.size(); i++)
        {
            Cell *cell = net->connectedCells[i];
            if (cell->lock)
                continue;

            update_cell_index(cell, A, B, cell->gain + 1);
        }
    }

    if (count_to == 1) // case2
    {
        for (int i = 0; i < net->connectedCells.size(); i++)
        {
            Cell *cell = net->connectedCells[i];
            if (cell->lock)
                continue;

            if (cell->group != baseCell->group)
            {
                update_cell_index(cell, A, B, cell->gain - 1);
                break;
            }
        }
    }

    if (count_from == 1) // case3
    {
        for (int i = 0; i < net->connectedCells.size(); i++)
        {
            Cell *cell = net->connectedCells[i];
            if (cell->lock)
                continue;

            if (cell->group != baseCell->group)
            {
                update_cell_index(cell, A, B, cell->gain - 1);
            }
        }
    }

    if (count_from == 2) // case4
    {
        for (int i = 0; i < net->connectedCells.size(); i++)
        {
            Cell *cell = net->connectedCells[i];
            if (cell->lock)
                continue;

            if (cell->group == baseCell->group)
            {
                update_cell_index(cell, A, B, cell->gain + 1);
                break;
            }
        }
    }

    return;
}

void update_cell_index(Cell *cell, bucketlist &A, bucketlist &B, int new_index)
{
    if (cell->group == 'A')
    {
        bucket_remove_cell(cell, A);
        cell->gain = new_index;
        insert_cell(cell, A);
    }
    else
    {
        bucket_remove_cell(cell, B);
        cell->gain = new_index;
        insert_cell(cell, B);
    }
}

void bucket_remove_cell(Cell *cell, bucketlist &bucket)
{
    if (bucket[cell->gain] == cell)
    {
        bucket[cell->gain] = cell->next;
    }

    if (cell->next != nullptr)
    {
        cell->next->prev = cell->prev;
    }

    if (cell->prev != nullptr)
    {
        cell->prev->next = cell->next;
    }
}

void move_baseCell(Cell *baseCell, bucketlist &from, bucketlist &to)
{

    from.size -= baseCell->size;
    to.size += baseCell->size;
    bucket_remove_cell(baseCell, from);

    baseCell->gain = 0;
    if (baseCell->group == 'A')
    {
        baseCell->group = 'B';
    }
    else
    {
        baseCell->group = 'A';
    }
}

void update_maxGain(bucketlist &A, bucketlist &B)
{
    A.max_gain = -Pmax - 1;
    B.max_gain = -Pmax - 1;
    for (int i = Pmax; i >= -Pmax; i--)
    {
        if (A[i] != nullptr)
        {
            A.max_gain = i;
            break;
        }
    }
    for (int i = Pmax; i >= -Pmax; i--)
    {
        if (B[i] != nullptr)
        {
            B.max_gain = i;
            return;
        }
    }
}

void load_best_result(pair<vector<Cell *>, int> &move_record, map<string, Cell *> &cellArray, bucketlist &A, bucketlist &B)
{
    for (int i = move_record.first.size() - 1; i > move_record.second; i--)
    {
        move_record.first[i]->lock = false;
        move_record.first[i]->gain = 0;
        if (move_record.first[i]->group == 'A')
        {
            for (int j = 0; j < move_record.first[i]->connectedNets.size(); j++)
            {
                move_record.first[i]->connectedNets[j]->cells_in_A--;
                move_record.first[i]->connectedNets[j]->cells_in_B++;
            }

            move_record.first[i]->group = 'B';
            B.size += move_record.first[i]->size;
            A.size -= move_record.first[i]->size;
            insert_cell(move_record.first[i], B);
        }
        else
        {
            for (int j = 0; j < move_record.first[i]->connectedNets.size(); j++)
            {
                move_record.first[i]->connectedNets[j]->cells_in_A++;
                move_record.first[i]->connectedNets[j]->cells_in_B--;
            }

            move_record.first[i]->group = 'A';
            A.size += move_record.first[i]->size;
            B.size -= move_record.first[i]->size;
            insert_cell(move_record.first[i], A);
        }
    }

    compute_init_gain(cellArray);
}

void generate_output(double start_time, vector<Net *> &netArray, map<string, Cell *> &cellArray)
{
    double sumAsize = 0, sumBsize = 0;
    for (auto const &i : cellArray)
    {
        if (i.second->group == 'A')
        {
            sumAsize += i.second->size;
        }
        else
        {
            sumBsize += i.second->size;
        }
    }

    ofstream output(outfile_name);

    vector<string> groupA, groupB;
    find_group(groupA, groupB, cellArray);

    vector<string> cut_set;
    find_cutset(cut_set, netArray);

    output << "Cut_size: " << cut_set.size() << "\n";
    output << "G1_size: " << int(sumAsize) << "\n";
    output << "G2_size: " << int(sumBsize) << "\n";
    output << "Time: " << time(NULL) - start_time << ".000 s\n";

    output << "G1:\n";
    for (int i = 0; i < groupA.size(); i++)
    {
        if (i == groupA.size() - 1)
        {
            output << groupA[i];
        }
        else
        {
            output << groupA[i] << " ";
        }
    }
    output << ";\n";

    output << "G2:\n";
    for (int i = 0; i < groupB.size(); i++)
    {
        if (i == groupB.size() - 1)
        {
            output << groupB[i];
        }
        else
        {
            output << groupB[i] << " ";
        }
    }
    output << ";\n";

    output << "Cut_set:\n";
    for (int i = 0; i < cut_set.size(); i++)
    {
        if (i == cut_set.size() - 1)
        {
            output << cut_set[i];
        }
        else
        {
            output << cut_set[i] << " ";
        }
    }
    output << ";\n";
}

void find_group(vector<string> &groupA, vector<string> &groupB, map<string, Cell *> &cellArray)
{
    for (auto const &i : cellArray)
    {
        if (i.second->group == 'A')
        {
            groupA.push_back(i.second->name);
        }
        else
        {
            groupB.push_back(i.second->name);
        }
    }
}

void find_cutset(vector<string> &cut_set, vector<Net *> &netArray)
{
    for (auto const &i : netArray)
    {
        if (net_is_cut(i))
        {
            cut_set.push_back(i->name);
        }
    }
}

bool net_is_cut(Net *net)
{
    int countA = 0;
    for (int i = 0; i < net->connectedCells.size(); i++)
    {
        if (net->connectedCells[i]->group == 'A')
        {
            countA++;
        }
    }

    if (countA == net->connectedCells.size() || countA == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}
