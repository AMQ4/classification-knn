#include <iostream>
#include <vector>
#include "classifire.cpp"

using namespace std;

class KNN : public Classifier
{
public:
    Dataset::DataType predict(const vector<Dataset::DataType> &sample) override
    {
        Dataset::DataType label;
        return label;
    }

    double evaluate(const Dataset &testData) override
    {
        return 0.0;
    }

    template <class proximity_measure>
    KNN(const string& path, int k = 1, proximity_measure pm = nullptr)
    {
        _proximity_measure = pm;
        _k = k;
        dataset = Dataset::read_csv(path);
        dataset.normalize();
    }

private:
    unsigned int _k;
    double (*_proximity_measure)(const vector<Dataset::DataType>&,const vector<Dataset::DataType>&);
    Dataset dataset;

    void train(const Dataset &trainingData) override
    {
    }
    void saveModel(const std::string &filePath) override
    {
    }
    void loadModel(const std::string &filePath) override
    {
    }
};

int main()
{
    KNN knn("./iris.csv", 10, [](const vector<Dataset::DataType>& a,const vector<Dataset::DataType>& b) -> double
            {
                double distance = 0.0;
                double sum = 0.0;
                for (size_t i = 0; i < a.size(); i++)
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

                return distance;

            });
    Dataset dataset;
    knn.predict(vector<Dataset::DataType>{1.0, 2.0, "ali"});
}
