#include "../include/parser.h"

void parser(string node_file, string net_file, map<string, Cell *> &cellArray, vector<Net *> &netArray)
{
    set<string> fix_nodes;
    readNodeFile(node_file, cellArray, fix_nodes);
    readNetFile(net_file, cellArray, netArray, fix_nodes);

    /* set<string> useless_cell;
     for (auto const &i : cellArray)
     {
         if (i.second->connectedNets.size() == 0)
         {
             useless_cell.insert(i.second->name);
         }
     }
     for (auto const &i : useless_cell)
     {
         cellArray.erase(i);
     }*/
}

void readNodeFile(string node_file, map<string, Cell *> &cellArray, set<string> &fix_nodes)
{
    ifstream node_list(node_file);
    string s, tmp, tmp1, tmp2, tmp3, tmp4, maxCellname;

    for (int i = 0; i < 6; i++)
        getline(node_list, s);

    node_list >> s;
    while (node_list >> tmp >> tmp1 >> tmp2)
    {

        if (tmp2 == "terminal" || tmp2 == "terminal_NI")
        {
            fix_nodes.insert(s);
            node_list >> s;
            continue;
        }

        Cell *newCell = new Cell;
        newCell->name = s;
        newCell->size = stod(tmp) * stod(tmp1);
        newCell->prev = nullptr;
        newCell->next = nullptr;
        newCell->lock = false;

        W += newCell->size;
        if (newCell->size > maxCellSize)
        {
            maxCellname = newCell->name;
            maxCellSize = newCell->size;
        }
        cellArray[s] = newCell;
        s = tmp2;
    }
}

void readNetFile(string net_file, map<string, Cell *> &cellArray, vector<Net *> &netArray, set<string> &fix_nodes)
{
    ifstream net_list(net_file);
    string s, tmp, tmp1;
    Cell *tmpCell;

    for (int i = 0; i < 7; i++)
        getline(net_list, s);

    while (net_list >> tmp >> tmp >> s >> tmp)
    {
        Net *newNet = new Net;
        newNet->name = tmp;
        int pinNum = stoi(s);
        set<string> cell_in_net;

        for (int i = 0; i < pinNum; i++)
        {
            net_list >> tmp >> tmp1 >> tmp1 >> tmp1 >> tmp1;
            if (cell_in_net.count(tmp) || fix_nodes.count(tmp))
                continue;

            cell_in_net.insert(tmp);
        }

        if (cell_in_net.size() <= 1)
        {
            delete newNet;
            continue;
        }

        newNet->cells_in_A = 0;
        newNet->cells_in_B = 0;
        netArray.push_back(newNet);

        for (auto const &i : cell_in_net)
        {
            tmpCell = cellArray[i];
            tmpCell->connectedNets.push_back(newNet);
            newNet->connectedCells.push_back(tmpCell);
        }

        /*pinNum = cell_in_net.size();
        if (pinNum > Pmax)
        {
            Pmax = pinNum;
        }*/
    }

    Pmax = 0;
    for (auto const &i : cellArray)
    {
        if (i.second->connectedNets.size() > Pmax)
        {
            Pmax = i.second->connectedNets.size();
        }
    }
}
