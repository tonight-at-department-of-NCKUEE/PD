#include "../include/parser.h"
#include "parser.h"

void parser(string yal_name, vector<Module *> &ModuleArray, map<string, Net *> &NetArray)
{
    ifstream yal_file(yal_name);

    if (!yal_file.is_open())
    {
        cout << "yal file not open" << endl;
        abort();
    }

    parse_modules(yal_file, ModuleArray, NetArray);
    parse_netlist(yal_file, ModuleArray, NetArray);
    yal_file.close();
}

void parse_modules(ifstream &yal_file, vector<Module *> &ModuleArray, map<string, Net *> &NetArray)
{
    string tmp, name, x1, x2, y1, y2;

    while (yal_file >> tmp)
    {
        if (tmp == "NETWORK;")
            break;

        if (tmp != "MODULE")
            continue;

        yal_file >> name;
        name.resize(name.size() - 1);

        yal_file >> tmp >> tmp;
        if (tmp != "GENERAL;")
            continue;

        Module *newModule = new Module;
        yal_file >> tmp >> tmp >> tmp >> x2 >> y2 >> tmp >> tmp >> x1 >> y1;
        y1.resize(y1.size() - 1);
        newModule->width = stod(x2) - stod(x1);
        newModule->height = stod(y2) - stod(y1);
        newModule->name = name;
        ModuleArray.push_back(newModule);
    }
}

void parse_netlist(ifstream &yal_file, vector<Module *> &ModuleArray, map<string, Net *> &NetArray)
{
    string tmp, name;
    int index = 0;
    set<string> net_set;

    while (yal_file >> tmp >> name)
    {
        if (tmp == "ENDNETWORK;")
            break;

        for (int i = 0; i < ModuleArray.size(); i++)
        {
            if (ModuleArray[i]->name == name)
            {
                index = i;
                break;
            }
        }

        set<string> net_in_module;
        while (yal_file >> tmp)
        {
            bool do_next_net = false;
            if (tmp[tmp.size() - 1] == ';')
            {
                do_next_net = true;
                tmp.resize(tmp.size() - 1);
            }

            if (net_set.count(tmp) == 0)
            {
                net_set.insert(tmp);
                Net *net = new Net;
                net->name = tmp;
                net->connectedModules.push_back(ModuleArray[index]);
                NetArray[tmp] = net;
                ModuleArray[index]->connectedNets.push_back(net);
            }
            else if (net_in_module.count(tmp) == 0)
            {
                ModuleArray[index]->connectedNets.push_back(NetArray[tmp]);
                NetArray[tmp]->connectedModules.push_back(ModuleArray[index]);
            }

            net_in_module.insert(tmp);

            if (do_next_net)
                break;
        }
    }
}
