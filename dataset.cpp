#include "dataset.h"

Dataset::Dataset(
    void (*normalizarion_function)(Dataset *),
    void (*renormalize_function)(Dataset *, vector<DataType> &)) : _normalize{normalizarion_function}, _re_normalize{renormalize_function}, _size{0}
{
}

Dataset::Dataset(const string &path)
{
    *this = read_csv(path);
}

Dataset Dataset::read_csv(const string &path)
{
    Dataset dataset;
    ifstream data(path, std::ios::in);
    if (not data.is_open())
    {
        cerr << path << " : No such file or the path is incorrect";
        return dataset;
    }

    string line;

    getline(data, line);
    if (line.back() == '\r')
        line.pop_back(); // remove the \r char.

    int i = 0, j = 0;

    while (true)
    {
        j = line.find(',', i);
        int len = (j == string::npos) ? string::npos : j - i;
        dataset.keys.push_back(line.substr(i, len));
        dataset.m.insert(make_pair(dataset.keys.back(), vector<DataType>()));
        if (j == string::npos)
        {
            break;
        }
        i = j + 1;
    }

    getline(data, line);
    if (line.back() == '\r')
        line.pop_back();
    int _ = 0;
    i = j = 0;
    while (true)
    {
        j = line.find(',', i);
        int len = (j == string::npos) ? string::npos : j - i;
        string entry = line.substr(i, len);
        if (is_numeric(entry))
        {
            dataset._is_numeric.push_back(true);
            dataset.m[dataset.keys[_]].push_back(atof(entry.c_str()));
        }
        else
        {
            dataset._is_numeric.push_back(false);
            dataset.m[dataset.keys[_]].push_back(entry);
        }
        if (j == string::npos)
        {
            break;
        }
        ++_;
        i = j + 1;
    }

    while (getline(data, line))
    {
        if (line.back() == '\r')
            line.pop_back();
        _ = i = j = 0;
        while (true)
        {
            j = line.find(',', i);
            int len = (j == string::npos) ? string::npos : j - i;
            string entry = line.substr(i, len);

            if (dataset._is_numeric[_])
            {
                dataset.m[dataset.keys[_]].push_back(atof(entry.c_str()));
            }
            else
            {
                dataset.m[dataset.keys[_]].push_back(entry);
            }
            if (j == string::npos)
            {
                break;
            }
            ++_;
            i = j + 1;
        }
    }
    dataset._size = dataset.m[dataset.keys.front()].size();
    data.close();
    return dataset;
}

void Dataset::normalize()
{
    _normalize(this);
}

void Dataset::print()
{
    PrettyTable tabel(get_attributes());

    for (size_t i = 0; i < _size; ++i)
    {
        auto row = iterrow(i);
        vector<string> table_row;
        for (auto &&j : row)
        {
            if (holds_alternative<double>(j))
            {
                table_row.push_back(to_string(get<double>(j)));
            }
            else
            {
                table_row.push_back(get<string>(j));
            }
        }
        tabel.add_row(table_row);
    }

    tabel.display();
    cout << "\n\nTotal printed records: " << _size << "\n";
}

void Dataset::scatter_plot(const string &x, const string &y, tuple<double, double> target)
{
    if (not has_attribute(x))
    {
        cout << "failed, attribute `" << x << "` not found.\n";
        return;
    }

    if (not has_attribute(y))
    {
        cout << "failed, attribute `" << y << "` not found.\n";
        return;
    }

    if (not holds_alternative<double>(m[x].front()))
    {
        cout << "`" << y << "` is not a numerical type.\n";
        return;
    }

    if (not holds_alternative<double>(m[x].front()))
    {
        cout << "`" << y << "` is not a numerical type.\n";
        return;
    }

    Gnuplot gp;

    gp << "set title 'iris dataset'\n";
    gp << "set xlabel '" << x << "' \n";
    gp << "set ylabel '" << y << "' \n";

    if (label.length() == 0)
    {
        gp << "plot '-' with points title 'species' pt 7 lc 1\n";

        for (int i = 0; i < m[keys.front()].size(); ++i)
        {
            gp << get<double>(m[x][i]) << " " << get<double>(m[y][i]) << '\n';
        }
    }
    else
    {
        unordered_map<DataType, int> colors;
        int counter = 10;

        gp << "plot '-' with points title '' pt 7 lc variable\n";

        for (int i = 0; i < m[keys.front()].size(); ++i)
        {
            if (colors[m[label][i]] == 0)
            {
                colors[m[label][i]] = counter++;
            }

            gp << get<double>(m[x][i]) << " " << get<double>(m[y][i]) << " " << colors[m[label][i]] << '\n';
        }
        gp << get<0>(target) << " " << get<1>(target) << " " << 40;
    }

    gp << "e\n";
    getchar();
}

void Dataset::set_label(const string &label)
{
    if (has_attribute(label))
    {
        this->label = label;
    }
    else
    {
        cout << '`' << label << "` not found, current label not changed.\n";
    }
}

string Dataset::get_label() const
{
    if (label.size() == 0)
    {
        throw "no label set yet, an empty strin returned.\n";
        return "";
    }
    return label;
}

vector<string> Dataset::get_attributes() const
{
    return keys;
}
vector<Dataset::DataType> &Dataset::operator[](const string &attribute)
{
    if (has_attribute(attribute))
    {
        return m[attribute];
    }
    else
    {
        cout << "failed, attribute `" << attribute << "` not found.\n";
    }
    static vector<DataType> _;
    return _;
}

void Dataset::set_normalize_function(void (*normalization_function)(Dataset *))
{
    _normalize = normalization_function;
}
vector<string> Dataset::get_numerics() const
{
    vector<string> numerics;
    for (size_t i = 0; i < keys.size(); i++)
    {
        if (_is_numeric[i])
        {
            numerics.push_back(keys[i]);
        }
    }
    return numerics;
}

void Dataset::renormalize(vector<DataType> &data_point)
{
    _re_normalize(this, data_point);
}
void Dataset::set_renormalize(void (*renormalize_function)(Dataset *, vector<DataType> &))
{
    _re_normalize = renormalize_function;
}

vector<Dataset::DataType> Dataset::iterrow(int at)
{
    try
    {
        if (at > _size or at < 0)
        {
            throw "index out of range, empty data point returnd.\n";
            return {};
        }
        vector<DataType> data_point;
        for (auto &key : keys)
        {
            data_point.push_back(m[key][at]);
        }
        return data_point;
    }

    catch (const std::string &e)
    {
        std::cerr << e << '\n';
        return {};
    }
}

vector<reference_wrapper<Dataset::DataType>> Dataset::iterrow_ref(int at)
{
    try
    {
        if (at > _size or at < 0)
        {
            throw "index out of range, empty data point returnd.\n";
        }

        vector<reference_wrapper<DataType>> data_point;
        for (auto &key : keys)
        {
            data_point.push_back(m[key][at]);
        }
        return data_point;
    }
    catch (const std::string &e)
    {
        std::cerr << e << '\n';
        return {};
    }
}
int Dataset::no_rows() const
{
    return _size;
}

void Dataset::push_back(const vector<DataType> &row)
{
    for (size_t i = 0; i < keys.size(); i++)
    {
        m[keys[i]].push_back(row[i]);
    }
    ++_size;
}

void Dataset::remove(int at)
{
    if (at > _size or at < 0)
    {
        throw range_error("index out of range.\n");
        return;
    }
    for (size_t i = 0; i < keys.size(); i++)
    {
        swap(m[keys[i]][i], m[keys[i]].back());
        m[keys[i]].pop_back();
    }
}
void Dataset::split(Dataset &train, Dataset &test, double ratio)
{
    if (ratio < 0.0 || ratio > 1.0)
    {
        throw range_error("ratio should to be >= 0 and <= 1.\n");
        return;
    }

    test.keys = train.keys = keys;
    test._is_numeric = train._is_numeric = _is_numeric;
    test.label = train.label = label;
    vector<int> indicies(_size);
    iota(indicies.begin(), indicies.end(), 0);

    random_shuffle(indicies.begin(), indicies.end());
    unsigned long long _ = ratio * _size;
    size_t i = 0;
    
    for (; i < _; i++)
    {
        train.push_back(iterrow(indicies[i]));
    }

    for (; i < _size; ++i)
    {
        test.push_back(iterrow(indicies[i]));
    }
}

void Dataset::to_csv(const string &path)
{
    ofstream output(path, ios::ios_base::out);
    for (size_t i = 0; i < keys.size(); i++)
    {
        if (i == keys.size() - 1)
        {
            output << keys[i] << endl;
        }
        else
        {
            output << keys[i] << ",";
        }
    }

    for (size_t i = 0; i < _size; i++)
    {
        auto row = iterrow(i);

        for (size_t j = 0; j < row.size(); j++)
        {
            if (j == row.size() - 1)
            {
                if (holds_alternative<double>(row[j]))
                {
                    output << get<double>(row[j]) << endl;
                }
                else
                {
                    output << get<string>(row[j]) << endl;
                }
            }
            else
            {
                if (holds_alternative<double>(row[j]))
                {
                    output << get<double>(row[j]) << ",";
                }
                else
                {
                    output << get<string>(row[j]) << ",";
                }
            }
        }
    }
}

bool Dataset::has_attribute(const string &attribute)
{
    for (size_t i = 0; i < keys.size(); i++)
    {
        if (attribute == keys[i])
        {
            return true;
        }
    }

    return false;
}

Dataset::~Dataset()
{
}

ostream &operator<<(ostream &out, const Dataset::DataType &a)
{
    if (holds_alternative<double>(a))
    {
        out << get<double>(a);
    }
    else
    {
        out << get<string>(a);
    }
    return out;
}

bool Dataset::is_normalized(const vector<Dataset::DataType>& data_point)
{
    for (size_t i = 0; i < keys.size(); i++)
    {
        if (_is_numeric[i])
        {
            if (get<double>(data_point[i]) > get<double>(local_parms[keys[i] + " nmax"]) or get<double>(data_point[i]) < get<double>(local_parms[DataType(keys[i] + " nmin")]))
            {
                return false;
            }
        }
    }
    return true;
}

bool operator==(const Dataset::DataType &a, const Dataset::DataType &b)
{
    if (holds_alternative<double>(a) and holds_alternative<double>(b))
    {
        return get<double>(a) == get<double>(b);
    }
    else
    {
        if (holds_alternative<string>(a) and holds_alternative<string>(b))
        {
            return get<string>(a) == get<string>(b);
        }
    }
    return false;
}

bool operator!=(const Dataset::DataType &a, const Dataset::DataType &b)
{
    return not(a == b);
}

bool is_numeric(const string &str)
{
    regex pat(R"(^[-+]?(\d+\.?\d*|\.\d+)$)"); // also we can make other pattern for just integers
                                              // R"(^[-+]?\d+$)"
    return regex_match(str, pat);
}