#include "KNN.h"

using namespace std;
using dataType = Dataset::DataType;
int main()
{
    Dataset mommy("./data/iris.csv"), train, test;

    mommy.set_label("species");
    mommy.split(train, test);

    KNN knn(train, 1);

    knn.evaluate(test);
}
