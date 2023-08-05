#include "dataset.cpp"

/**
 * @brief Abstract base class for machine learning classifiers.
 */
class Classifier
{
protected:
    /**
     * @brief Save the trained model to a file.
     *
     * @param filePath The path to the file where the model will be saved.
     */
    virtual void saveModel(const std::string &filePath) = 0;

    /**
     * @brief Load a previously trained model from a file.
     *
     * @param filePath The path to the file containing the saved model.
     */
    virtual void loadModel(const std::string &filePath) = 0;

    /**
     * @brief Train the classifier using the provided training data.
     *
     * @param trainingData The dataset used for training.
     */
    virtual void train(const Dataset &trainingData) = 0;
public:
    /**
     * @brief Predict the class label for a given sample.
     *
     * @param sample The input sample for prediction.
     * @return The predicted class label.
     */
    virtual Dataset::DataType predict(const vector<Dataset::DataType> &sample) = 0;

    /**
     * @brief Evaluate the classifier's performance on a test dataset.
     *
     * @param testData The dataset used for evaluation.
     * @return A performance metric indicating the classifier's accuracy or other measure.
     */
    virtual double evaluate(const Dataset &testData) = 0;
    /**
     * @brief Virtual destructor.
     *
     * Base class destructor should be virtual to ensure proper destruction
     * when objects of derived classes are deleted.
     */
    virtual ~Classifier() {}
};