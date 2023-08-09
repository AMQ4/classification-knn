#ifndef H_DATASET
#define H_DATASET
/**
 * @file dataset.cpp
 * @brief Implementation of the Dataset class for managing and manipulating datasets.
 *
 * This file contains the implementation of the `Dataset` class, which represents a dataset with attributes and values.
 * It provides methods for reading data from CSV files, normalizing and renormalizing data, printing, and visualization using `gnuplot`.
 * The class allows manipulation of attribute values and provides convenient utilities for data analysis and visualization.
 *
 * @author Ahmad Mahmoud Al-qaisi
 */

#include <gnuplot-iostream.h>
#include "prettytable.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <variant>
#include <vector>
#include <unordered_map>
#include <regex>
#include <algorithm>
#include <numeric>

using namespace std;

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
        void (*normalizarion_function)(Dataset *)= [](Dataset *self)
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
        void (*renormalize_function)(Dataset *, vector<DataType> &)= [](Dataset *self, vector<DataType> &data_point)
    {
            for (size_t i = 0; i < data_point.size(); i++)
            {
                if (self->_is_numeric[i])
                {
                    data_point[i] = (get<double>(data_point[i]) - get<double>(self->local_parms[self->keys[i] + " nmin"])) 
                    / 
                    (get<double>(self->local_parms[self->keys[i] + " nmax"]) - get<double>(self->local_parms[self->keys[i] + " nmin"]));
                }
            } });
    /**
     * @brief Construct a Dataset object by reading data from a CSV file.
     *
     * @param path The path to the CSV file containing the dataset's attribute values.
     */
    Dataset(const string &path);
    ~Dataset();
    /**
     * @brief Reads a dataset from a CSV file.
     *
     * @param path The path to the CSV file.
     * @return The populated Dataset object.
     */
    static Dataset read_csv(const string &path);
    /**
     * @brief Normalizes the dataset values using the specified normalization function.
     *
     */
    void normalize();
    /**
     * @brief Prints the dataset.
     */
    void print();
    /**
     * @brief Creates a scatter plot using Gnuplot.
     *
     * @param x The attribute for the x-axis.
     * @param y The attribute for the y-axis.
     * @param [target] A target point to highlight on the plot (optional).
     */
    void scatter_plot(const string &x, const string &y, tuple<double, double> target = {__DBL_MAX__, __DBL_MAX__});
    /**
     * @brief Sets the label attribute for the dataset.
     *
     * @param label The label attribute name.
     */
    void set_label(const string &label);
    /**
     * @brief Retrieves the current label attribute.
     *
     * @return The label attribute.
     */
    string get_label() const;
    /**
     * @brief Retrieves a vector of attribute names present in the dataset.
     * @return A vector of strings containing the attribute names.
     */
    vector<string> get_attributes() const;
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
    vector<DataType> &operator[](const string &attribute);
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
    void set_normalize_function(void (*normalization_function)(Dataset *));
    /**
     * @brief Retrieves the names of numeric attributes in the dataset.
     *
     * @return A vector of strings containing the names of the numeric attributes.
     */
    vector<string> get_numerics() const;

    /**
     * @brief Applies renormalization to a data point using the specified renormalization function.
     *
     * @param data_point The data point to be renormalized.
     */
    void renormalize(vector<DataType> &data_point);

    /**
     * @brief Sets the renormalization function for the dataset.
     *
     * This method allows you to set a custom renormalization function for the dataset.
     *
     * @param renormalize_function A pointer to a renormalization function that takes a Dataset pointer and a vector of DataType.
     */
    void set_renormalize(void (*renormalize_function)(Dataset *, vector<DataType> &));

    /**
     * @brief Retrieves a data point at a specific index by value.
     *
     * @param at The index of the data point to retrieve.
     * @return A vector containing the values of the data point.
     */
    vector<DataType> iterrow(int at);

    /**
     * @brief Retrieves a data point at a specific index by reference.
     *
     * @param at The index of the data point to retrieve.
     * @return A vector of reference wrappers to the values of the data point.
     */
    vector<reference_wrapper<DataType>> iterrow_ref(int at);

    /**
     * @brief Retrieves the number of rows in the dataset.
     *
     * @return The number of rows.
     */
    int no_rows() const;
    /**
     * @brief Add a new data point to the dataset.
     *
     * This function adds a new data point to the dataset. The provided vector contains attribute values for the new data point,
     * and each value is added to the corresponding attribute's vector.
     *
     * @param row A vector containing attribute values for the new data point.
     *
     * @note The size of the provided vector should match the number of attributes in the dataset. If the sizes do not match,
     * the behavior is undefined.
     */
    void push_back(const vector<DataType> &row);

    /**
     * @brief Remove a data point from the dataset at the specified index.
     *
     * This function removes a data point from the dataset at the specified index. The data point is removed
     * by swapping its values with the values of the last data point and then removing the last data point.
     *
     * @param at The index of the data point to be removed.
     *
     * @note The index should be within the valid range [0, _size - 1]. If the index is outside this range,
     * a range_error is thrown.
     */
    void remove(int at);

    /**
     * @brief Split the dataset into training and testing datasets based on a given ratio.
     *
     * This function divides the dataset into two subsets: a training dataset and a testing dataset. The ratio parameter
     * determines the portion of data assigned to the training dataset, while the remaining data is assigned to the testing dataset.
     *
     * @param train The training dataset where a portion of the data will be stored.
     * @param test The testing dataset where the remaining portion of the data will be stored.
     * @param ratio The ratio of data assigned to the training dataset (default is 0.75).
     *
     * @note The ratio should be in the range [0, 1]. If the ratio is outside this range, a range_error is thrown.
     */
    void split(Dataset &train, Dataset &test, double ratio = 0.75);
    /**
     * @brief Construct a CSV representation of the dataset.
     *
     * This function constructs a CSV (Comma-Separated Values) representation of the dataset and saves it to a file specified by the provided path.
     * The CSV file will have the attribute names as the header row and the corresponding attribute values for each data point.
     *
     * @param path The path to the CSV file where the dataset's CSV representation will be saved.
     */
    void to_csv(const string &path);

    bool is_normalized(const vector<DataType>& data_point);

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
    bool has_attribute(const string &attribute);
};

/**
 * @brief Checks if a given string represents a numeric value using regular expressions.
 *
 * @param str The string to check for numeric representation.
 * @return true if the string is numeric, false otherwise.
 */
bool is_numeric(const string &str);

/**
 * @brief Overloaded stream insertion operator for Dataset::DataType values.
 *
 * This operator allows printing values of Dataset::DataType to an output stream.
 */
ostream &operator<<(ostream &out, const Dataset::DataType &a);

bool operator==(const Dataset::DataType &a, const Dataset::DataType &b);

bool operator!=(const Dataset::DataType &a, const Dataset::DataType &b);
#endif
