#include "client/market_engine.hpp"
#include "common/uid_generator.hpp"

MarketEngine::MarketEngine() {}

MarketEngine::~MarketEngine() {}

OrderEntry MarketEngine::Convert_To_Order_Entry(const OrderRequest& order_request, const TimeStamp& timestamp, const u64 remaining_quantity)
{
    OrderEntry order_entry;
    order_entry.order_id = UIDGenerator::Get_Next_ID();
    order_entry.price = order_request.price;
    order_entry.quantity = order_request.quantity;
    order_entry.remaining_quantity = remaining_quantity;
    order_entry.side = order_request.side;
    order_entry.type = order_request.type;
    order_entry.timestamp = timestamp;

    return order_entry;
}

OrderRequest MarketEngine::Convert_To_Order_Request(const OrderEntry& order_entry)
{
    OrderRequest order_request;
    order_request.price = order_entry.price;
    order_request.quantity = order_entry.quantity;
    order_request.side = order_entry.side;
    order_request.type = order_entry.type;

    return order_request;
}

int MarketEngine::Add_Order(const OrderRequest& order_request)
{
    OrderEntry order_entry = Convert_To_Order_Entry(order_request, Timer::Get_Current_Timestamp(), order_request.quantity);

    int error = market_engine_.Process_Order(order_entry);

    return error < 0 ? error : order_entry.order_id; // Return order ID on success, error code on failure
}

// Cancel and Modify operations to be implemented in the future, currently not supported by the Matching Engine

// int MarketEngine::Cancel_Order(const u64 order_id)
// {
//     return market_engine_.Cancel_Order(order_id);
// }

// int MarketEngine::Modify_Order(const OrderRequest& order_request)
// {
//     OrderEntry order_entry = Convert_To_Order_Entry(order_request, Timer::Get_Current_Timestamp(), order_request.quantity);

//     int error = market_engine_.Modify_Order(order_entry);

//     return error < 0 ? error : order_entry.order_id; // Return order ID on success, error code on failure
// }

 OrderBookSnapshot MarketEngine::Get_Order_Book_Snapshot()
{
    return market_engine_.Get_Order_Book_Snapshot();
}

BestBidAsk MarketEngine::Get_Best_Bid_Ask()
{
    return market_engine_.Get_Best_Bid_Ask();
}

OrderDepth MarketEngine::Get_Order_Depth(u32 depth)
{
    return market_engine_.Get_Order_Depth(depth);
}
