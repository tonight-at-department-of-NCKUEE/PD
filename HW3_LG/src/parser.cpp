#include "../include/parser.h"
#include "parser.h"

//         0         1         2                     3            4
// ./LG_E24096530 –benchmark /benchmark/superblue1 –output /out/superblue1.pl
void parser(char *command[], DataBase &db)
{
    string shapes_file = string(command[2]) + ".shapes";
    string nodes_file = string(command[2]) + ".nodes";
    string nets_file = string(command[2]) + ".nets";
    string legal_file = string(command[2]) + ".legal";
    string scl_file = string(command[2]) + ".scl";

    set<string> nonRect;

    parse_shapes(shapes_file, db, nonRect);
    parse_nodes(nodes_file, db, nonRect);
    parse_legal(legal_file, db);
    parse_nets(nets_file, db);
    parse_scl(scl_file, db);
}

void parse_shapes(string filename, DataBase &db, set<string> &nonRect)
{
    ifstream shapefile(filename);

    if (!shapefile.is_open())
    {
        cout << ".shapes not open" << endl;
        abort();
    }

    cout << "Parsing " << filename << endl;

    string token, s, cellname;

    for (int i = 0; i < 6; i++)
        getline(shapefile, s);

    while (getline(shapefile, s))
    {
        vector<string> line;
        istringstream iss(s);
        while (iss >> token)
        {
            line.push_back(token);
        }

        if (line.size() == 3)
        {
            cellname = line[0];
            nonRect.insert(cellname);
            continue;
        }

        Cell *cell = new Cell(cellname, stod(line[1]), stod(line[2]), stod(line[3]), stod(line[4]));
        db.nonRects.push_back(cell);
    }
}

void parse_nodes(string filename, DataBase &db, set<string> &nonRect)
{
    ifstream nodefile(filename);

    if (!nodefile.is_open())
    {
        cout << ".nodes not open" << endl;
        abort();
    }

    cout << "Parsing " << filename << endl;

    string token, s;

    for (int i = 0; i < 7; i++)
        getline(nodefile, s);

    while (getline(nodefile, s))
    {
        vector<string> line;
        istringstream iss(s);
        while (iss >> token)
        {
            line.push_back(token);
        }

        Cell *cell = new Cell(line[0], 0, 0, stod(line[1]), stod(line[2]));
        db.CellArray[line[0]] = cell;
        if (line.size() == 3)
        {
            db.movables.push_back(cell);
        }
        else if (line[3] == "terminal")
        {
            if (nonRect.count(line[0]))
                cell->notRect = true;
            db.terminals.push_back(cell);
        }
        else if (line[3] == "terminal_NI")
        {
            db.terminal_NIs.push_back(cell);
        }
    }
}

void parse_legal(string filename, DataBase &db)
{
    ifstream legalfile(filename);

    if (!legalfile.is_open())
    {
        cout << ".legal not open" << endl;
        abort();
    }

    cout << "Parsing " << filename << endl;

    string token, s;

    for (int i = 0; i < 4; i++)
        getline(legalfile, s);

    while (getline(legalfile, s))
    {
        vector<string> line;
        istringstream iss(s);
        while (iss >> token)
        {
            line.push_back(token);
        }

        db.CellArray[line[0]]->x = stod(line[1]);
        db.CellArray[line[0]]->y = stod(line[2]);
    }
}

void parse_nets(string filename, DataBase &db)
{
    ifstream netfile(filename);

    if (!netfile.is_open())
    {
        cout << ".nets not open" << endl;
        abort();
    }

    cout << "Parsing " << filename << endl;

    string token, s;
    set<string> visited_cells;
    Net *net;

    for (int i = 0; i < 7; i++)
        getline(netfile, s);

    int tmp = 0;
    while (getline(netfile, s))
    {
        vector<string> line;
        istringstream iss(s);
        while (iss >> token)
            line.push_back(token);

        if (line.size() == 4)
        {
            if (tmp == 1)
                db.NetArray.push_back(net);
            else
                tmp = 1;
            net = new Net(line[3]);
            visited_cells.clear();
        }
        else
        {
            Pin pin;
            pin.cell = db.CellArray[line[0]];
            pin.Xoffset = stod(line[3]);
            pin.Yoffset = stod(line[4]);

            if (visited_cells.count(line[0]) == 0)
            {
                pin.cell->nets.push_back(net);
                visited_cells.insert(line[0]);
            }

            net->pins.push_back(pin);
        }
    }
    db.NetArray.push_back(net);
}

void parse_scl(string filename, DataBase &db)
{
    ifstream sclfile(filename);

    if (!sclfile.is_open())
    {
        cout << ".scl not open" << endl;
        abort();
    }

    cout << "Parsing " << filename << endl;

    string token, s;
    Row row;
    Subrow subrow;
    double sitespace;
    double x1 = INT_MAX, y1 = INT_MAX, x2 = 0, y2 = 0;

    for (int i = 0; i < 6; i++)
        getline(sclfile, s);

    while (getline(sclfile, s))
    {
        vector<string> line;
        istringstream iss(s);
        while (iss >> token)
            line.push_back(token);

        if (line[0] == "Coordinate")
        {
            subrow.y = stod(line[2]);
        }
        else if (line[0] == "Height")
        {
            subrow.height = stod(line[2]);
        }
        else if (line[0] == "Sitespacing")
        {
            sitespace = stod(line[2]);
        }
        else if (line[0] == "SubrowOrigin")
        {
            subrow.x = stod(line[2]);
            subrow.width = sitespace * stod(line[5]);
        }
        else if (line[0] == "End")
        {
            row.subrows.push_back(subrow);
            db.rows.push_back(row);
            row.subrows.clear();
            if (subrow.Top() > y2)
                y2 = subrow.Top();
            if (subrow.Right() > x2)
                x2 = subrow.Right();
            if (subrow.x < x1)
                x1 = subrow.x;
            if (subrow.y < y1)
                y1 = subrow.y;
        }
    }

    db.chip_x1 = x1;
    db.chip_y1 = y1;
    db.chip_x2 = x2;
    db.chip_y2 = y2;
    db.chip_width = x2 - x1;
    db.chip_height = y2 - y1;
    db.rowheight = db.rows[0].subrows[0].height;
}
