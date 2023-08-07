#include <algorithm>
#include <iostream>

using namespace std;

int main()
{
    vector<int> x{6,2,8,3,9,0,1,2,45,7,3,0};
    for (auto &&i : x)
    {
        cout << i <<" ,"; 
    }
    cout << endl<<endl;
    //make_heap(x.begin(), x.end(), greater<int>());
    partial_sort(x.begin(), x.begin() + 3, x.end(), greater<int>());

    for (auto &&i : x)
    {
        cout << i <<" ,"; 
    }
    
}