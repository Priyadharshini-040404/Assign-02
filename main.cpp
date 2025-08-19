#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <algorithm>

struct Sale {
    std::string date;
    std::string sales_id;
    std::string item_name;
    int item_quantity;
    double unit_price;
};

// Generate unique sales ID using timestamp
std::string generate_sales_id() {
    std::time_t t = std::time(nullptr);
    return "SID" + std::to_string(t);
}

// Check if date format is valid
bool valid_date_format(const std::string& date) {
    return date.size() == 10 && date[4] == '-' && date[7] == '-';
}

// Append new sale to sales.csv
void append_to_sales_file(const std::string& filename, const Sale& sale) {
    std::ofstream file;
    bool file_exists = std::filesystem::exists(filename);
    file.open(filename, std::ios::app); // append mode

    if (!file_exists) {
        file << "date,sales_id,item_name,item_quantity,unit_price\n";
    }

    file << sale.date << "," << sale.sales_id << "," << sale.item_name << ","
         << sale.item_quantity << "," << std::fixed << std::setprecision(2)
         << sale.unit_price << "\n";

    file.close();
}

// Read all records from sales.csv into a vector
std::vector<Sale> read_sales_file(const std::string& filename) {
    std::vector<Sale> sales;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        Sale sale;
        std::string quantity, price;

        std::getline(ss, sale.date, ',');
        std::getline(ss, sale.sales_id, ',');
        std::getline(ss, sale.item_name, ',');
        std::getline(ss, quantity, ',');
        std::getline(ss, price, ',');

        sale.item_quantity = std::stoi(quantity);
        sale.unit_price = std::stod(price);

        sales.push_back(sale);
    }

    file.close();
    return sales;
}

// Write sorted sales data to temp.csv
void write_sorted_sales(const std::vector<Sale>& sales, const std::string& filename) {
    std::ofstream file(filename);
    file << "date,sales_id,item_name,item_quantity,unit_price\n";

    for (const auto& sale : sales) {
        file << sale.date << "," << sale.sales_id << "," << sale.item_name << ","
             << sale.item_quantity << "," << std::fixed << std::setprecision(2)
             << sale.unit_price << "\n";
    }

    file.close();
}

int main() {
    const std::string filename = "sales.csv";
    std::string choice;

    do {
        Sale new_sale;
        std::string input;

        // Input and validate date
        while (true) {
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::getline(std::cin, new_sale.date);
            if (valid_date_format(new_sale.date)) break;
            std::cerr << "Invalid date format. Please enter in YYYY-MM-DD.\n";
        }

        // Input item name
        std::cout << "Enter item name: ";
        std::getline(std::cin, new_sale.item_name);

        // Input and validate item quantity
        while (true) {
            std::cout << "Enter item quantity: ";
            std::getline(std::cin, input);
            try {
                new_sale.item_quantity = std::stoi(input);
                break;
            } catch (...) {
                std::cerr << "Invalid quantity. Please enter a valid integer.\n";
            }
        }

        // Input and validate unit price
        while (true) {
            std::cout << "Enter unit price: ";
            std::getline(std::cin, input);
            try {
                new_sale.unit_price = std::stod(input);
                break;
            } catch (...) {
                std::cerr << "Invalid price. Please enter a valid number.\n";
            }
        }

        // Generate sales ID
        new_sale.sales_id = generate_sales_id();

        // Append to file
        append_to_sales_file(filename, new_sale);
        std::cout << "Record added successfully to " << filename << "\n";

        // Ask to continue
        std::cout << "Do you want to enter another record? (y/n): ";
        std::getline(std::cin, choice);

    } while (choice == "y" || choice == "Y");

    // Ask if user wants to make any changes
    std::string edit_choice;
    std::cout << "Do you want to make any changes in the inputs? (y/n): ";
    std::getline(std::cin, edit_choice);

    if (edit_choice == "n" || edit_choice == "N") {
        std::vector<Sale> sales = read_sales_file(filename);

        // Sort the sales by date (lexicographically works for YYYY-MM-DD)
        std::sort(sales.begin(), sales.end(), [](const Sale& a, const Sale& b) {
            return a.date < b.date;
        });

        // Write to temp.csv (create if doesn't exist)
        const std::string temp_filename = "temp.csv";
        write_sorted_sales(sales, temp_filename);
        std::cout << "Sorted data written to " << temp_filename << "\n";
    } else {
        std::cout << "Please update your inputs and run the program again.\n";
    }

    std::cout << "Program completed.\n";
    return 0;
}
