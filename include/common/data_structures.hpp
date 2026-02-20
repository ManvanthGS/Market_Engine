#pragma once

#include "common/types.hpp"

enum class OrderType {
    Market,
    Limit,
    Stop
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
