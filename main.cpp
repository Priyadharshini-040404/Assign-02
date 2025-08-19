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

std::string generate_sales_id() {
    std::time_t t = std::time(nullptr);
    return "SID" + std::to_string(t);
}

int date_to_int(const std::string& date) {
    std::string s = date;
    s.erase(std::remove(s.begin(), s.end(), '-'), s.end());
    return std::stoi(s);
}

std::vector<std::string> split_csv_line(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ',')) {
        result.push_back(item);
    }
    return result;
}

std::vector<Sale> read_sales_file(const std::string& filename) {
    std::vector<Sale> sales;
    std::ifstream file(filename);
    if (!file.is_open()) return sales;
    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        auto fields = split_csv_line(line);
        if (fields.size() != 5) continue;
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

void write_sales_file(const std::string& filename, const std::vector<Sale>& sales) {
    std::ofstream file(filename);
    file << "date,sales_id,item_name,item_quantity,unit_price\n";
    for (const auto& s : sales) {
        file << s.date << "," << s.sales_id << "," << s.item_name << ","
             << s.item_quantity << "," << std::fixed << std::setprecision(2) << s.unit_price << "\n";
    }
}

bool valid_date_format(const std::string& date) {
    if (date.size() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;
    // Further validation could be added
    return true;
}

int main() {
    std::vector<Sale> sales = read_sales_file("sales.csv");

    while (true) {
        std::string date, item_name;
        int item_quantity;
        double unit_price;

        std::cout << "Enter date (YYYY-MM-DD): ";
        std::getline(std::cin, date);
        if (!valid_date_format(date)) {
            std::cerr << "Invalid date format.\n";
            continue;  // ask again
        }

        std::cout << "Enter item name: ";
        std::getline(std::cin, item_name);

        std::cout << "Enter item quantity: ";
        std::string quantity_str;
        std::getline(std::cin, quantity_str);
        try {
            item_quantity = std::stoi(quantity_str);
            if (item_quantity < 0) throw std::invalid_argument("negative");
        } catch (...) {
            std::cerr << "Invalid quantity.\n";
            continue;
        }

        std::cout << "Enter unit price: ";
        std::string price_str;
        std::getline(std::cin, price_str);
        try {
            unit_price = std::stod(price_str);
            if (unit_price < 0) throw std::invalid_argument("negative");
        } catch (...) {
            std::cerr << "Invalid unit price.\n";
            continue;
        }

        std::string sales_id = generate_sales_id();

        sales.push_back({date, sales_id, item_name, item_quantity, unit_price});

        std::cout << "Record added.\n";

        std::cout << "Do you want to enter another record? (y/n): ";
        std::string choice;
        std::getline(std::cin, choice);
        if (choice != "y" && choice != "Y") {
            break;
        }
    }

    // Write all records to sales.csv
    write_sales_file("sales.csv", sales);

    // Sort by date ascending and write to temp.csv
    std::sort(sales.begin(), sales.end(), [](const Sale& a, const Sale& b) {
        return date_to_int(a.date) < date_to_int(b.date);
    });
    write_sales_file("temp.csv", sales);

    std::cout << "Data saved to sales.csv and sorted data saved to temp.csv\n";

    return 0;
}
