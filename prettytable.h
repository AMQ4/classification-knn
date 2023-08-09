#ifndef H_PRETTYTABLE
#define H_PRETTYTABLE

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

class PrettyTable
{
public:
    PrettyTable(const vector<string> &headers);

    void add_row(const vector<string> &row);

    void display() const;

private:
    vector<string> headers;
    vector<vector<string>> data;
};

#endif //!H_PRETTYTABLE
