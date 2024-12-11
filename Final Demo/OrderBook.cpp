#include "OrderBook.h"
#include <sstream>
#include <thread>
#include <algorithm>
#include <unordered_map>

// Functions from DataClear.cpp
struct Row {
    std::vector<std::string> columns; // Use a dynamic list of columns
};

// Function to read CSV and return a vector of Rows
std::vector<Row> readCsv(const std::string& filePath) {
    std::vector<Row> rows;
    std::ifstream file(filePath);
    std::string line;

    if (file.is_open()) {
        // Read each line from the CSV file
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string cell;
            Row row;

            // Parse each cell in the row
            while (std::getline(ss, cell, ',')) {
                row.columns.push_back(cell);
            }

            rows.push_back(row);
        }
        file.close();
    }
    return rows;
}

// Function to write a vector of Rows to a CSV
void writeCsv(const std::string& filePath, const std::vector<Row>& rows) {
    std::ofstream file(filePath);
    if (file.is_open()) {
        for (const auto& row : rows) {
            for (size_t i = 0; i < row.columns.size(); ++i) {
                file << row.columns[i];
                if (i < row.columns.size() - 1) file << ",";
            }
            file << "\n";
        }
        file.close();
    }
}

// Function to remove consecutive 'New' rows
std::vector<Row> removeConsecutiveNewRows(const std::vector<Row>& rows) {
    std::vector<Row> cleanedRows;
    for (size_t i = 0; i < rows.size(); ++i) {
        if (i < rows.size() - 1 &&
            rows[i].columns.size() >= 5 && rows[i + 1].columns.size() >= 5 &&
            rows[i].columns[0] == rows[i + 1].columns[0] &&
            rows[i].columns[1] == rows[i + 1].columns[1] &&
            rows[i].columns[2] == rows[i + 1].columns[2] &&
            rows[i].columns[3] == rows[i + 1].columns[3] &&
            rows[i].columns[4] == "New") {
            continue; // Skip adding this row if it matches the condition
        }
        cleanedRows.push_back(rows[i]);
    }
    return cleanedRows;
}

// Function to remove duplicates only if they have "Fill"
std::vector<Row> removeDuplicates(const std::vector<Row>& rows) {
    std::vector<Row> cleanedRows;
    std::unordered_map<std::string, int> seen;
    for (const auto& row : rows) {
        if (row.columns.size() < 5) {
            cleanedRows.push_back(row); // Skip processing if there are not enough columns
            continue;
        }

        std::string key = row.columns[0] + row.columns[1] + row.columns[2] + row.columns[3] + row.columns[4];
        
        if (seen.find(key) == seen.end()) {
            seen[key] = 1;
            cleanedRows.push_back(row);
        } else {
            if (row.columns[4] == "Fill") {
                continue; // Remove duplicate if it has "Fill" status
            } else {
                cleanedRows.push_back(row); // Keep if not "Fill"
            }
        }
    }
    return cleanedRows;
}

// Existing functions from OrderBook.cpp remain unchanged

void InsertOrderToBook(vector<Order>& order_book, Order& new_order, int side) {
    auto position = order_book.begin();
    while (position != order_book.end()) {
        if ((side == 1 && position->order_price < new_order.order_price) ||
            (side == 2 && position->order_price > new_order.order_price)) {
            break;
        }
        ++position;
    }

    order_book.insert(position, move(new_order));  // Use move semantics here
}

void ProcessOrders(const string& input_file_name) {
    vector<vector<Order>> order_books[5];  // Declare array

    for (int i = 0; i < 5; ++i) {
        order_books[i].push_back(vector<Order>());  // Initialize dynamically
        order_books[i].push_back(vector<Order>());
    }

    const vector<string> instrument_list = {"Rose", "Lavender", "Lotus", "Tulip", "Orchid"};

    ifstream input_file(input_file_name);
    ofstream output_file("execution_report.csv");
    output_file << "Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price,Reason,Transaction Time\n";

    string line;
    int line_count = 0;
    
    while (getline(input_file, line)) {
        if (++line_count == 1) continue;  // Skip header

        stringstream ss(line);
        vector<string> row(5);
        for (int i = 0; i < 5; ++i) {
            getline(ss, row[i], ',');
        }


        /////////////////////////
        Order new_order(row[0], row[1], row[2], row[4], row[3]);

        // Always execute new orders with "New" status before checking matches
        if (new_order.IsNotRejected()) {
            new_order.ExecuteOrder(output_file);  // Log the "New" order

            int book_index = find(begin(instrument_list), end(instrument_list), new_order.instrument_name) - begin(instrument_list);

            if (new_order.order_side == 1) {
                ExecuteOrders(order_books[book_index][1], new_order, output_file);
                InsertOrderToBook(order_books[book_index][0], new_order, 1);
            } else {
                ExecuteOrders(order_books[book_index][0], new_order, output_file);
                InsertOrderToBook(order_books[book_index][1], new_order, 2);
            }
        } else {
            // If rejected, log it as rejected
            new_order.ExecuteOrder(output_file);
        }
    }

    input_file.close();
    output_file.close();

    // Additional Step: Cleanup the execution report
    std::string inputFile = "execution_report.csv";
    std::string outputFile = "execution_report_final.csv";

    // Step 1: Read the CSV
    std::vector<Row> rows = readCsv(inputFile);

    // Step 2: Remove consecutive 'New' rows
    std::vector<Row> cleanedRows = removeConsecutiveNewRows(rows);

    // Step 3: Remove duplicates only if they have "Fill"
    cleanedRows = removeDuplicates(cleanedRows);

    // Step 4: Write the cleaned data to a new CSV file
    writeCsv(outputFile, cleanedRows);

    std::cout << "Processing complete. Check the file: " << outputFile << std::endl;
}

void ExecuteOrders(vector<Order>& opposite_orders, Order& new_order, ofstream& output_file) {
    while (!opposite_orders.empty()) {
        Order& best_opposite_order = opposite_orders.front();

        if ((new_order.order_side == 1 && best_opposite_order.order_price > new_order.order_price) ||
            (new_order.order_side == 2 && best_opposite_order.order_price < new_order.order_price)) {
            break;
        }

        if (new_order.remaining_quantity == best_opposite_order.remaining_quantity) {
            new_order.status = 2;  // Fully matched
            best_opposite_order.status = 2;

            new_order.ExecuteOrder(output_file);  // Log the fully matched order
            best_opposite_order.ExecuteOrder(output_file);

            opposite_orders.erase(opposite_orders.begin());
            break;
        } else if (new_order.remaining_quantity > best_opposite_order.remaining_quantity) {
            new_order.status = 3;  // Partially filled
            best_opposite_order.status = 2;

            new_order.ExecuteOrder(output_file, best_opposite_order.remaining_quantity);
            new_order.remaining_quantity -= best_opposite_order.remaining_quantity;

            best_opposite_order.ExecuteOrder(output_file);
            opposite_orders.erase(opposite_orders.begin());
        } else {
            new_order.status = 2;
            best_opposite_order.status = 3;

            new_order.ExecuteOrder(output_file);
            best_opposite_order.ExecuteOrder(output_file, new_order.remaining_quantity);

            best_opposite_order.remaining_quantity -= new_order.remaining_quantity;
            break;
        }
    }
}
