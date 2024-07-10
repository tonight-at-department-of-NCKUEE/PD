#include "../include/util.h"
#include "util.h"

void build_congestion_map(vector<Tree> trees, DataBase &db)
{
    cout << "Building congestion map" << endl;

    for (int i = 0; i < trees.size(); i++)
    {
        int pin_size = trees[i].deg * 2 - 2;
        for (int j = 0; j < pin_size; j++)
        {
            int neighbor_idx = trees[i].branch[j].n;
            if (neighbor_idx == j)
                continue;

            connect_two_pin(trees[i].branch[j], trees[i].branch[neighbor_idx], db);
        }
    }

    for (int i = 0; i < db.Grids.size(); i++)
    {
        double left_overflow, right_overflow, down_overflow, top_overflow;

        if (db.Grids[i].TopEdge == nullptr)
            top_overflow = 0;
        else
            top_overflow = db.Grids[i].TopEdge->calc_overflow();

        if (db.Grids[i].RightEdge == nullptr)
            right_overflow = 0;
        else
            right_overflow = db.Grids[i].RightEdge->calc_overflow();

        down_overflow = db.Grids[i].DownEdge.calc_overflow();
        left_overflow = db.Grids[i].LeftEdge.calc_overflow();

        db.Grids[i].h_overflow = max(left_overflow, right_overflow);
        db.Grids[i].v_overflow = max(down_overflow, top_overflow);
    }

    cout << "Finished" << endl;
}

void plot_matlab(char *argv[], DataBase &db, string HV)
{
    int overflow_num = 0;
    for (int i = 0; i < db.Grids.size(); i++)
    {
        overflow_num++;
        if (db.Grids[i].h_overflow == 0 && HV == "_h.m")
            break;
        if (db.Grids[i].v_overflow == 0 && HV == "_v.m")
            break;
    }

    string m_file = string(argv[4]);
    string toRemove = ".txt";
    size_t pos = m_file.find(toRemove);
    if (pos != string::npos)
        m_file.erase(pos, toRemove.length());
    m_file = m_file + HV;

    cout << "Plot " << m_file << endl;
    ofstream mfile(m_file);

    if (!mfile.is_open())
    {
        cout << "Plot matlab file failed." << endl;
        abort();
    }
    mfile << "axis equal;\n"
          << "hold on;\n"
          << "grid on;\n";
    mfile << "block_x=[" << db.grid_x1 << " " << db.grid_x1 << " " << db.grid_x2 << " " << db.grid_x2 << " " << db.grid_x1 << "];" << endl;
    mfile << "block_y=[" << db.grid_y1 << " " << db.grid_y2 << " " << db.grid_y2 << " " << db.grid_y1 << " " << db.grid_y1 << "];" << endl;
    mfile << "fill(block_x, block_y, 'b');" << endl;
    for (int i = 0; i < overflow_num / 10; i++)
    {
        double x1 = db.Grids[i].x;
        double y1 = db.Grids[i].y;
        double x2 = db.Grids[i].x + db.Grids[i].width;
        double y2 = db.Grids[i].y + db.Grids[i].height;
        mfile << "block_x=[" << x1 << " " << x1 << " " << x2 << " " << x2 << " " << x1 << "];" << endl;
        mfile << "block_y=[" << y1 << " " << y2 << " " << y2 << " " << y1 << " " << y1 << "];" << endl;
        mfile << "fill(block_x, block_y, 'm','EdgeColor', 'none');" << endl;
        // mfile << "fill(block_x, block_y, 'y','FaceAlpha',0.4);" << endl;
        // mfile << "text(" << m->x << "," << m->y + m->height / 2 << ",'" << m->name << "')" << endl;
    }
    mfile.close();
}

bool comparator_edge(Edge *lhs, Edge *rhs)
{
    return lhs->calc_overflow() > rhs->calc_overflow();
};

void write_output(string filename, DataBase &db, int start_time)
{
    cout << "Write " << filename << endl;
    ofstream file(filename);

    vector<Edge *> Edges;

    double total_overflow = 0;
    int overflow_num = 0;
    for (int i = 0; i < db.DownEdges.size(); i++)
    {
        Edges.push_back(db.DownEdges[i]);
        total_overflow += db.DownEdges[i]->calc_overflow();
        if (db.DownEdges[i]->calc_overflow() > 0)
            overflow_num++;
    }
    for (int i = 0; i < db.LeftEdges.size(); i++)
    {
        Edges.push_back(db.LeftEdges[i]);
        total_overflow += db.LeftEdges[i]->calc_overflow();
        if (db.LeftEdges[i]->calc_overflow() > 0)
            overflow_num++;
    }

    sort(Edges.begin(), Edges.end(), &comparator_edge);

    double ACE05, ACE1, ACE2, ACE5;
    double overflow_sum = 0;
    for (int i = 0; i < int(overflow_num / 20) + 1; i++)
    {
        overflow_sum += Edges[i]->calc_overflow();
        if (i == int(overflow_num / 200))
            ACE05 = overflow_sum / int(overflow_num / 200);
        if (i == int(overflow_num / 100))
            ACE1 = overflow_sum / int(overflow_num / 100);
        if (i == int(overflow_num / 50))
            ACE2 = overflow_sum / int(overflow_num / 50);
        if (i == int(overflow_num / 20))
            ACE5 = overflow_sum / int(overflow_num / 20);
    }

    if (!file.is_open())
    {
        cout << "Write output file failed." << endl;
        abort();
    }

    file << "runtime : " << time(NULL) - start_time << " s\n";
    file << "\n";
    file << "total overflow : " << total_overflow << "\n";
    file << "\n";
    file << "ACE\t0.50%\t1.00%\t2.00%\t5.00%\n ";
    file << "\n";
    file << "\t" << ACE05 << "\t" << ACE1 << "\t" << ACE2 << "\t" << ACE5 << "\n ";
}

double total_hpwl(vector<Net *> &NetArray)
{
    double sum = 0;

    for (int i = 0; i < NetArray.size(); i++)
    {
        sum += net_hpwl(NetArray[i]);
    }

    return sum;
}

void write_pl(DataBase &db, char *filename)
{
    const char *path = filename;
    ofstream pl;
    pl.open(path);

    if (!pl.is_open())
    {
        cout << filename << " not open" << endl;
        abort();
    }

    cout << "Writing file: " << filename << endl;

    pl << "UCLA pl 1.0" << endl;
    pl << "#Created : Sep 17 2012" << endl;
    pl << "#User : Natarajan Viswanathan, IBM Corporation, Austin(nviswan @us.ibm.com) " << endl;

    for (int i = 0; i < db.movables.size(); i++)
    {
        pl << db.movables[i]->name << "\t" << db.movables[i]->x << "\t" << db.movables[i]->y << "\t"
           << ":  N\n";
    }

    for (int i = 0; i < db.terminals.size(); i++)
    {
        pl << db.terminals[i]->name << "\t" << db.terminals[i]->x << "\t" << db.terminals[i]->y << "\t"
           << ":  N  /FIXED\n";
    }

    for (int i = 0; i < db.terminal_NIs.size(); i++)
    {
        pl << db.terminal_NIs[i]->name << "\t" << db.terminal_NIs[i]->x << "\t" << db.terminal_NIs[i]->y << "\t"
           << ":  N  /FIXED_NI\n";
    }

    pl.close();
}
