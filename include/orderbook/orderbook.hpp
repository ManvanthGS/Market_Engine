#pragma once

#include "common/data_structures.hpp"
#include "common/types.hpp"
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

using OrdersMap = std::unordered_map<u64, OrderEntry>;
using PriceList = std::vector<OrderEntry>;
using BuyOrdersMap = std::map<float, PriceList, std::greater<float>>;
using SellOrdersMap = std::map<float, PriceList, std::less<float>>;

void print_hello();

class OrderBook
{
  private:
    // Map of order_id to OrderEntry for quick access
    OrdersMap order_entries_;

    // Map of price to list of buy orders sorted in descending order
    BuyOrdersMap buy_orders_;

    // Map of price to list of sell orders sorted in ascending order
    SellOrdersMap sell_orders_;

  public:
    // Constructor and Destructor
    OrderBook(/* args */);
    ~OrderBook();

    // OrderBook Operations
    int Add_Order_Entry(const OrderEntry& order_entry);
    int Cancel_Order_Entry(const u64 order_id);
    int Modify_Order_Entry(const OrderEntry& order_entry);

    // snapshots
    OrderBookSnapshot Get_Order_Book_Snapshot();
    BestBidAsk Get_Best_Bid_Ask();
    OrderDepth Get_Order_Depth(u32 depth);

    OrderEntry Get_Order_Entry(const u64 order_id);
    BuyOrdersMap const& Get_Buy_Orders_Map();
    SellOrdersMap const& Get_Sell_Orders_Map();
};
