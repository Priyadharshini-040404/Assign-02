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
void append_to_sales_file(const std::string& filename, const Sale& sale, bool write_header = false) {
    std::ofstream file;
    file.open(filename, std::ios::app); // append mode

    if (write_header) {
        file << "date,sales_id,item_name,item_quantity,unit_price\n";
    }

    file << sale.date << "," << sale.sales_id << "," << sale.item_name << ","
         << sale.item_quantity << "," << std::fixed << std::setprecision(2)
         << sale.unit_price << "\n";

    file.close();
}

// Read all sales from CSV
std::vector<Sale> read_sales_file(const std::string& filename) {
    std::vector<Sale> sales;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // skip header

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

// Write all sales to file (overwrite)
void write_sales_to_file(const std::string& filename, const std::vector<Sale>& sales) {
    std::ofstream file(filename);
    file << "date,sales_id,item_name,item_quantity,unit_price\n";

    for (const auto& sale : sales) {
        file << sale.date << "," << sale.sales_id << "," << sale.item_name << ","
             << sale.item_quantity << "," << std::fixed << std::setprecision(2)
             << sale.unit_price << "\n";
    }

    file.close();
}

// Get user input for a sale (reused for add/edit)
Sale get_sale_input(const std::string& sales_id = "") {
    Sale new_sale;
    std::string input;

    // Input and validate date
    while (true) {
        std::cout << "Enter date (YYYY-MM-DD): ";
        std::getline(std::cin, new_sale.date);
        if (valid_date_format(new_sale.date)) break;
        std::cerr << "Invalid date format. Please enter in YYYY-MM-DD.\n";
    }

    // Item name
    std::cout << "Enter item name: ";
    std::getline(std::cin, new_sale.item_name);

    // Quantity
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

    // Unit price
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

    new_sale.sales_id = sales_id.empty() ? generate_sales_id() : sales_id;
    return new_sale;
}

// Sort and write to temp.csv
void sort_and_save_temp(const std::vector<Sale>& sales) {
    std::vector<Sale> sorted_sales = sales;

    std::sort(sorted_sales.begin(), sorted_sales.end(), [](const Sale& a, const Sale& b) {
        return a.date < b.date;
    });

    const std::string temp_file = "temp.csv";
    write_sales_to_file(temp_file, sorted_sales);
    std::cout << "Sorted data written to " << temp_file << "\n";
}

int main() {
    const std::string filename = "sales.csv";
    std::string choice;

    bool file_exists = std::filesystem::exists(filename);
    if (!file_exists) {
        std::ofstream file(filename);
        file << "date,sales_id,item_name,item_quantity,unit_price\n";
        file.close();
    }

    // Step 1: Add new sales
    do {
        Sale new_sale = get_sale_input();
        append_to_sales_file(filename, new_sale);
        std::cout << "Record added successfully.\n";

        std::cout << "Do you want to enter another record? (y/n): ";
        std::getline(std::cin, choice);
    } while (choice == "y" || choice == "Y");

    std::vector<Sale> all_sales = read_sales_file(filename);

    // Step 2: Ask if update is needed
    std::string update_choice;
    std::cout << "Do you want to make any changes in the inputs? (y/n): ";
    std::getline(std::cin, update_choice);

    if (update_choice == "y" || update_choice == "Y") {
        std::string target_id;
        std::cout << "Enter the Sales ID to update: ";
        std::getline(std::cin, target_id);

        bool found = false;
        for (auto& sale : all_sales) {
            if (sale.sales_id == target_id) {
                std::cout << "Enter new details for Sales ID " << target_id << ":\n";
                sale = get_sale_input(target_id);
                found = true;
                break;
            }
        }

        if (found) {
            write_sales_to_file(filename, all_sales);
            std::cout << "Sales ID " << target_id << " updated successfully.\n";
        } else {
            std::cerr << "Sales ID not found.\n";
        }
    }

    // Step 3: Ask if deletion is needed
    std::string delete_choice;
    std::cout << "Do you want to delete any record? (y/n): ";
    std::getline(std::cin, delete_choice);

    if (delete_choice == "y" || delete_choice == "Y") {
        std::string delete_id;
        std::cout << "Enter the Sales ID to delete: ";
        std::getline(std::cin, delete_id);

        auto original_size = all_sales.size();
        all_sales.erase(std::remove_if(all_sales.begin(), all_sales.end(),
            [&](const Sale& s) { return s.sales_id == delete_id; }),
            all_sales.end());

        if (all_sales.size() < original_size) {
            write_sales_to_file(filename, all_sales);
            std::cout << "Sales ID " << delete_id << " deleted successfully.\n";
        } else {
            std::cerr << "Sales ID not found.\n";
        }
    }

    // Step 4: Sort and save to temp.csv
    sort_and_save_temp(all_sales);
    return 0;
}
