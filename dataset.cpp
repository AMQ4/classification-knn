/**
 * @file dataset.cpp
 * @brief Implementation of the Dataset class for managing and manipulating datasets.
 *
 * This file contains the implementation of the `Dataset` class, which represents a dataset with attributes and values.
 * It provides methods for reading data from CSV files, normalizing and renormalizing data, printing, and visualization using `gnuplot`.
 * The class allows manipulation of attribute values and provides convenient utilities for data analysis and visualization.
 *
 * @author Ahmad Mahmoud Al-qaisi
 * @date August 6, 2023
 */

#include <gnuplot-iostream.h>
#include <iostream>
#include <fstream>
#include <cctype>
#include <variant>
#include <vector>
#include <unordered_map>
#include <regex>
#include <algorithm>

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
    unordered_map<DataType, DataType> local_parms;
    /**
     * @brief Constructs a Dataset object file with optional normalization and renormalization functions.
     * @param [normalization_function] A pointer to a normalization function that takes a Dataset pointer and
     * applies normalization to the dataset's numeric attributes. If not provided, the default range transformation
     * normalization function, with min and max = 0, 1 respectively, will be used.
     * @param [renormalize_function] A pointer to a renormalization function that takes a Dataset pointer and a
     * vector of DataType (data point) and applies renormalization using the previously computed min and max values.
     * If not provided, a default renormalization function is used.
     */
    Dataset(
        void (*normalizarion_function)(Dataset *) = [](Dataset *self)
        {
                for (size_t i = 0; i < self->keys.size(); i++)
                {
                    if (self->_is_numeric[i])
                    {
                        double min = get<double>(*min_element(self->m[self->keys[i]].begin(), self->m[self->keys[i]].end(), 
                        [](const Dataset::DataType &a, const Dataset::DataType &b)
                        { 
                            return get<double>(a) < get<double>(b); 
                        }));
                        double max = get<double>(*max_element(self->m[self->keys[i]].begin(), self->m[self->keys[i]].end(), [](const Dataset::DataType& a, const Dataset::DataType& b){
                            return get<double>(a) < get<double>(b);
                        }));

                        self->local_parms.insert(make_pair(DataType(self->keys[i]+" nmax"), DataType(max)));
                        self->local_parms.insert(make_pair(DataType(self->keys[i]+" nmin"), DataType(min)));

                        for (auto &j : self->m[self->keys[i]])
                        {
                            j = (get<double>(j) - min) / (max - min);
                        }
                    }
                } },
        void (*renormalize_function)(Dataset *, vector<DataType> &) = [](Dataset *self, vector<DataType> &data_point)
        {
            for (size_t i = 0; i < data_point.size(); i++)
            {
                if (self->_is_numeric[i])
                {
                    data_point[i] = (get<double>(data_point[i]) - get<double>(self->local_parms[self->keys[i] + " nmin"])) 
                    / 
                    (get<double>(self->local_parms[self->keys[i] + " nmax"]) - get<double>(self->local_parms[self->keys[i] + " nmin"]));
                }
            } }) : _normalize{normalizarion_function}, _re_normalize{renormalize_function}
    {
    }
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
    /**
     * @brief Normalizes the dataset values using the specified normalization function.
     *
     */
    void normalize()
    {
        _normalize(this);
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
    /**
     * @brief Retrieves a vector of attribute names present in the dataset.
     * @return A vector of strings containing the attribute names.
     */
    vector<string> get_attributes() const
    {
        return keys;
    }
    /**
     * @brief Retrieves the values of a specific attribute in the dataset.
     *
     * This method allows you to access the values of a specific attribute in the dataset using
     * the attribute name as an index. It returns a vector containing the values of the specified
     * attribute.
     *
     * @param attribute The name of the attribute whose values you want to retrieve.
     * @return A vector containing the values of the specified attribute.
     *
     * @note If the attribute does not exist in the dataset, an error message will be printed and an
     * empty vector will be returned.
     *
     */
    vector<DataType> &operator[](const string &attribute)
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
    /**
     * @brief Sets the normalization function for the dataset.
     *
     * This method allows you to set a custom normalization function for the dataset. The provided
     * normalization function will be used to normalize the numeric attributes of the dataset.
     *
     * @param normalization_function A pointer to a normalization function that takes a Dataset pointer.
     *
     * @note The normalization function should take a Dataset pointer as a parameter and apply normalization
     * to the dataset's attributes as needed.
     *
     * Example Usage:
     * @code
     * Dataset dataset("data.csv");
     * void customNormalization(Dataset *self) {
     *     // Implement your custom normalization logic here
     * }
     * dataset.set_normalize_function(customNormalization);
     * dataset.normalize();
     * @endcode
     */
    void set_normalize_function(void (*normalization_function)(Dataset *))
    {
        _normalize = normalization_function;
    }
    /**
     * @brief Retrieves the names of numeric attributes in the dataset.
     *
     * @return A vector of strings containing the names of the numeric attributes.
     */
    vector<string> get_numerics() const
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

    /**
     * @brief Applies renormalization to a data point using the specified renormalization function.
     *
     * @param data_point The data point to be renormalized.
     */
    void renormalize(vector<DataType> &data_point)
    {
        _re_normalize(this, data_point);
    }

    /**
     * @brief Sets the renormalization function for the dataset.
     *
     * This method allows you to set a custom renormalization function for the dataset.
     *
     * @param renormalize_function A pointer to a renormalization function that takes a Dataset pointer and a vector of DataType.
     */
    void set_renormalize(void (*renormalize_function)(Dataset *, vector<DataType> &))
    {
        _re_normalize = renormalize_function;
    }

    // Data Iteration

    /**
     * @brief Retrieves a data point at a specific index by value.
     *
     * @param at The index of the data point to retrieve.
     * @return A vector containing the values of the data point.
     */
    vector<DataType> iterrow(int at)
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

    /**
     * @brief Retrieves a data point at a specific index by reference.
     *
     * @param at The index of the data point to retrieve.
     * @return A vector of reference wrappers to the values of the data point.
     */
    vector<reference_wrapper<DataType>> iterrow_ref(int at)
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

    /**
     * @brief Retrieves the number of rows in the dataset.
     *
     * @return The number of rows.
     */
    int no_rows() const
    {
        return _size;
    }

private:
    unordered_map<string, vector<DataType>> m;            /**< Map storing attribute values for the dataset. */
    vector<string> keys;                                  /**< Vector containing the names of attributes. */
    string label;                                         /**< The label attribute for the dataset. */
    vector<bool> _is_numeric;                             /**< Vector indicating whether each attribute is numeric. */
    int _size;                                            /**< The number of rows in the dataset. */
    void (*_normalize)(Dataset *);                        /**< Pointer to the normalization function. */
    void (*_re_normalize)(Dataset *, vector<DataType> &); /**< Pointer to the renormalization function. */

    /**
     * @brief Checks if the dataset has a specific attribute.
     *
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

/**
 * @brief Overloaded stream insertion operator for Dataset::DataType values.
 *
 * This operator allows printing values of Dataset::DataType to an output stream.
 */
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
    return not (a == b);
}
