#include "../include/parser.h"
#include "parser.h"

//         0       1      2                                 3        4
// ./CE_E24096530 –bench benchmarks/adaptec1/adaptec1.aux –output CE_E24096530_adaptec1.txt
void parser(char *command[], DataBase &db)
{
    string aux_file = string(command[2]);
    string toRemove = ".aux";
    size_t pos = aux_file.find(toRemove);
    if (pos != string::npos)
        aux_file.erase(pos, toRemove.length());

    string shapes_file = aux_file + ".shapes";
    string nodes_file = aux_file + ".nodes";
    string nets_file = aux_file + ".nets";
    string pl_file = aux_file + ".pl";
    string scl_file = aux_file + ".scl";
    string route_file = aux_file + ".route";

    set<string> nonRect;

    parse_shapes(shapes_file, db, nonRect);
    parse_nodes(nodes_file, db, nonRect);
    parse_pl(pl_file, db);
    parse_nets(nets_file, db);
    parse_scl(scl_file, db);
    parse_route(route_file, db);

    cout << "Parsing finished" << endl;
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

void parse_pl(string filename, DataBase &db)
{
    ifstream plfile(filename);

    if (!plfile.is_open())
    {
        cout << ".pl not open" << endl;
        abort();
    }

    cout << "Parsing " << filename << endl;

    string token, s;

    for (int i = 0; i < 4; i++)
        getline(plfile, s);

    while (getline(plfile, s))
    {
        vector<string> line;
        istringstream iss(s);
        while (iss >> token)
        {
            line.push_back(token);
        }

        if (line.size() == 0)
            continue;

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

        if (line.size() == 0)
            continue;

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
        else if (line[0] == "END")
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

void parse_route(string filename, DataBase &db)
{
    int HorizontalNum = 0, VerticalNum = 0, LayerNum = 0;
    double x, y, width, height;
    vector<double> MinWireWidth;
    vector<double> MinWireSpacing;

    ifstream routefile(filename);

    if (!routefile.is_open())
    {
        cout << ".route not open" << endl;
        abort();
    }

    cout << "Parsing " << filename << endl;

    string token, s, cellname;

    for (int i = 0; i < 4; i++)
        getline(routefile, s);

    while (getline(routefile, s))
    {
        vector<string> line;
        istringstream iss(s);
        while (iss >> token)
        {
            line.push_back(token);
        }

        if (line.size() == 0)
            continue;

        if (line[0] == "Grid")
        {
            HorizontalNum = stoi(line[2]);
            VerticalNum = stoi(line[3]);
            LayerNum = stoi(line[4]);
        }
        else if (line[0] == "MinWireWidth")
        {
            for (int i = 0; i < LayerNum; i++)
                MinWireWidth.push_back(stod(line[i + 2]));
        }
        else if (line[0] == "MinWireSpacing")
        {
            for (int i = 0; i < LayerNum; i++)
                MinWireSpacing.push_back(stod(line[i + 2]));
        }
        else if (line[0] == "GridOrigin")
        {
            x = stod(line[2]);
            y = stod(line[3]);
        }
        else if (line[0] == "TileSize")
        {
            width = stod(line[2]);
            height = stod(line[3]);
        }
        else if (line[0] == "NumBlockageNodes")
            break;
    }

    while (getline(routefile, s))
    {
        vector<string> line;
        istringstream iss(s);
        while (iss >> token)
        {
            line.push_back(token);
        }

        if (line.size() == 6)
        {
            Blockage blockage;
            blockage.cell = db.CellArray[line[0]];
            for (int i = 2; i < line.size(); i++)
                blockage.layers.insert(stoi(line[i]));
            db.blockages.push_back(blockage);
        }
    }

    db.grid_x1 = x;
    db.grid_y1 = y;

    double tmp = y;
    for (int i = 0; i < HorizontalNum; i++)
    {
        y = tmp;
        for (int j = 0; j < VerticalNum; j++)
        {
            Grid grid(x, y, width, height);
            init_edge_supply(grid.DownEdge, db, MinWireWidth, MinWireSpacing);
            init_edge_supply(grid.LeftEdge, db, MinWireWidth, MinWireSpacing);
            y += height;
            db.Grids.push_back(grid);
        }
        x += width;
    }

    db.grid_x2 = x;
    db.grid_y2 = y;
    db.grid_height = height;
    db.grid_width = width;

    for (int i = 0; i < db.Grids.size(); i++)
    {
        db.DownEdges.push_back(&db.Grids[i].DownEdge);
    }

    int idx = 0;
    for (int i = 0; i < VerticalNum; i++)
    {
        idx = i;
        for (int j = 0; j < HorizontalNum; j++)
        {
            db.LeftEdges.push_back(&db.Grids[idx].LeftEdge);
            idx += VerticalNum;
        }
    }

    db.HorizontalNum = HorizontalNum;
    db.VerticalNum = VerticalNum;

    for (int i = 0; i < db.Grids.size() - 1; i++)
    {
        db.Grids[i].TopEdge = &db.Grids[i + 1].DownEdge;
        if (i + VerticalNum < db.Grids.size())
            db.Grids[i].RightEdge = &db.Grids[i + VerticalNum].LeftEdge;
    }
}

void init_edge_supply(Edge &edge, DataBase &db, vector<double> &MinWireWidth, vector<double> &MinWireSpacing)
{
    int LayerNum = MinWireSpacing.size();
    edge.supply = 0;
    for (int i = 0; i < LayerNum; i++)
    {
        double length = edge.Length();
        for (int j = 0; j < db.blockages.size(); j++)
        {
            if (db.blockages[j].layers.count(i + 1) == 0)
                continue;
            length = length - overlapLength(edge, db.blockages[j]);
        }

        edge.supply += length / (MinWireWidth[i] + MinWireSpacing[i]);
    }
}

double overlapLength(Edge &edge, Blockage &blockage)
{
    double length = 0.0;

    Cell *cell = blockage.cell;

    if (segmentsOverlap(edge.x1, edge.x2, cell->x, cell->Right()) == false)
        return length;

    if (segmentsOverlap(edge.y1, edge.y2, cell->y, cell->Top()) == false)
        return length;

    double x_overlap = min(edge.x2, cell->Right()) - max(edge.x1, cell->x);
    double y_overlap = min(edge.y2, cell->Top()) - max(edge.y1, cell->y);

    length = sqrt(x_overlap * x_overlap + y_overlap * y_overlap);

    return length;
}

bool segmentsOverlap(double a_start, double a_end, double b_start, double b_end)
{
    return max(a_start, b_start) <= min(a_end, b_end);
}
