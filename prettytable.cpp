#include "prettytable.h"

PrettyTable::PrettyTable(const vector<string> &headers):headers{headers}{}

void PrettyTable::add_row(const vector<string>& row)
{
    data.push_back(row);
}

void PrettyTable::display()const
{
           if (headers.empty() || data.empty())
        {
            cout << "Table is empty." << endl;
            return;
        }

        vector<size_t> columnWidths(headers.size(), 0);

        // Calculate column widths based on headers and data
        for (size_t i = 0; i < headers.size(); ++i)
        {
            columnWidths[i] = headers[i].size();
            for (const auto &row : data)
            {
                if (i < row.size() && row[i].size() > columnWidths[i])
                {
                    columnWidths[i] = row[i].size();
                }
            }
        }

        // Display headers
        for (size_t i = 0; i < headers.size(); ++i)
        {
            cout << setw(columnWidths[i]) << left << headers[i];
            if (i < headers.size() - 1)
            {
                cout << " | ";
            }
        }
        cout << endl;

        // Display separator
        for (size_t i = 0; i < headers.size(); ++i)
        {
            cout << string(columnWidths[i], '-') << " ";
            if (i < headers.size() - 1)
            {
                cout << "+ ";
            }
        }
        cout << endl;

        // Display data rows
        for (const auto &row : data)
        {
            for (size_t i = 0; i < row.size(); ++i)
            {
                cout << setw(columnWidths[i]) << left << row[i];
                if (i < row.size() - 1)
                {
                    cout << " | ";
                }
            }
            cout << endl;
        }
}

