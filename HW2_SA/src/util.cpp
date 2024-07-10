#include "../include/util.h"
#include "util.h"

void init_constant(char *argv[], vector<Module *> &ModuleArray)
{
    desired_aspect_ratio = stod(argv[2]);
    whitespace = stod(argv[3]) / 100;

    for (int i = 0; i < ModuleArray.size(); i++)
    {
        A += ModuleArray[i]->height * ModuleArray[i]->width;
    }

    chip_height = sqrt((1 + whitespace) * A * desired_aspect_ratio);
    chip_width = sqrt((1 + whitespace) * A / desired_aspect_ratio);
}

void init_tree(BStarTree &tree, vector<Module *> &ModuleArray)
{
    sort(ModuleArray.begin(), ModuleArray.end(), &comparator);

    tree.root = ModuleArray[0];
    Module *left_top = tree.root;
    Module *last_one = tree.root;
    tree.root->set_coordinate(0, 0);

    for (int i = 1; i < ModuleArray.size(); i++)
    {
        Module *target = ModuleArray[i];
        if (last_one->x + last_one->width + target->width > chip_width)
        {
            left_top->put_top(target);
            target->x = left_top->x;
            left_top = target;
        }
        else
        {
            last_one->put_right(target);
            target->x = last_one->x + last_one->width;
        }

        last_one = target;
    }
}

void BStarTree::packing(Module *node)
{

    if (node == nullptr)
        return;

    if (node == this->root)
    {
        node->x = 0;
    }
    else if (node == node->parent->left)
    {
        node->x = node->parent->x + node->parent->width;
    }
    else if (node == node->parent->right)
    {
        node->x = node->parent->x;
    }
    // cout << node->name << endl;

    update_contour(*this, node);
    /*cout << node->x << endl;
    cout << node->y << endl;
    while (1)
    {
        cout << contour->y << "(" << contour->x1 << "," << contour->x2 << ") ";
        contour = contour->next;
        if (contour->x1 == 0)
            break;
    }*/

    // getchar();

    packing(node->left);
    packing(node->right);
}

void BStarTree::reset_contour()
{
    Contour *tmp;
    contour = contour->next;
    while (contour->x1 != 0)
    {
        tmp = contour->next;
        delete contour;
        contour = tmp;
    }
    delete contour;
    contour = nullptr;
}

void update_contour(BStarTree &tree, Module *m)
{
    Contour *newc = new Contour;
    newc->x1 = m->x;
    newc->x2 = m->x + m->width;
    newc->y = 0;
    m->y = 0;

    if (tree.contour == nullptr)
    {
        newc->y = m->height;
        tree.contour = newc;
        tree.contour->next = tree.contour;
        tree.contour->prev = tree.contour;
        return;
    }

    while (1)
    {
        if (newc->x1 >= tree.contour->x2)
        {
            if (tree.contour->next->x1 == 0)
                break;

            tree.contour = tree.contour->next;
            continue;
        }

        if (tree.contour->y + m->height > newc->y)
        {
            newc->y = tree.contour->y + m->height;
            m->y = tree.contour->y;
        }

        if (tree.contour->next->x1 == 0 || tree.contour->next->x1 >= newc->x2)
            break;
        tree.contour = tree.contour->next;
    }

    if (m->y == 0)
        newc->y = m->height;

    while (tree.contour->x1 != 0)
    {
        tree.contour = tree.contour->next;
    }
    Contour *tmp;

    /*cout << "m->y:" << m->y << endl;
    cout << "newc->y:" << newc->y << endl;
    cout << "m.width:" << m->width << endl;
    cout << "new contour:" << newc->y << "(" << newc->x1 << "," << newc->x2 << ")" << endl;
    cout << "tree contour:" << tree.contour->y << "(" << tree.contour->x1 << "," << tree.contour->x2 << ")" << endl;*/
    while (1)
    {
        if (tree.contour->next->x1 == 0 && tree.contour->x2 == newc->x1)
        {
            newc->next = tree.contour->next;
            tree.contour->next->prev = newc;
            tree.contour->next = newc;
            newc->prev = tree.contour;
            break;
        }

        tmp = tree.contour->next;
        if (newc->x1 <= tree.contour->x1 && newc->x2 < tree.contour->x2 && newc->x2 > tree.contour->x1)
        {
            if (tree.contour->prev != newc)
            {
                tree.contour->prev->next = newc;
                newc->prev = tree.contour->prev;
                tree.contour->prev = newc;
            }
            newc->next = tree.contour;
            tree.contour->x1 = newc->x2;
            break;
        }

        if (newc->x1 > tree.contour->x1 && newc->x1 < tree.contour->x2 && newc->x2 >= tree.contour->x2)
        {
            tree.contour->next = newc;
            newc->prev = tree.contour;
            tree.contour->x2 = newc->x1;
        }

        if (newc->x1 <= tree.contour->x1 && newc->x2 >= tree.contour->x2)
        {
            if (tree.contour->next == tree.contour)
            {
                tree.contour->x1 = newc->x1;
                tree.contour->x2 = newc->x2;
                tree.contour->y = newc->y;
                delete newc;
                break;
            }

            if (tree.contour->prev != newc)
            {
                tree.contour->prev->next = newc;
                newc->prev = tree.contour->prev;
            }
            tree.contour->next->prev = newc;
            newc->next = tree.contour->next;
            delete tree.contour;
        }

        tree.contour = tmp;
        if (tree.contour->x1 == 0)
            break;
    }

    while (tree.contour->x1 != 0)
    {
        tree.contour = tree.contour->next;
    }
}

void simulated_annealing(BStarTree &tree, vector<Module *> &ModuleArray, vector<net_data> &nets)
{
    double gamma = 0.99;
    double temperature = 1000000;
    double W;
    double H;
    double current_cost = cost(tree, ModuleArray);
    double prev_cost = current_cost;
    double best_cost = current_cost;

    vector<Module *> best_tree;
    vector<m_data> best_data;
    vector<Module *> prev_tree;
    vector<m_data> prev_data;
    store_tree(prev_tree, tree, prev_data);
    store_tree(best_tree, tree, best_data);

    long long count = 0;
    while (1)
    {
        count++;

        int pertubation = rand() % 4;
        if (pertubation == 0)
        {
            rotate(ModuleArray);
        }
        else if (pertubation == 1 || pertubation == 3)
        {
            exchange(ModuleArray, tree);
        }
        else if (pertubation == 2)
        {
            delete_insert(ModuleArray, tree);
        }

        tree.reset_contour();
        tree.packing(tree.root);

        W = calc_W(tree.contour);
        H = calc_H(tree.contour);

        current_cost = cost(tree, ModuleArray);
        if (H <= chip_height && W <= chip_width && HPWL(ModuleArray, nets) < max_hpwl)
        {
            cout << "iteration " << count << " accept, cost = " << current_cost << endl;
            cout << "H:" << H << "/" << chip_height << ", W:" << W << "/" << chip_width << endl;
            store_tree(best_tree, tree, best_data);
            break;
        }
        else if (count % 100000 == 0)
        {
            temperature = 1000000;
            prev_cost = 100;
            best_cost = 100;
        }
        else if (count > 500000)
            break;

        if (current_cost <= prev_cost)
        {
            prev_cost = current_cost;
            store_tree(prev_tree, tree, prev_data);
            if (current_cost < best_cost)
            {
                best_cost = current_cost;
                store_tree(best_tree, tree, best_data);
            }
            // cout << "iteration " << count << " accept, cost = " << current_cost << endl;
            // cout << "H:" << H << "/" << chip_height << ", W:" << W << "/" << chip_width << endl;
        }
        else
        {
            double random_num = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
            double delta_c = current_cost - prev_cost;
            if (random_num < exp(-(delta_c / temperature)))
            {
                prev_cost = current_cost;
                store_tree(prev_tree, tree, prev_data);
                // cout << "*iteration " << count << " accept, cost = " << current_cost << endl;
            }
            else
            {
                load_tree(prev_tree, tree, prev_data);
            }
        }
        temperature *= gamma;
    }

    cout << "best cost:" << best_cost << endl;
    load_tree(best_tree, tree, best_data);
    tree.reset_contour();
    tree.packing(tree.root);
}

void store_tree(vector<Module *> &s, BStarTree &tree, vector<m_data> &data)
{
    s.clear();
    data.clear();
    queue<Module *> q;

    q.push(tree.root);

    while (q.empty() == false)
    {
        Module *node = q.front();
        s.push_back(node);
        q.pop();

        m_data m;
        if (node != nullptr)
        {
            m.height = node->height;
            m.width = node->width;
            m.name = node->name;
            q.push(node->left);
            q.push(node->right);
        }

        data.push_back(m);
    }
}

void load_tree(vector<Module *> &s, BStarTree &tree, vector<m_data> &data)
{
    s[0]->left = nullptr;
    s[0]->right = nullptr;
    s[0]->name = data[0].name;
    s[0]->height = data[0].height;
    s[0]->width = data[0].width;
    tree.root = s[0];
    queue<Module *> q;
    q.push(tree.root);

    int i = 1;
    int tmp = 0;
    while (q.empty() == false)
    {
        Module *node = q.front();

        if (s[i] != nullptr)
        {
            s[i]->name = data[i].name;
            s[i]->height = data[i].height;
            s[i]->width = data[i].width;
            s[i]->left = nullptr;
            s[i]->right = nullptr;
            q.push(s[i]);
            s[i]->parent = node;
        }

        if (node->left == nullptr && tmp == 0)
        {
            node->left = s[i];
            tmp = 1;
        }
        else
        {
            node->right = s[i];
            q.pop();
            tmp = 0;
        }
        i++;
    }
}

void rotate(vector<Module *> &ModuleArray)
{
    int target_idx = rand() % ModuleArray.size();
    Module *M = ModuleArray[target_idx];
    // cout << "rotate " << M->name << endl;
    swap(M->height, M->width);
}

void exchange(vector<Module *> &ModuleArray, BStarTree &tree)
{
    int idx1 = rand() % ModuleArray.size();
    int idx2 = rand() % ModuleArray.size();
    while (idx1 == idx2)
        idx2 = rand() % ModuleArray.size();

    Module *M1 = ModuleArray[idx1];
    Module *M2 = ModuleArray[idx2];
    // cout << "exchange " << M1->name << " " << M2->name << endl;

    swap(M1->height, M2->height);
    swap(M1->width, M2->width);
    swap(M1->name, M2->name);
    // swap(M1->connectedNets, M2->connectedNets);

    /*Module *new_p_parent = M2->parent;
    Module *new_p_left = M2->left;
    Module *new_p_right = M2->right;
    Module **new_p_link = &tree.root;
    if (M2->parent)
        new_p_link = M2->parent->left == M2 ? &M2->parent->left : &M2->parent->right;

    Module *new_q_parent = M1->parent;
    Module *new_q_left = M1->left;
    Module *new_q_right = M1->right;
    Module **new_q_link = &tree.root;
    if (M1->parent)
        new_q_link = M1->parent->left == M1 ? &M1->parent->left : &M1->parent->right;

    if (M2->parent == M1)
    {
        new_p_parent = M2;
        new_p_link = nullptr;
        if (M1->left == M2)
            new_q_left = M1;
        else
            new_q_right = M1;
    }
    else if (M1->parent == M2)
    {
        new_q_parent = M1;
        new_q_link = nullptr;
        if (M2->left == M1)
            new_p_left = M2;
        else
            new_p_right = M2;
    }

    M1->parent = new_p_parent;
    M1->left = new_p_left;
    if (M1->left)
        M1->left->parent = M1;
    M1->right = new_p_right;
    if (M1->right)
        M1->right->parent = M1;
    if (new_p_link)
        *new_p_link = M1;

    M2->parent = new_q_parent;
    M2->left = new_q_left;
    if (M2->left)
        M2->left->parent = M2;
    M2->right = new_q_right;
    if (M2->right)
        M2->right->parent = M2;
    if (new_q_link)
        *new_q_link = M2;*/
}

void delete_insert(vector<Module *> &ModuleArray, BStarTree &tree)
{
    int idx = rand() % ModuleArray.size();
    while (ModuleArray[idx] == tree.root || (ModuleArray[idx]->left != nullptr && ModuleArray[idx]->right != nullptr))
    {
        idx = rand() % ModuleArray.size();
    }

    Module *target = ModuleArray[idx];
    // cout << "delete " << target->name << endl;
    if (target == target->parent->right)
    {
        if (target->right == nullptr && target->left == nullptr)
        {
            target->parent->right = nullptr;
        }
        else if (target->right == nullptr)
        {
            target->parent->right = target->left;
            target->left->parent = target->parent;
        }
        else
        {
            target->parent->right = target->right;
            target->right->parent = target->parent;
        }
    }
    else
    {
        if (target->right == nullptr && target->left == nullptr)
        {
            target->parent->left = nullptr;
        }
        else if (target->right == nullptr)
        {
            target->parent->left = target->left;
            target->left->parent = target->parent;
        }
        else
        {
            target->parent->left = target->right;
            target->right->parent = target->parent;
        }
    }

    while (1)
    {
        idx = rand() % ModuleArray.size();
        if (ModuleArray[idx]->name == target->name)
            continue;

        if (ModuleArray[idx]->right == nullptr)
        {
            ModuleArray[idx]->right = target;
            target->parent = ModuleArray[idx];
            break;
        }
        else if (ModuleArray[idx]->left == nullptr)
        {
            ModuleArray[idx]->left = target;
            target->parent = ModuleArray[idx];
            break;
        }
    }

    target->right = nullptr;
    target->left = nullptr;
}

double cost(BStarTree &tree, vector<Module *> &ModuleArray)
{
    // double hpwl = HPWL(ModuleArray);
    double hpwl = 0;
    double a = Alpha;
    double b = Beta;
    double Rstar = desired_aspect_ratio;
    double R = current_aspect_ratio(tree.contour);
    double W = calc_W(tree.contour);
    double H = calc_H(tree.contour);
    A = W * H;

    return a * A + b * hpwl + (1 - a - b) * pow((Rstar - R), 2);
}

double current_aspect_ratio(Contour *c)
{
    double h = 0, w = 0;
    w = c->prev->x2;
    while (1)
    {
        if (c->y > h)
            h = c->y;
        if (c->next->x1 == 0)
            break;
        c = c->next;
    }
    return h / w;
}

double HPWL(vector<Module *> &ModuleArray, vector<net_data> &nets)
{
    double hpwl = 0;
    double xmax = 0;
    double ymax = 0;
    double xmin = INT_MAX;
    double ymin = INT_MAX;

    for (int i = 0; i < nets.size(); i++)
    {
        for (int j = 0; j < nets[i].modules.size(); j++)
        {
            for (int k = 0; k < ModuleArray.size(); k++)
            {
                Module *m = ModuleArray[k];
                if (m->name != nets[i].modules[j])
                    continue;
                if (m->x + m->width / 2 > xmax)
                    xmax = m->x + m->width / 2;
                if (m->x + m->width / 2 < xmin)
                    xmin = m->x + m->width / 2;
                if (m->y + m->height / 2 > ymax)
                    ymax = m->y + m->height / 2;
                if (m->y + m->height / 2 < ymin)
                    ymin = m->y + m->height / 2;
                break;
            }
        }
        hpwl += xmax + ymax - xmin - ymin;
        xmax = 0;
        ymax = 0;
        xmin = INT_MAX;
        ymin = INT_MAX;
    }

    return hpwl;
}

double calc_H(Contour *c)
{
    double R = current_aspect_ratio(c);
    double W = calc_W(c);
    return R * W;
}

double calc_W(Contour *c)
{
    return c->prev->x2;
}

void plot_mfile(string filename, vector<Module *> &ModuleArray)
{
    ofstream mfile(filename);

    if (!mfile.is_open())
    {
        cout << "Plot matlab file failed." << endl;
        abort();
    }
    mfile << "axis equal;\n"
          << "hold on;\n"
          << "grid on;\n";
    mfile << "block_x=[0 0 " << chip_width << " " << chip_width << " 0];" << endl;
    mfile << "block_y=[0 " << chip_height << " " << chip_height << " 0 0];" << endl;
    mfile << "fill(block_x, block_y, 'c');" << endl;
    for (int i = 0; i < ModuleArray.size(); i++)
    {
        Module *m = ModuleArray[i];
        mfile << "block_x=[" << m->x << " " << m->x << " " << m->x + m->width << " " << m->x + m->width << " " << m->x << "];" << endl;
        mfile << "block_y=[" << m->y << " " << m->y + m->height << " " << m->y + m->height << " " << m->y << " " << m->y << "];" << endl;
        mfile << "fill(block_x, block_y, 'y','FaceAlpha',0.4);" << endl;
        mfile << "text(" << m->x << "," << m->y + m->height / 2 << ",'" << m->name << "')" << endl;
    }
    mfile.close();
}

void write_outfile(string filename, vector<Module *> &ModuleArray, vector<m_data> &data)
{
    ofstream out(filename);
    if (!out.is_open())
    {
        cout << ".out file not open" << endl;
        abort();
    }

    for (int i = 0; i < ModuleArray.size(); i++)
    {
        int R = 0;
        Module *M = ModuleArray[i];
        for (int j = 0; j < data.size(); j++)
        {
            if (M->name != data[j].name)
                continue;

            if (M->height == data[j].width)
                R = 1;

            break;
        }
        out << M->name << " " << M->x << " " << M->y << " " << R << "\n";
    }
    out.close();
}

void write_txtfile(char *argv[], vector<Module *> &ModuleArray, int run_time, int hpwl)
{
    ofstream txt(argv[4]);
    if (!txt.is_open())
    {
        cout << ".txt file not open" << endl;
        abort();
    }

    txt << "Benchmark : " << argv[1] << "\n";
    txt << "ratio : " << argv[2] << "\n";
    txt << "whitespace : " << argv[3] << "%\n";
    txt << "Runtime : " << run_time << "s\n";
    txt << "Wirelength : " << hpwl << "\n";
    txt << "Chip_width = " << chip_width << "\n";
    txt << "Chip_height = " << chip_height << "\n";
    txt.close();
}

void save_init_data(vector<m_data> &data, vector<Module *> &ModuleArray, map<string, Net *> NetArray, vector<net_data> &nets)
{
    for (int i = 0; i < ModuleArray.size(); i++)
    {
        m_data d;
        d.name = ModuleArray[i]->name;
        d.height = ModuleArray[i]->height;
        d.width = ModuleArray[i]->width;
        data.push_back(d);
    }

    for (auto &&i : NetArray)
    {
        net_data n;
        n.name = i.second->name;
        for (int j = 0; j < i.second->connectedModules.size(); j++)
        {
            n.modules.push_back(i.second->connectedModules[j]->name);
        }
        nets.push_back(n);
    }
}
