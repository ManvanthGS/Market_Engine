#pragma once

#include "types.hpp"
#include "timer.hpp"

enum class OrderType {
    Market,
    Limit
};

enum class Side {
    Buy,
    Sell
};

struct OrderEntry {
    u64 order_id;
    float price;
    u64 quantity;
    u64 remaining_quantity;
    Side side;
    OrderType type;
    TimeStamp timestamp;
};

struct OrderBookSnapshot {
    std::vector<OrderEntry> buy_orders;
    std::vector<OrderEntry> sell_orders;
};

struct BestBidAsk
{
    float bid_price;
    float ask_price;
    u64 bid_quantity;
    u64 ask_quantity;
    float spread;
};

struct OrderDepth
{
    std::vector<std::pair<float, u64>> buy_depth; // price, quantity
    std::vector<std::pair<float, u64>> sell_depth; // price, quantity
};
