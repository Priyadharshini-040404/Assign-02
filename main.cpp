#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>
#include <iomanip>

// Define a structure to store each sale record
struct Sale {
    std::string date;
    std::string sales_id;
    std::string item_name;
    int item_quantity;
    double unit_price;
};

// Generate a unique sales ID using timestamp
std::string generate_sales_id() {
    std::time_t t = std::time(nullptr);
    return "SID" + std::to_string(t);
}

// Check if the date format is valid (basic check)
bool valid_date_format(const std::string& date) {
    return date.size() == 10 && date[4] == '-' && date[7] == '-';
}

// Write all records to sales.csv (overwrite or create if not exists)
void write_sales_to_file(const std::string& filename, const std::vector<Sale>& sales) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing.\n";
        return;
    }

    // Write header
    file << "date,sales_id,item_name,item_quantity,unit_price\n";

    // Write each sale
    for (const auto& s : sales) {
        file << s.date << "," << s.sales_id << "," << s.item_name << ","
             << s.item_quantity << "," << std::fixed << std::setprecision(2)
             << s.unit_price << "\n";
    }

    file.close();
}

int main() {
    std::vector<Sale> sales;
    std::string choice;

    do {
        Sale new_sale;
        std::string input;

        // Input date
        std::cout << "Enter date (YYYY-MM-DD): ";
        std::getline(std::cin, new_sale.date);
        if (!valid_date_format(new_sale.date)) {
            std::cerr << "Invalid date format. Please use YYYY-MM-DD.\n";
            continue;
        }

        // Input item name
        std::cout << "Enter item name: ";
        std::getline(std::cin, new_sale.item_name);

        // Input item quantity
        std::cout << "Enter item quantity: ";
        std::getline(std::cin, input);
        try {
            new_sale.item_quantity = std::stoi(input);
        } catch (...) {
            std::cerr << "Invalid quantity. Must be an integer.\n";
            continue;
        }

        // Input unit price
        std::cout << "Enter unit price: ";
        std::getline(std::cin, input);
        try {
            new_sale.unit_price = std::stod(input);
        } catch (...) {
            std::cerr << "Invalid unit price. Must be a number.\n";
            continue;
        }

        // Generate sales ID
        new_sale.sales_id = generate_sales_id();

        // Add to list
        sales.push_back(new_sale);

        // Ask if user wants to add more
        std::cout << "Do you want to enter another record? (y/n): ";
        std::getline(std::cin, choice);

    } while (choice == "y" || choice == "Y");

    // Write all collected data to sales.csv
    write_sales_to_file("sales.csv", sales);

    std::cout << "All records saved to sales.csv\n";

    return 0;
}
