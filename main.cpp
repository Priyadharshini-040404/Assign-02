#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <map>

struct Sale {
    std::string date; // DD/MM/YYYY
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

// Validate DD/MM/YYYY format and year range
bool valid_date_format(const std::string& date) {
    if (date.length() != 10 || date[2] != '/' || date[5] != '/') return false;

    std::string day_str = date.substr(0, 2);
    std::string month_str = date.substr(3, 2);
    std::string year_str = date.substr(6, 4);

    // Ensure all characters are digits
    for (char ch : day_str + month_str + year_str) {
        if (!isdigit(ch)) return false;
    }

    int day = std::stoi(day_str);
    int month = std::stoi(month_str);
    int year = std::stoi(year_str);

    if (year < 1950 || year > 2050) return false;
    if (month < 1 || month > 12) return false;

    int max_day[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        max_day[1] = 29;

    return day >= 1 && day <= max_day[month - 1];
}

// Convert date from DD/MM/YYYY to YYYY-MM-DD (for sorting and report)
std::string convert_date_format(const std::string& date) {
    // date format: DD/MM/YYYY
    std::string day = date.substr(0,2);
    std::string month = date.substr(3,2);
    std::string year = date.substr(6,4);
    return year + "-" + month + "-" + day;
}

void append_to_sales_file(const std::string& filename, const Sale& sale, bool write_header = false) {
    std::ofstream file(filename, std::ios::app);
    if (write_header) {
        file << "date,sales_id,item_name,item_quantity,unit_price\n";
    }
    file << sale.date << "," << sale.sales_id << "," << sale.item_name << ","
         << sale.item_quantity << "," << std::fixed << std::setprecision(2) << sale.unit_price << "\n";
    file.close();
}

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

Sale get_sale_input(const std::string& sales_id = "") {
    Sale new_sale;
    std::string input;

    while (true) {
        std::cout << "Enter date (DD/MM/YYYY): ";
        std::getline(std::cin, new_sale.date);
        if (valid_date_format(new_sale.date)) break;
        std::cerr << "Invalid input. Please enter in DD/MM/YYYY format (Year: 1950 to 2050).\n";
    }

    std::cout << "Enter item name: ";
    std::getline(std::cin, new_sale.item_name);

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

void sort_and_save_temp(const std::vector<Sale>& sales) {
    std::vector<Sale> sorted = sales;

    std::sort(sorted.begin(), sorted.end(), [](const Sale& a, const Sale& b) {
        auto to_date_num = [](const std::string& d) {
            int day = std::stoi(d.substr(0, 2));
            int month = std::stoi(d.substr(3, 2));
            int year = std::stoi(d.substr(6, 4));
            return year * 10000 + month * 100 + day;
        };
        return to_date_num(a.date) < to_date_num(b.date);
    });

    write_sales_to_file("temp.csv", sorted);
    std::cout << "Sorted data written to temp.csv\n";
}

// Get today's date in YYYY-MM-DD format for report header
std::string get_today_date() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(now, "%d-%m-%Y");
    return oss.str();
}


void generate_report(const std::vector<Sale>& sales) {
    std::ofstream report("report.txt");
    if (!report) {
        std::cerr << "Error opening report.txt for writing.\n";
        return;
    }

    report << "Date: " << get_today_date() << "\n";
    report << "Sales Report : Stationary Items Sold\n\n";

    // Group sales by date (convert dates to YYYY-MM-DD for consistent grouping)
    std::map<std::string, std::vector<Sale>> sales_by_date;
    for (const auto& s : sales) {
        std::string d = convert_date_format(s.date);
        sales_by_date[d].push_back(s);
    }

    double grand_total = 0.0;
    const std::string line_sep = std::string(100, '-');

    for (const auto& [date, sales_vec] : sales_by_date) {
        report << line_sep << "\n\n";
        report << "Date                SaleID             ItemName        Quantity     Price          SalesAmount \n\n";
        report << line_sep << "\n\n";

        double subtotal = 0.0;
        for (const auto& sale : sales_vec) {
            double sales_amount = sale.item_quantity * sale.unit_price;
            subtotal += sales_amount;

            report << std::left
                   << std::setw(18) << date
                   << std::setw(18) << sale.sales_id
                   << std::setw(16) << sale.item_name
                   << std::setw(12) << sale.item_quantity
                   << std::setw(14) << std::fixed << std::setprecision(2) << sale.unit_price
                   << std::fixed << std::setprecision(2) << sales_amount
                   << "\n";
        }

        report << "\n" << line_sep << "\n\n";
        report << std::setw(70) << " " << "Subtotal for " << date << " is :" << subtotal << "\n\n";
        report << line_sep << "\n\n";

        grand_total += subtotal;
    }

    report << std::setw(70) << " " << "GRAND TOTAL is : " << grand_total << "\n\n";
    report << "Submission\n";
    report << "End of Report\n";

    report.close();
    std::cout << "Report generated successfully in report.txt\n";
}

int main() {
    const std::string filename = "sales.csv";
    std::string choice;

    if (!std::filesystem::exists(filename)) {
        std::ofstream file(filename);
        file << "date,sales_id,item_name,item_quantity,unit_price\n";
        file.close();
    }

    do {
        Sale new_sale = get_sale_input();
        append_to_sales_file(filename, new_sale);
        std::cout << "Record added successfully.\n";
        std::cout << "Do you want to enter another record? (y/n): ";
        std::getline(std::cin, choice);
    } while (choice == "y" || choice == "Y");

    std::vector<Sale> all_sales = read_sales_file(filename);

    // Update
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

    // Delete
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

    sort_and_save_temp(all_sales);

    // Generate report.txt after all operations
    generate_report(all_sales);
    return 0;
}
