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
    float best_bid_price;
    float best_ask_price;
    u64 best_bid_quantity;
    u64 best_ask_quantity;
    float bid_ask_spread;
};

struct OrderDepth
{
    std::vector<std::pair<float, u64>> buy_depth; // price, quantity
    std::vector<std::pair<float, u64>> sell_depth; // price, quantity
};
