#include "KNN.cpp"
using namespace std;

int main()
{
    KNN knn("./data/iris_training.csv", "species", 1);
    auto testing = Dataset::read_csv("./data/iris_testing.csv");
    knn.evaluate(testing);
    
}