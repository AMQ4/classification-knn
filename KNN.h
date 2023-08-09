#ifndef H_KNN
#define H_KK
/**
 * @file KNN.cpp
 * @brief Implementation of the KNN (K-Nearest Neighbors) classifier derived from the Classifier base class.
 *
 * This file contains the implementation of the `KNN` class, which is a derived class from the `Classifier` base class.
 * It implements the K-Nearest Neighbors classification algorithm and provides methods for prediction and evaluation.
 * The class utilizes the `Dataset` class to manage and manipulate data and defines its own proximity measure function.
 *
 * @author Ahmad Mahmoud Al-qaisi
 */

#include "classifire.h"
#include <iostream>
#include <numeric>

using namespace std;

double euclidean_distance_mesure(Dataset *, const vector<Dataset::DataType> &, const vector<Dataset::DataType> &);
/**
 * @brief A class representing the K-Nearest Neighbors (KNN) classifier.
 *
 * This class inherits from the Classifier base class and provides implementations
 * for the KNN classifier's prediction and evaluation methods.
 */
class KNN : public Classifier
{
public:
    /**
     * @brief Predicts the class label for a given sample using KNN.
     *
     * @param sample The input sample for which to predict the class label.
     * @return The predicted class label.
     */
    Dataset::DataType predict(const vector<Dataset::DataType> &sample) override;
    /**
     * @brief Evaluate the classifier's performance on a test dataset, return the confusion matrix,
     * and print a classification report including micro-accuracy, micro-recall, and micro-precision.
     *
     * @param testData The dataset used for evaluation.
     * @return A confusion matrix containing counts of actual and predicted labels for each class.
     */
    unordered_map<Dataset::DataType, unordered_map<Dataset::DataType, int>> evaluate(Dataset &testData) override;

    /**
     * @brief Constructs a KNN classifier instance.
     *
     * @param path The path to the dataset.
     * @param label The label for classification.
     * @param k The number of nearest neighbors to consider (default is 1).
     * @param proximity_measure The proximity measure function (default is Euclidean distance).
     */
    KNN(
        const string &path, const string &label, int k=1,
        double (*proximity_measure)(Dataset *, const vector<Dataset::DataType> &, const vector<Dataset::DataType> &) = euclidean_distance_mesure);
    /**
     * @brief Construct a KNN classifier using the specified training dataset and parameters.
     *
     * This constructor initializes a KNN classifier using the provided training dataset and specified parameters.
     * The KNN classifier uses a proximity measure to determine the similarity between data points.
     *
     * @param train_dataset The training dataset used to train the KNN classifier.
     * @param k The number of nearest neighbors to consider (default is 1).
     * @param proximity_measure A pointer to the proximity measure function used to calculate distances between data points
     * (default is euclidean_distance_mesure).
     *
     * @note The proximity_measure function should take a Dataset pointer, two vectors of Dataset::DataType values representing
     * two data points, and return a distance measure.
     */
    KNN(const Dataset &train_dataset, int k = 1, double (*proximity_measure)(Dataset *, const vector<Dataset::DataType> &, const vector<Dataset::DataType> &) = euclidean_distance_mesure);

    /**
     * @brief Set the proximity measure function for the KNN classifier.
     *
     * This function sets the proximity measure function used by the KNN classifier to calculate distances between data points.
     * The proximity measure function should take a Dataset pointer and two vectors of Dataset::DataType values representing
     * two data points, and it should return a distance measure.
     *
     * @param proximity_measure A pointer to the proximity measure function.
     */
    void set_proximity_measure(double (*proximity_measure)(Dataset *, const vector<Dataset::DataType> &, const vector<Dataset::DataType> &));
    /**
     * @brief Perform k-nearest neighbor search using the first k nearest neighbors.
     *
     * This function finds the k nearest neighbors based on the specified proximity measure.
     * The comparison function determines the sorting order for nearest neighbors.
     * For distance-based measures, use the default comparison. For similarity, adjust the comparison accordingly.
     *
     * @param target The target data point for neighbor search.
     * @param comparison_fn A comparison function for sorting neighbors.
     * @return Vector of pairs: proximity measure(distance or similarity) and data point index.
     */
    vector<pair<double, int>> first_knn(
        const vector<Dataset::DataType> &target, bool (*comparison_fn)(double, double) = [](double a, double b)
                                             { return a <= b; });

    /**
     * @brief Sets the dataset for the KNN classifier.
     *
     * @param path The path to the dataset.
     */
    void set_dataset(const string &path);
    /**
     * @brief Get the Dataset object.
     *
     * This function returns the Dataset object associated with the class instance.
     *
     * @return A reference to the Dataset object.
     */
    Dataset &get_dataset();

private:
    unsigned int _k;                                                                                               /**< The number of nearest neighbors to consider. */
    double (*_proximity_measure)(Dataset *, const vector<Dataset::DataType> &, const vector<Dataset::DataType> &); /**< The proximity measure function. */
    Dataset dataset;                                                                                               /**< The dataset used for classification. */
    /**
     * @brief Train the classifier using the provided training data.
     *
     * @param trainingData The dataset used for training.
     */
    void train(const Dataset &trainingData) override;
    /**
     * @brief Save the trained model to a file.
     *
     * @param filePath The path to the file where the model will be saved.
     */
    void saveModel(const std::string &filePath) override;
    /**
     * @brief Load a previously trained model from a file.
     *
     * @param filePath The path to the file containing the saved model.
     */
    void loadModel(const std::string &filePath) override;
};

#endif
