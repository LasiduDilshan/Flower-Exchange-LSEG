#ifndef ORDER_H
#define ORDER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class Order {
public:
    string client_order_id;
    string instrument_name;
    int order_side;
    double order_price;
    int order_quantity;
    string order_id;
    int status;
    string reason;
    int remaining_quantity;
    int order_priority;

    static const vector<string> instrument_list;  // Changed to vector

    // Default constructor
    Order();

    // Parameterized constructor
    Order(const string&, const string&, const string&, const string&, const string&);

    // Copy constructor
    Order(const Order& other);

    // Copy assignment operator
    Order& operator=(const Order& other);

    // Move constructor
    Order(Order&& other) noexcept;

    // Move assignment operator
    Order& operator=(Order&& other) noexcept;

    void ValidateOrder(const string&, const string&, const string&, const string&, const string&);
    bool IsNotRejected() const;
    void ExecuteOrder(ofstream&, int);
    void ExecuteOrder(ofstream&);

private:
    bool IsInstrumentValid(const string&);
    int ParseOrderSide(const string&);
    double ParsePrice(const string&);
    int ParseQuantity(const string&);
    string GenerateOrderId();
};

#endif
