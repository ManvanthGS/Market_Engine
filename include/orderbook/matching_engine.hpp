#pragma once

#include "common/data_structures.hpp"
#include "orderbook/orderbook.hpp"

class MatchingEngine
{
  private:
    OrderBook order_book_;

    void Match_Orders(const OrderEntry& incoming_order);
    void Insert_Order(const OrderEntry& order_entry);
    void Remove_Order(const OrderEntry& order_entry);

    bool Is_Crossing(const OrderEntry& incoming_order, const OrderEntry& book_order);

  public:
    MatchingEngine();
    ~MatchingEngine();

    int Process_Order(const OrderEntry& order_entry);
    int Cancel_Order(const u64 order_id);
    int Modify_Order(const OrderEntry& order_entry);

    OrderBookSnapshot Get_Order_Book_Snapshot();
    BestBidAsk Get_Best_Bid_Ask();
    OrderDepth Get_Order_Depth(u32 depth);
};
