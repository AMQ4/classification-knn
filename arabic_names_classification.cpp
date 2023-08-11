#include <bits/stdc++.h>
#include "dataset.h"
#include "KNN.h"

#define all(a) a.begin(), a.end()

int main()
{
    Dataset arabic_names = Dataset::read_csv("./data/arabic_names.csv"), train, test;
    arabic_names.set_label("gender");
    arabic_names.split(train, test); 

    // set Jaccard Similarity
    KNN knn(train, 5);
    vector<Dataset::DataType> x;

    knn.set_proximity_measure([](Dataset *, const vector<Dataset::DataType> &a, const vector<Dataset::DataType> &b)
                              {
                                multiset<char> _a (all(get<string>(a[0]))), _b (all(get<string>(b[0])));
                                vector<char> r;

                                set_intersection(all(_a), all(_b), back_inserter(r));
    
                                return  1-(1.0 * r.size() / 
                                       (_a.size() + _b.size() - r.size())); 
                              });
                              
    knn.evaluate(test);   

    string name;
    cout << "Try with your self : ";
    cin >> name;

    x.push_back(name);

    cout << name << " it seems to be a " << knn.predict(x) << " name!" << endl;
}
