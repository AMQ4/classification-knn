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
#include <iostream>
#include <numeric>

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
    Dataset::DataType predict(const vector<Dataset::DataType> &sample) override
    {
        auto _k_nn = first_knn(sample);
        auto keys = dataset.get_attributes();

        unordered_map<Dataset::DataType, double> weights;
        int l = 0;

        for (size_t i = 0; i < keys.size(); i++)
        {
            if (dataset.get_label() == keys[i])
            {
                l = i;
                break;
            }
        }

        double sum = 0.0;

        for (auto &&i : _k_nn)
        {
            sum += exp(-i.first);
        }

        for (auto &&i : _k_nn)
        {
            weights[dataset.iterrow(i.second)[l]] += exp(-i.first) / sum;
        }

        return (*max_element(weights.begin(), weights.end(), [](const pair<Dataset::DataType, double> &a, const pair<Dataset::DataType, double> &b)
                             { return a.second > b.second; }))
            .first;
    }
    /**
     * @brief Evaluate the classifier's performance on a test dataset, return the confusion matrix,
     * and print a classification report including micro-accuracy, micro-recall, and micro-precision.
     *
     * @param testData The dataset used for evaluation.
     * @return A confusion matrix containing counts of actual and predicted labels for each class.
     */
    unordered_map<Dataset::DataType, unordered_map<Dataset::DataType, int>> evaluate(Dataset &testData) override
    {
        unordered_map<Dataset::DataType, unordered_map<Dataset::DataType, int>> confusion_matrix;

        int l = 0;
        for (size_t i = 0; i < dataset.get_attributes().size(); i++)
        {
            if (dataset.get_attributes()[i] == dataset.get_label())
            {
                l = i;
                break;
            }
        }

        for (size_t i = 0; i < testData.no_rows(); i++)
        {
            auto _actual = testData.iterrow(i)[l];
            auto _predicted = predict(testData.iterrow(i));
            ++confusion_matrix[_actual][_predicted];
        }
        unordered_map<string, vector<double>> confusion_matrix_elements;

        double TN = 0.0;
        for (auto &&i : confusion_matrix)
        {
            for (auto &&j : i.second)
            {
                if (i.first == j.first)
                {
                    confusion_matrix_elements["TP"].push_back(j.second);
                    TN += j.second;
                }
                else
                {
                    confusion_matrix_elements["FN"].push_back(j.second);
                    confusion_matrix_elements["FP"].push_back(j.second);
                }
                TN += j.second;
            }
        }

        double TP = accumulate(confusion_matrix_elements["TP"].begin(), confusion_matrix_elements["TP"].end(), 0.0),
               FN = accumulate(confusion_matrix_elements["FN"].begin(), confusion_matrix_elements["FN"].end(), 0.0),
               FP = accumulate(confusion_matrix_elements["FP"].begin(), confusion_matrix_elements["FP"].end(), 0.0);

        cout << "\nModel Micro-Precision : " << (int)round((TP / (TP + FP)) * 100) << "%"
             << "\nModel Micro-Recall    : " << (int)round((TP / (TP + FN)) * 100) << "%"
             << "\nModel Micro-Accuracy  : " << (int)round(((TP + TN) / (TP + TN + FP + FN)) * 100) << "%\n"
             << endl;

        return confusion_matrix;
    }

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
        double (*proximity_measure)(Dataset *, const vector<Dataset::DataType> &, const vector<Dataset::DataType> &) = [](Dataset *self, const vector<Dataset::DataType> &a, const vector<Dataset::DataType> &b)
        {
                double sum = 0.0;
                auto keys = self->get_attributes();
                
                for (size_t i = 0; i < keys.size(); i++)
                {
                    if(keys[i] != self->get_label())
                    {
                    if(holds_alternative<double>(a[i]))
                    {
                        // numerical
                        sum += pow(get<double>(a[i]) - get<double>(b[i]), 2);
                    }
                    else
                    {
                        // categorical
                        sum += (int)(get<string>(a[i]) == get<string>(b[i]));
                    }
                    }
                }
                
                return sqrt(sum); })
    {
        _proximity_measure = proximity_measure;
        _k = k;
        dataset = Dataset::read_csv(path);
        dataset.normalize();
        dataset.set_label(label);
    }
    /**
     * @brief Finds the first k nearest neighbors of a target using the provided comparison function.
     *
     * @param target The target data for which to find nearest neighbors.
     * @param comparison_fn The comparison function to determine proximity (default is less-than comparison).
     * @return A vector of indices, distances pair corresponding to the first k nearest neighbors.
     */
    vector<pair<double, int>> first_knn(
        const vector<Dataset::DataType> &target, bool (*comparison_fn)(double, double) = [](double a, double b)
                                                 { return a <= b; })
    {
        if (dataset.get_label().length() == 0)
        {
            cerr << " label unset, an empty vector returned.\n";
            return {};
        }

        auto _target = target;
        dataset.renormalize(_target);

        vector<pair<double, int>> proxi_measure_res;

        for (size_t i = 0; i < dataset.no_rows(); i++)
        {
            if (i == 147)
            {
                cout << "";
            }
            auto _ = dataset.iterrow(i);
            proxi_measure_res.push_back(make_pair(_proximity_measure(&dataset, _, _target), i));
        }

        partial_sort(proxi_measure_res.begin(), proxi_measure_res.begin() + _k, proxi_measure_res.end(), [&](const pair<double, int> &a, const pair<double, int> &b)
                     { return comparison_fn(a.first, b.first); });

        return vector<pair<double, int>>(proxi_measure_res.begin(), proxi_measure_res.begin() + _k);
    }

    /**
     * @brief Sets the dataset for the KNN classifier.
     *
     * @param path The path to the dataset.
     */
    void set_dataset(const string &path)
    {
        dataset = Dataset::read_csv(path);
    }
    /**
     * @brief Get the Dataset object.
     *
     * This function returns the Dataset object associated with the class instance.
     *
     * @return A reference to the Dataset object.
     */
    Dataset &get_dataset()
    {
        return dataset;
    }

private:
    unsigned int _k;                                                                                               /**< The number of nearest neighbors to consider. */
    double (*_proximity_measure)(Dataset *, const vector<Dataset::DataType> &, const vector<Dataset::DataType> &); /**< The proximity measure function. */
    Dataset dataset;                                                                                               /**< The dataset used for classification. */
    /**
     * @brief Train the classifier using the provided training data.
     *
     * @param trainingData The dataset used for training.
     */
    void train(const Dataset &trainingData) override
    {
    }
    /**
     * @brief Save the trained model to a file.
     *
     * @param filePath The path to the file where the model will be saved.
     */
    void saveModel(const std::string &filePath) override
    {
    }
    /**
     * @brief Load a previously trained model from a file.
     *
     * @param filePath The path to the file containing the saved model.
     */
    void loadModel(const std::string &filePath) override
    {
    }
};
