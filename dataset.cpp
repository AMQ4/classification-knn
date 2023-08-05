#include <gnuplot-iostream.h>
#include <iostream>
#include <fstream>
#include <cctype>
#include <variant>
#include <vector>
#include <unordered_map>
#include <regex>
#include <numeric>

using namespace std;

/**
 * @brief Checks if a given string represents a numeric value using regular expressions.
 *
 * @param str The string to check for numeric representation.
 * @return true if the string is numeric, false otherwise.
 */

bool is_numeric(const string &str)
{
    regex pat(R"(^[-+]?(\d+\.?\d*|\.\d+)$)"); // also we can make other pattern for just integers
                                              // R"(^[-+]?\d+$)"
    return regex_match(str, pat);
}
/**
 * @brief Represents a dataset with attributes and values, and provides visualization methods.
 */
class Dataset
{
public:
    using DataType = variant<double, string>;
    Dataset() {}
    ~Dataset() {}
    /**
     * @brief Reads a dataset from a CSV file.
     *
     * @param path The path to the CSV file.
     * @return The populated Dataset object.
     */
    static Dataset read_csv(string path)
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
        data.close();
        return dataset;
    }
    /**
     * @brief Prints the dataset.
     */
    void print()
    {
        int norecords = 1;
        for (size_t i = 0; i < keys.size() - 1; i++)
        {
            cout << keys[i] << ", ";
        }
        cout << keys.back() << "\n";
        int len = m[keys.front()].size();

        for (size_t i = 0; i < len; ++i, ++norecords)
        {
            for (size_t j = 0; j < m.size(); j++)
            {
                if (holds_alternative<double>(m[keys[j]][i]))
                {
                    if (j == m.size() - 1)
                    {
                        cout << get<double>(m[keys[j]][i]) << endl;
                    }
                    else
                    {
                        cout << get<double>(m[keys[j]][i]) << ", ";
                    }
                }
                else
                {
                    if (j == m.size() - 1)
                    {
                        cout << get<string>(m[keys[j]][i]) << endl;
                    }
                    else
                    {
                        cout << get<string>(m[keys[j]][i]) << ", ";
                    }
                }
            }
        }

        cout << "\n\nTotal printed records: " << norecords << "\n";
    }
    /**
     * @brief Creates a scatter plot using Gnuplot.
     *
     * @param x The attribute for the x-axis.
     * @param y The attribute for the y-axis.
     * @param [target] A target point to highlight on the plot (optional).
     */
    void scatter_plot(const string &x, const string &y, tuple<double, double> target = {__DBL_MAX__, __DBL_MAX__})
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
    /**
     * @brief Sets the label attribute for the dataset.
     *
     * @param label The label attribute name.
     */
    void set_label(const string &label)
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
    /**
     * @brief Retrieves the current label attribute.
     *
     * @return The label attribute.
     */
    string get_label() const
    {
        return label;
    }
    vector<string> get_attributes()const
    {
        return keys;
    }
    vector<DataType> operator[](const string& attribute)
    {
        if(has_attribute(attribute))
        {
            return m[attribute];
        }
        else
        {
            cout << "failed, attribute `" << attribute << "` not found.\n";   
        }
    }

private:
    unordered_map<string, vector<DataType>> m;
    vector<string> keys;
    string label;
    vector<bool> _is_numeric;
    void (*normalize)(Dataset&);

    /**
     * @brief Checks if the dataset has a specific attribute.
     *
     * @param attribute The attribute to check for.
     * @return true if the attribute exists, false otherwise.
     */
    bool has_attribute(const string &attribute)
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
};

void ztranslation(Dataset& dataset)
{
    auto attributes = dataset.get_attributes();
    for (auto &i : attributes)
    {
        if(holds_alternative<double>(dataset[i].front()))
        {
            double sum = 0.0;
            for (auto &j : dataset[i])
            {
                sum += get<double>(j);
            }
            
            double mean = sum / dataset[i].size();
            cout << mean << '\n';
            sum = 0.0;

            for (auto &j : dataset[i])
            {
                sum += pow(get<double>(j) - mean, 2);
            }

            double omega = sqrt(sum / (dataset[i].size()));
            cout << omega << '\n';
            for (auto &j : dataset[i])
            {
                get<double>(j) = (get<double>(j) - mean) / omega;
            }
//          check
            sum = 0.0;
            for (auto &j : dataset[i])
            {
                sum += get<double>(j);
            }
            mean = sum / dataset[i].size();

            cout << "mean  " << mean << endl;
            
            sum = 0.0;

            for (auto &j : dataset[i])
            {
                sum += pow(get<double>(j) - mean, 2);
            }
            omega = sqrt(sum/dataset[i].size());

            cout << "omega " << omega << endl;

            for (auto &&j : dataset[i])
            {
                cout << get<double>(j) << ", ";
            }
            
            exit(0);
        }
    }    
}

int main()
{
    Dataset d = Dataset::read_csv("./iris.csv");

    ztranslation(d);
}
