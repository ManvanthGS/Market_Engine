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
};
