#pragma once

#include "common/data_structures.hpp"
#include "orderbook/orderbook.hpp"

class MatchingEngine
{
  private:
    OrderBook order_book_;

    void Match_Orders(const OrderEntry& incoming_order);

  public:
    MatchingEngine();
    ~MatchingEngine();

    int Process_Order(const OrderEntry& order_entry);
    void Get_Order_Book_Snapshot();
    void Get_Best_Bid_Ask();
    void Get_Order_Depth(u32 depth);
};
