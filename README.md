# Flower Trading System

## Overview

The Flower Trading System is a sophisticated trading platform designed to facilitate the buying and selling of flowers between clients. The system processes trade orders from a CSV file, matches buying and selling orders based on certain criteria, and logs the results in an execution report. The platform supports multiple types of flowers, including **Rose**, **Lavender**, **Lotus**, **Tulip**, and **Orchid**.

## Features

1. **Order Matching**:
   - Orders are processed from a CSV file containing client order details.
   - The system matches buy and sell orders based on price and quantity.
   - If a buy and sell order match both in price and quantity, they are executed immediately.
   - If no exact match is found, the system defaults to matching orders based on the order they were received.

2. **Exact Quantity Matching**:
   - The system prioritizes matching buy and sell orders with the same quantities.
   - If such an exact quantity match exists, it is executed before considering any other orders.

3. **Handling of Partial and Full Fills**:
   - If an order cannot be fully matched, partial fills are supported.
   - The system continues to match the remaining quantities until the order is fully filled or there are no matching orders left.

4. **Rejection of Invalid Orders**:
   - Orders with invalid sizes, prices, or instruments are rejected and logged with a rejection reason.
   - The system checks for the following validation rules:
     - Valid instruments (e.g., only `Rose`, `Lavender`, `Lotus`, `Tulip`, and `Orchid` are accepted).
     - Valid sides (1 for buy orders, 2 for sell orders).
     - Quantities must be between 10 and 1000, and multiples of 10.
     - Prices must be positive numbers.

5. **Execution Report**:
   - All trades are logged in an `execution_report.csv` file.
   - Each order, whether it is fully executed, partially filled, or rejected, is recorded with details such as order ID, client ID, instrument, side, status, quantity, price, and the reason for rejection (if applicable).
   - The report also includes a timestamp in milliseconds for each transaction to track execution timing.

6. **Multithreading and Parallel Processing**:
   - The system leverages multithreading to improve performance.
   - Orders are processed in parallel, allowing for faster order matching, especially when handling large volumes of trades.
   - A separate thread handles the logging of results, ensuring the system continues processing orders while writing to the report.

## How It Works

### Order Processing
Orders are read from an input CSV file. Each order includes details such as client ID, instrument, side (buy or sell), price, and quantity. The system validates each order and processes it according to the following steps:
1. **Validation**: Ensures the order is valid (correct instrument, side, size, price).
2. **New Order**: Valid orders are initially recorded as "New" in the execution report.
3. **Matching**: The system attempts to match buy and sell orders:
   - Exact matches based on quantity are prioritized.
   - If no exact matches are found, the system matches based on the order in which the buy and sell orders were received.
4. **Logging**: Orders that are matched, partially filled, or rejected are logged in the execution report.

### Rejection Handling
Orders that fail validation are immediately rejected. The rejection reason is logged in the execution report, and the order is not further processed.

### Multithreading and Pipelining
To speed up processing:
- Order matching is done in parallel for different instruments.
- Logging is handled asynchronously, ensuring that the system continues processing new orders while writing results to the `execution_report.csv`.

## Requirements

- C++ compiler with threading support (`g++` with C++11 or later).
- Input CSV file with the following format:
  ```
  Client_Order_ID,Instrument_Name,Side,Price,Quantity
  client_001,Rose,1,50.00,100
  ```
## How to Use

1. **Prepare Input**: Ensure your orders are listed in a CSV file with the required format.
2. **Compile**: Compile the code using a C++11-compatible compiler (e.g., `g++ -std=c++11 -pthread Main.cpp Order.cpp OrderBook.cpp -o flower_exchange`).
3. **Run the Program**: Provide the input CSV file to the program. The system will process the orders and generate the `execution_report.csv`.
4. **Check the Report**: Open the `execution_report.csv` file to view the results of the trades.

![Path](https://github.com/user-attachments/assets/048d4287-890f-42a3-bf84-1927c8669acd
