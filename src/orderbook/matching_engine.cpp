#include "orderbook/matching_engine.hpp"

void MatchingEngine::Match_Orders(const OrderEntry& incoming_order)
{
    // Matching Logic
    auto buy_orders = order_book_.Get_Buy_Orders_Map();
    auto sell_orders = order_book_.Get_Sell_Orders_Map();

    if (incoming_order.side == Side::Buy) {

    } else {

    }
}

MatchingEngine::MatchingEngine() {}

MatchingEngine::~MatchingEngine() {}

int MatchingEngine::Process_Order(const OrderEntry& order_entry)
{
    OrderDepth order_depth = order_book_.Get_Order_Depth(5);
    return order_book_.Add_Order_Entry(order_entry);
}

OrderBookSnapshot MatchingEngine::Get_Order_Book_Snapshot()
{
    return order_book_.Get_Order_Book_Snapshot();
}

BestBidAsk MatchingEngine::Get_Best_Bid_Ask()
{
    return order_book_.Get_Best_Bid_Ask();
}

OrderDepth MatchingEngine::Get_Order_Depth(u32 depth)
{
    return order_book_.Get_Order_Depth(depth);
}
