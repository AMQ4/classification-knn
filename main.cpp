#include "KNN.cpp"

using namespace std;
using dataType = Dataset::DataType;
int main()
{
KNN knn("./data/iris.csv", "species", 1);

    //vector<Dataset::DataType> x = {4.8,3.0,1.4,0.1,"Iris-setosa"};
    auto x = knn.get_dataset().iterrow(0);
    for (auto &&i : x)
    {
        cout << i << ", ";
    }
cout << endl;    
    cout << knn.predict(x);
}
