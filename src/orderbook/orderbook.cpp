#include "orderbook/orderbook.hpp"
#include <algorithm>

void print_hello() {
    std::cout << "Hello, OrderBook!" << std::endl;
}

OrderBook::OrderBook(/* args */) {}

OrderBook::~OrderBook() {}

int OrderBook::Add_Order_Entry(const OrderEntry& order_entry)
{
    // Add to order_entries map
    order_entries[order_entry.order_id] = order_entry;

    // Add to buy_orders or sell_orders based on side
    if (order_entry.side == Side::Buy) {
        buy_orders[order_entry.price].push_back(order_entry);
    } else {
        sell_orders[order_entry.price].push_back(order_entry);
    }

    return 0; // Success
}

int OrderBook::Cancel_Order_Entry(const u64 order_id)
{
    auto it = order_entries.find(order_id);
    if (it == order_entries.end()) {
        return -1; // Order not found
    }

    OrderEntry order_entry = it->second;

    // Remove from buy_orders or sell_orders based on side
    if (order_entry.side == Side::Buy) {
        auto& orders_at_price = buy_orders[order_entry.price];
        orders_at_price.erase(std::remove_if(orders_at_price.begin(), orders_at_price.end(),
                                            [&](const OrderEntry& entry) { return entry.order_id == order_id; }),
                              orders_at_price.end());
        if (orders_at_price.empty()) {
            buy_orders.erase(order_entry.price);
        }
    } else {
        auto& orders_at_price = sell_orders[order_entry.price];
        orders_at_price.erase(std::remove_if(orders_at_price.begin(), orders_at_price.end(),
                                            [&](const OrderEntry& entry) { return entry.order_id == order_id; }),
                              orders_at_price.end());
        if (orders_at_price.empty()) {
            sell_orders.erase(order_entry.price);
        }
    }

    // Remove from order_entries map
    order_entries.erase(it);

    return 0; // Success
}

int OrderBook::Modify_Order_Entry(const OrderEntry& order_entry)
{
    OrderEntry existing_order = Get_Order_Entry(order_entry.order_id);

    // Cancel the existing order
    int cancel_result = Cancel_Order_Entry(order_entry.order_id);
    if (cancel_result != 0) {
        return cancel_result; // Failed to cancel existing order
    }

    // Update the order entry with new details
    OrderEntry modified_order;

    // If price is modified, we need to update the price level in the order book
    modified_order.order_id = order_entry.order_id;
    modified_order.price = order_entry.price;

    modified_order.quantity = order_entry.quantity != 0 ? order_entry.quantity : existing_order.quantity;
    modified_order.remaining_quantity = order_entry.remaining_quantity != 0 ? order_entry.remaining_quantity : existing_order.remaining_quantity;

    if(order_entry.quantity == existing_order.quantity) {
        modified_order.remaining_quantity = existing_order.remaining_quantity; // If quantity is unchanged, keep remaining quantity the same
    } else if(order_entry.quantity < existing_order.quantity) {
        modified_order.remaining_quantity = existing_order.remaining_quantity - (existing_order.quantity - order_entry.quantity); // Reduce remaining quantity if total quantity is reduced
    } else {
        modified_order.remaining_quantity = existing_order.remaining_quantity + (order_entry.quantity - existing_order.quantity); // Increase remaining quantity if total quantity is increased
    }

    modified_order.side = order_entry.side; // update side
    modified_order.type = order_entry.type; // update type
    modified_order.timestamp = existing_order.timestamp; // Timestamp remains unchanged


    // Add the modified order
    return Add_Order_Entry(modified_order);
}

OrderBookSnapshot OrderBook::Get_Order_Book_Snapshot()
{
    OrderBookSnapshot snapshot;

    // Collect buy orders
    for (const auto& [price, orders] : buy_orders) {
        snapshot.buy_orders.insert(snapshot.buy_orders.end(), orders.begin(), orders.end());
    }

    // Collect sell orders
    for (const auto& [price, orders] : sell_orders) {
        snapshot.sell_orders.insert(snapshot.sell_orders.end(), orders.begin(), orders.end());
    }

    return snapshot;
}

BestBidAsk OrderBook::Get_Best_Bid_Ask()
{
    BestBidAsk best_bid_ask;

    if (!buy_orders.empty()) {
        const auto& best_bid = buy_orders.begin()->second.front();
        best_bid_ask.bid_price = best_bid.price;
        best_bid_ask.bid_quantity = best_bid.remaining_quantity;
    }

    if (!sell_orders.empty()) {
        const auto& best_ask = sell_orders.begin()->second.front();
        best_bid_ask.ask_price = best_ask.price;
        best_bid_ask.ask_quantity = best_ask.remaining_quantity;
    }

    if (best_bid_ask.bid_price > 0 && best_bid_ask.ask_price > 0) {
        best_bid_ask.spread = best_bid_ask.ask_price - best_bid_ask.bid_price;
    } else {
        best_bid_ask.spread = 0; // No valid bid or ask
    }

    return best_bid_ask;
}

OrderDepth OrderBook::Get_Order_Depth(u32 depth)
{
    OrderDepth order_depth;

    // Get buy depth
    for (const auto& [price, orders] : buy_orders) {
        if (order_depth.buy_depth.size() >= depth) {
            break;
        }
        u64 total_quantity = 0;
        for (const auto& order : orders) {
            total_quantity += order.remaining_quantity;
        }
        order_depth.buy_depth.emplace_back(price, total_quantity);
    }

    // Get sell depth
    for (const auto& [price, orders] : sell_orders) {
        if (order_depth.sell_depth.size() >= depth) {
            break;
        }
        u64 total_quantity = 0;
        for (const auto& order : orders) {
            total_quantity += order.remaining_quantity;
        }
        order_depth.sell_depth.emplace_back(price, total_quantity);
    }

    return order_depth;
}

OrderEntry OrderBook::Get_Order_Entry(const u64 order_id)
{
    auto it = order_entries.find(order_id);
    if (it != order_entries.end()) {
        return it->second;
    }
    throw std::runtime_error("Order not found");
}
