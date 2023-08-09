#include "KNN.h"

double euclidean_distance_mesure(Dataset *self, const vector<Dataset::DataType> &_a, const vector<Dataset::DataType> &_b)
{
    auto a = _a, b = _b;

    if (a.size() < b.size())
    {
        swap(a, b);
    }

    double sum = 0.0;
    auto keys = self->get_attributes();
    int l = 0;

    for (size_t i = 0; i < keys.size(); i++)
    {
        if (keys[i] == self->get_label())
        {
            l = i;
            break;
        }
    }

    for (size_t i = 0; i < b.size(); i++)
    {
        if (holds_alternative<double>(b[i]))
        {
            // numerical
            sum += pow(get<double>(a[(i >= l and a.size() != b.size()) ? i + 1 : i]) - get<double>(b[i]), 2);
        }
        else
        {
            // categorical
            sum += (int)(get<string>(a[(i >= l and a.size() != b.size()) ? i + 1 : i]) == get<string>(b[i]));
        }
    }

    return sqrt(sum);
};

KNN::KNN(
    const string &path, const string &label, int k,
    double (*proximity_measure)(Dataset *, const vector<Dataset::DataType> &, const vector<Dataset::DataType> &))
{
    _proximity_measure = proximity_measure;
    _k = k;
    dataset = Dataset::read_csv(path);
    dataset.normalize();
    dataset.set_label(label);
}

Dataset::DataType KNN::predict(const vector<Dataset::DataType> &sample)
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

unordered_map<Dataset::DataType, unordered_map<Dataset::DataType, int>> KNN::evaluate(Dataset &testData)
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

    std::cout << "\nModel Micro-Precision : " << (int)round((TP / (TP + FP)) * 100) << "%"
         << "\nModel Micro-Recall    : " << (int)round((TP / (TP + FN)) * 100) << "%"
         << "\nModel Micro-Accuracy  : " << (int)round(((TP + TN) / (TP + TN + FP + FN)) * 100) << "%\n"
         << endl;

    return confusion_matrix;
}

KNN::KNN(const Dataset &train_dataset, int k, double (*proximity_measure)(Dataset *, const vector<Dataset::DataType> &, const vector<Dataset::DataType> &))
{
    _proximity_measure = proximity_measure;
    dataset = move(train_dataset);
    _k = k;
    dataset.normalize();
    dataset.set_label(dataset.get_label());
}

void KNN::set_proximity_measure(double (*proximity_measure)(Dataset *, const vector<Dataset::DataType> &, const vector<Dataset::DataType> &))
{
    _proximity_measure = proximity_measure;
}

vector<pair<double, int>> KNN::first_knn(
    const vector<Dataset::DataType> &target, bool (*comparison_fn)(double, double))
{
    if (dataset.get_label().length() == 0)
    {
        cerr << " label unset, an empty vector returned.\n";
        return {};
    }

    auto _target = target;
    if(dataset.is_normalized(_target))
    dataset.renormalize(_target);

    vector<pair<double, int>> proxi_measure_res;

    for (size_t i = 0; i < dataset.no_rows(); i++)
    {
        auto _ = dataset.iterrow(i);
        proxi_measure_res.push_back(make_pair(_proximity_measure(&dataset, _, _target), i));
    }

    partial_sort(proxi_measure_res.begin(), proxi_measure_res.begin() + _k, proxi_measure_res.end(), [&](const pair<double, int> &a, const pair<double, int> &b)
                 { return comparison_fn(a.first, b.first); });

    return vector<pair<double, int>>(proxi_measure_res.begin(), proxi_measure_res.begin() + _k);
}

void KNN::set_dataset(const string &path)
{
    dataset = Dataset::read_csv(path);
}

Dataset &KNN::get_dataset()
{
    return dataset;
}

void KNN::saveModel(const string&)
{
}

void KNN::loadModel(const string&)
{
}

void KNN::train(const Dataset&)
{
}
