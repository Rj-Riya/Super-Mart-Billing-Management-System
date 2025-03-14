#include <iostream>
#include <iomanip>
#include <mysql.h>
#include <sstream>  // For stringstream
using namespace std;

char HOST[] = "localhost";
char USER[] = "root";
char PASS[] = "1234";  
char DB[] = "supermarket";

// Convert integer to string using stringstream (alternative to to_string)
string toString(int value) {
    ostringstream oss;
    oss << value;
    return oss.str();
}

// Function to add a new product
void addProduct(MYSQL* conn) {
    string name;
    double price;
    int stock;

    cout << "Enter product name: ";
    cin >> name;
    cout << "Enter product price: ";
    cin >> price;
    cout << "Enter stock quantity: ";
    cin >> stock;

    string query = "INSERT INTO products (name, price, stock) VALUES ('" + name + "', " + toString(price) + ", " + toString(stock) + ")";
    
    if (mysql_query(conn, query.c_str()) != 0) {
        cerr << "Error: " << mysql_error(conn) << endl;
    } else {
        cout << "Product " << name << " added successfully." << endl;
    }
}

// Function to update stock of an existing product
void updateStock(MYSQL* conn) {
    string name;
    int stock;

    cout << "Enter product name: ";
    cin >> name;
    cout << "Enter new stock quantity: ";
    cin >> stock;

    string query = "UPDATE products SET stock = " + toString(stock) + " WHERE name = '" + name + "'";

    if (mysql_query(conn, query.c_str()) != 0) {
        cerr << "Error: " << mysql_error(conn) << endl;
    } else {
        cout << "Stock updated successfully for " << name << "." << endl;
    }
}

// Function to generate a bill
void generateBill(MYSQL* conn) {
    string name;
    int quantity;
    double price, total = 0;

    while (true) {
        cout << "Enter product name (or type 'done' to finish): ";
        cin >> name;
        if (name == "done") break;

        cout << "Enter quantity: ";
        cin >> quantity;

        string query = "SELECT price, stock FROM products WHERE name = '" + name + "'";
        if (mysql_query(conn, query.c_str()) != 0) {
            cerr << "Error: " << mysql_error(conn) << endl;
            continue;
        }

        MYSQL_RES* res = mysql_store_result(conn);
        if (res == NULL || mysql_num_rows(res) == 0) {
            cout << "Product not found!" << endl;
            continue;
        }

        MYSQL_ROW row = mysql_fetch_row(res);
        price = atof(row[0]);
        int stock = atoi(row[1]);

        if (quantity > stock) {
            cout << "Insufficient stock!" << endl;
            mysql_free_result(res);
            continue;
        }

        total += price * quantity;
        cout << "Added " << quantity << " x " << name << " - $" << price * quantity << endl;

        // Update stock after purchase
        string updateQuery = "UPDATE products SET stock = stock - " + toString(quantity) + " WHERE name = '" + name + "'";
        mysql_query(conn, updateQuery.c_str());

        mysql_free_result(res);
    }

    cout << "Total bill amount: $" << fixed << setprecision(2) << total << endl;
}

// Function to view available products
void viewProducts(MYSQL* conn) {
    string query = "SELECT * FROM products";

    if (mysql_query(conn, query.c_str()) != 0) {
        cerr << "Error: " << mysql_error(conn) << endl;
    } else {
        MYSQL_RES* res = mysql_store_result(conn);
        if (res != NULL) {
            cout << "\n------------------------------------------------------------\n";
            cout << setw(10) << left << "ID" 
                 << setw(20) << "Product Name" 
                 << setw(10) << "Price" 
                 << setw(10) << "Stock" << endl;
            cout << "------------------------------------------------------------\n";
            
            while (MYSQL_ROW row = mysql_fetch_row(res)) {
                cout << setw(10) << left << row[0] 
                     << setw(20) << row[1] 
                     << setw(10) << row[2] 
                     << setw(10) << row[3] << endl;
            }
            cout << "------------------------------------------------------------\n";
            mysql_free_result(res);
        }
    }
}

// Main function
int main() {
    MYSQL* conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, HOST, USER, PASS, DB, 0, NULL, 0)) {
        cerr << "Error connecting to MySQL database: " << mysql_error(conn) << endl;
        exit(1);
    }

    int option;
    do {
        cout << "\n1. Add Product\n2. Update Stock\n3. Generate Bill\n4. View Products\n0. Exit\n";
        cout << "Choose an option: ";
        cin >> option;

        switch (option) {
            case 1: addProduct(conn); break;
            case 2: updateStock(conn); break;
            case 3: generateBill(conn); break;
            case 4: viewProducts(conn); break;
        }
    } while (option != 0);

    mysql_close(conn);
    return 0;
}
