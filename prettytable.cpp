#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

class PrettyTable
{
public:
    PrettyTable(const std::vector<std::string> &headers) : headers(headers) {}

    void add_row(const std::vector<std::string> &row)
    {
        data.push_back(row);
    }

    void display()
    {
        if (headers.empty() || data.empty())
        {
            std::cout << "Table is empty." << std::endl;
            return;
        }

        std::vector<size_t> columnWidths(headers.size(), 0);

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
            std::cout << std::setw(columnWidths[i]) << std::left << headers[i];
            if (i < headers.size() - 1)
            {
                std::cout << " | ";
            }
        }
        std::cout << std::endl;

        // Display separator
        for (size_t i = 0; i < headers.size(); ++i)
        {
            std::cout << std::string(columnWidths[i], '-') << " ";
            if (i < headers.size() - 1)
            {
                std::cout << "+ ";
            }
        }
        std::cout << std::endl;

        // Display data rows
        for (const auto &row : data)
        {
            for (size_t i = 0; i < row.size(); ++i)
            {
                std::cout << std::setw(columnWidths[i]) << std::left << row[i];
                if (i < row.size() - 1)
                {
                    std::cout << " | ";
                }
            }
            std::cout << std::endl;
        }
    }

private:
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> data;
};
