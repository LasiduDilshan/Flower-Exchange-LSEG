#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include "Order.h"
#include <vector>
#include <fstream>

void InsertOrderToBook(std::vector<Order>&, Order&, int);
void ProcessOrders(const std::string&);
void ExecuteOrders(std::vector<Order>&, Order&, std::ofstream&);

#endif
