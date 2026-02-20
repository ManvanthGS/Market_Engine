#pragma once

#include "common/data_structures.hpp"
#include <iostream>

void print_hello();

class OrderBook
{
  private:
    /* data */
  public:
    // Constructor and Destructor
    OrderBook(/* args */);
    ~OrderBook();

    // OrderBook Operations
    int Add_Order_Entry(const OrderEntry& order_entry);
    int Cancel_Order_Entry(const u64 order_id);
    int Modify_Order_Entry(const OrderEntry& order_entry);

    void Get_Order_Book_Snapshot();
    void Get_Best_Bid_Ask();
    void Get_Order_Depth();

    OrderEntry Get_Order_Entry(const u64 order_id);
};
