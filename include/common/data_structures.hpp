#pragma once

#include "common/types.hpp"

enum class OrderType {
    Market,
    Limit
};

enum class Side {
    Buy,
    Sell
};

struct  Order {
    u64 order_id;
    u64 price;
    u64 quantity;
    u64 remaining_quantity;
    Side side;
    OrderType type;
    TimeStamp timestamp;
};
