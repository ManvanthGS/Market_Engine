#pragma once

#include "common/data_structures.hpp"
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

void print_hello();

class OrderBook
{
  private:
    // Map of order_id to OrderEntry for quick access
    std::unordered_map<u64, OrderEntry> order_entries;

    // Map of price to list of buy orders sorted in descending order
    std::map<float, std::vector<OrderEntry>, std::greater<float>> buy_orders;

    // Map of price to list of sell orders sorted in ascending order
    std::map<float, std::vector<OrderEntry>, std::less<float>> sell_orders;

  public:
    // Constructor and Destructor
    OrderBook(/* args */);
    ~OrderBook();

    // OrderBook Operations
    int Add_Order_Entry(const OrderEntry& order_entry);
    int Cancel_Order_Entry(const u64 order_id);
    int Modify_Order_Entry(const OrderEntry& order_entry);

    OrderBookSnapshot Get_Order_Book_Snapshot();
    BestBidAsk Get_Best_Bid_Ask();
    OrderDepth Get_Order_Depth(u32 depth);

    OrderEntry Get_Order_Entry(const u64 order_id);
};
