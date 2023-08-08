#include "KNN.cpp"
using namespace std;
using dataType = Dataset::DataType;
int main()
{
    Dataset mum("./data/Breast Cancer Wisconsin.csv"), train, test;
    mum.split(train, test);
    train.set_label("diagnosis");
    
    KNN knn(train, 115);
    knn.evaluate(test);
    
}
