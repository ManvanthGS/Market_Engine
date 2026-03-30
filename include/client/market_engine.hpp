#pragma once

#include "common/data_structures.hpp"
#include "common/timer.hpp"
#include "orderbook/matching_engine.hpp"

struct OrderRequest
{
    float price;
    u64 quantity;
    Side side;
    OrderType type;
};

class MarketEngine
{
  private:
    MatchingEngine market_engine_;

    // helper function to convert OrderRequest to OrderEntry
    OrderEntry Convert_To_Order_Entry(const OrderRequest& order_request, const TimeStamp& timestamp, const u64 remaining_quantity);

    // helper function to convert OrderEntry to OrderRequest
    OrderRequest Convert_To_Order_Request(const OrderEntry& order_entry);

  public:
    MarketEngine();
    ~MarketEngine();

    // Market Engine Operations
    int Add_Order(const OrderRequest& order_request);
    int Cancel_Order(const u64 order_id);
    int Modify_Order(const OrderRequest& order_request);

    void Get_Order_Book_Snapshot();
    void Get_Best_Bid_Ask();
    void Get_Order_Depth();
};
