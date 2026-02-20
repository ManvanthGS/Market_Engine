#include "client/market_engine.hpp"
#include "common/timer.hpp"

MarketEngine::MarketEngine() {}

MarketEngine::~MarketEngine() {}

OrderEntry MarketEngine::Convert_To_Order_Entry(const OrderRequest& order_request, const TimeStamp& timestamp, const u64 remaining_quantity)
{
    OrderEntry order_entry;
    order_entry.order_id = order_request.order_id;
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
    order_request.order_id = order_entry.order_id;
    order_request.price = order_entry.price;
    order_request.quantity = order_entry.quantity;
    order_request.side = order_entry.side;
    order_request.type = order_entry.type;

    return order_request;
}

int MarketEngine::Add_Order(const OrderRequest& order_request)
{
    OrderEntry order_entry = Convert_To_Order_Entry(order_request, Timer::Get_Current_Timestamp(), order_request.quantity);

    return order_book.Add_Order_Entry(order_entry);
}

int MarketEngine::Cancel_Order(const u64 order_id)
{
    return order_book.Cancel_Order_Entry(order_id);
}

int MarketEngine::Modify_Order(const OrderRequest& order_request)
{
    OrderEntry order_entry = Convert_To_Order_Entry(order_request, Timer::Get_Current_Timestamp(), order_request.quantity);

    return order_book.Modify_Order_Entry(order_entry);
}

void MarketEngine::Get_Order_Book_Snapshot()
{
    order_book.Get_Order_Book_Snapshot();
}

void MarketEngine::Get_Best_Bid_Ask()
{
    order_book.Get_Best_Bid_Ask();
}

void MarketEngine::Get_Order_Depth()
{
    order_book.Get_Order_Depth();
}
