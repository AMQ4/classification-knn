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

#include <fstream>
#include <variant>
#include <vector>
#include <unordered_map>

using namespace std;

/**
 * @brief Checks if a given string represents a numeric value using regular expressions.
 *
 * @param str The string to check for numeric representation.
 * @return true if the string is numeric, false otherwise.
 */
bool is_numeric(const string &str);

/**
 * @brief Represents a dataset with attributes and values, and provides visualization methods.
 */
class Dataset
{
public:
    using DataType = variant<double, string>;
    unordered_map<DataType, DataType> local_parms; /**< A map to store local parameters related to normalization and renormalization. */
    
    // Constructors and Destructor
    
    /**
     * @brief Reads a dataset from a CSV file.
     *
     * @param path The path to the CSV file.
     * @return The populated Dataset object.
     */
    static Dataset read_csv(string path);
    
    // Data Manipulation Methods
    
    /**
     * @brief Normalizes the dataset values using the specified normalization function.
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
    
    // Operator Overloading
    
    /**
     * @brief Retrieves the values of a specific attribute in the dataset.
     *
     * This method allows you to access the values of a specific attribute in the dataset using
     * the attribute name as an index. It returns a vector containing the values of the specified
     * attribute.
     *
     * @param attribute The name of the attribute whose values you want to retrieve.
     * @return A vector containing the values of the specified attribute.
     */
    vector<DataType> &operator[](const string &attribute);
    
    // Normalization and Renormalization
    
    /**
     * @brief Sets the normalization function for the dataset.
     *
     * This method allows you to set a custom normalization function for the dataset.
     *
     * @param normalization_function A pointer to a normalization function that takes a Dataset pointer.
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
    
    // Data Iteration
    
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
    
private:
    unordered_map<string, vector<DataType>> m; /**< Map storing attribute values for the dataset. */
    vector<string> keys; /**< Vector containing the names of attributes. */
    string label; /**< The label attribute for the dataset. */
    vector<bool> _is_numeric; /**< Vector indicating whether each attribute is numeric. */
    int _size; /**< The number of rows in the dataset. */
    void (*_normalize)(Dataset *); /**< Pointer to the normalization function. */
    void (*_re_normalize)(Dataset *, vector<DataType> &); /**< Pointer to the renormalization function. */
    /**
     * @brief Checks if the dataset has a specific attribute.
     *
     * @return true if the attribute exists, false otherwise.
     */
    bool has_attribute(const string &attribute);
};
