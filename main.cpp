#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>

struct Sale {
    std::string date;
    std::string sales_id;
    std::string item_name;
    int item_quantity;
    double unit_price;
};

// Function to generate a sales ID based on current timestamp
std::string generate_sales_id() {
    std::time_t t = std::time(nullptr);
    return "SID" + std::to_string(t);
}

// Parse date string YYYY-MM-DD into a comparable integer YYYYMMDD for sorting
int date_to_int(const std::string& date) {
    std::string s = date;
    s.erase(std::remove(s.begin(), s.end(), '-'), s.end()); // remove dashes
    return std::stoi(s);
}

// Split CSV line by commas, return vector of strings
std::vector<std::string> split_csv_line(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        result.push_back(item);
    }
    return result;
}

// Read sales.csv into vector<Sale>
std::vector<Sale> read_sales_file(const std::string& filename) {
    std::vector<Sale> sales;
    std::ifstream file(filename);
    if (!file.is_open()) return sales; // file not found returns empty vector

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        auto fields = split_csv_line(line);
        if (fields.size() != 5) continue; // skip malformed lines

        Sale s;
        s.date = fields[0];
        s.sales_id = fields[1];
        s.item_name = fields[2];
        s.item_quantity = std::stoi(fields[3]);
        s.unit_price = std::stod(fields[4]);
        sales.push_back(s);
    }
    return sales;
}

// Write vector<Sale> to CSV file
void write_sales_file(const std::string& filename, const std::vector<Sale>& sales) {
    std::ofstream file(filename);
    file << "date,sales_id,item_name,item_quantity,unit_price\n";
    for (const auto& s : sales) {
        file << s.date << "," << s.sales_id << "," << s.item_name << ","
             << s.item_quantity << "," << std::fixed << std::setprecision(2) << s.unit_price << "\n";
    }
}

int main() {
    std::string date, item_name;
    int item_quantity;
    double unit_price;

    std::cout << "Enter date (YYYY-MM-DD): ";
    std::getline(std::cin, date);
    // Basic validation of date format length
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        std::cerr << "Invalid date format.\n";
        return 1;
    }

    std::cout << "Enter item name: ";
    std::getline(std::cin, item_name);

    std::cout << "Enter item quantity: ";
    std::cin >> item_quantity;
    if (item_quantity < 0) {
        std::cerr << "Quantity must be positive.\n";
        return 1;
    }

    std::cout << "Enter unit price: ";
    std::cin >> unit_price;
    if (unit_price < 0) {
        std::cerr << "Unit price must be positive.\n";
        return 1;
    }

    // Generate sales_id
    std::string sales_id = generate_sales_id();

    // Read existing sales
    std::vector<Sale> sales = read_sales_file("sales.csv");

    // Append new record
    sales.push_back({date, sales_id, item_name, item_quantity, unit_price});

    // Write back to sales.csv (overwrite with all data)
    write_sales_file("sales.csv", sales);

    // Sort by date ascending
    std::sort(sales.begin(), sales.end(), [](const Sale& a, const Sale& b) {
        return date_to_int(a.date) < date_to_int(b.date);
    });

    // Write sorted data to temp.csv
    write_sales_file("temp.csv", sales);

    std::cout << "Data saved to sales.csv and sorted data saved to temp.csv\n";
    return 0;
}
