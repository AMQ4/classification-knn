/**
 * @file KNN.cpp
 * @brief Implementation of the KNN (K-Nearest Neighbors) classifier derived from the Classifier base class.
 *
 * This file contains the implementation of the `KNN` class, which is a derived class from the `Classifier` base class.
 * It implements the K-Nearest Neighbors classification algorithm and provides methods for prediction and evaluation.
 * The class utilizes the `Dataset` class to manage and manipulate data and defines its own proximity measure function.
 * 
 * @author Ahmad Mahmoud Al-qaisi
 * @date August 6, 2023
 */

#include "classifire.cpp"

using namespace std;

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
    Dataset::DataType predict(const vector<Dataset::DataType> &sample);

    /**
     * @brief Evaluates the performance of the KNN classifier on a test dataset.
     * 
     * @param testData The test dataset on which to evaluate the classifier.
     * @return The evaluation result.
     */
    double evaluate(const Dataset &testData);

    /**
     * @brief Constructs a KNN classifier instance.
     * 
     * @param path The path to the dataset.
     * @param label The label for classification.
     * @param k The number of nearest neighbors to consider (default is 1).
     * @param proximity_measure The proximity measure function (default is Euclidean distance).
     */
    KNN(
        const string &path, const string &label, int k = 1,
        double (*proximity_measure)(const vector<Dataset::DataType> &, const vector<Dataset::DataType> &) = [](const vector<Dataset::DataType> &a, const vector<Dataset::DataType> &b) -> double
        {
            // Implementation of the default proximity measure function
        });

    /**
     * @brief Finds the first k nearest neighbors of a target using the provided comparison function.
     * 
     * @param target The target data for which to find nearest neighbors.
     * @param comparison_fn The comparison function to determine proximity (default is less-than comparison).
     * @return A vector of indices corresponding to the first k nearest neighbors.
     */
    vector<int> first_knn(
        const vector<Dataset::DataType> &target, bool (*comparison_fn)(double, double) = [](double a, double b)
        { return a < b; });

    /**
     * @brief Sets the dataset for the KNN classifier.
     * 
     * @param path The path to the dataset.
     */
    void set_dataset(const string &path);

private:
    unsigned int _k; /**< The number of nearest neighbors to consider. */
    double (*_proximity_measure)(const vector<Dataset::DataType> &, const vector<Dataset::DataType> &); /**< The proximity measure function. */
    Dataset dataset; /**< The dataset used for classification. */

    /**
     * @brief The train function (override) for training the classifier.
     * 
     * @param trainingData The training dataset.
     */
    void train(const Dataset &trainingData);

    /**
     * @brief The saveModel function (override) for saving the classifier model.
     * 
     * @param filePath The file path to save the model.
     */
    void saveModel(const std::string &filePath);

    /**
     * @brief The loadModel function (override) for loading the classifier model.
     * 
     * @param filePath The file path to load the model from.
     */
    void loadModel(const std::string &filePath);
};
