#include <gtest/gtest.h>
#include "orderbook/orderbook.hpp"

// ----------------------
// Test Fixture
// ----------------------

class OrderBookTest : public ::testing::Test
{
protected:
    OrderBook orderbook_;

    OrderEntry CreateOrder(u64 id, float price, u32 qty, Side side)
    {
        OrderEntry order;
        order.order_id = id;
        order.price = price;
        order.quantity = qty;
        order.side = side;
        return order;
    }
};

// ----------------------
// ADD ORDER TESTS
// ----------------------

TEST_F(OrderBookTest, AddSingleBuyOrder)
{
    auto order = CreateOrder(1, 100.5f, 10, Side::Buy);

    EXPECT_EQ(orderbook_.Add_Order_Entry(order), 0);

    auto stored = orderbook_.Get_Order_Entry(1);
    EXPECT_EQ(stored.price, 100.5f);
    EXPECT_EQ(stored.quantity, 10);
}

TEST_F(OrderBookTest, AddMultipleOrdersSamePrice)
{
    orderbook_.Add_Order_Entry(CreateOrder(1, 100.0f, 10, Side::Buy));
    orderbook_.Add_Order_Entry(CreateOrder(2, 100.0f, 20, Side::Buy));

    const auto& buy_map = orderbook_.Get_Buy_Orders_Map();

    ASSERT_EQ(buy_map.size(), 1);
    ASSERT_EQ(buy_map.at(100.0f).size(), 2);
}

TEST_F(OrderBookTest, BuyOrdersSortedDescending)
{
    orderbook_.Add_Order_Entry(CreateOrder(1, 100.0f, 10, Side::Buy));
    orderbook_.Add_Order_Entry(CreateOrder(2, 101.0f, 10, Side::Buy));

    const auto& buy_map = orderbook_.Get_Buy_Orders_Map();

    auto it = buy_map.begin();
    ASSERT_NE(it, buy_map.end());
    EXPECT_EQ(it->first, 101.0f);
}

TEST_F(OrderBookTest, SellOrdersSortedAscending)
{
    orderbook_.Add_Order_Entry(CreateOrder(1, 100.0f, 10, Side::Sell));
    orderbook_.Add_Order_Entry(CreateOrder(2, 99.0f, 10, Side::Sell));

    const auto& sell_map = orderbook_.Get_Sell_Orders_Map();

    auto it = sell_map.begin();
    ASSERT_NE(it, sell_map.end());
    EXPECT_EQ(it->first, 99.0f);
}

// ----------------------
// CANCEL ORDER TESTS
// ----------------------

TEST_F(OrderBookTest, CancelOrderSuccess)
{
    orderbook_.Add_Order_Entry(CreateOrder(1, 100.0f, 10, Side::Buy));

    EXPECT_EQ(orderbook_.Cancel_Order_Entry(1), 0);

    EXPECT_THROW(orderbook_.Get_Order_Entry(1), std::exception);
}

TEST_F(OrderBookTest, CancelNonExistentOrder)
{
    EXPECT_NE(orderbook_.Cancel_Order_Entry(999), 0);
}

TEST_F(OrderBookTest, CancelRemovesPriceLevel)
{
    orderbook_.Add_Order_Entry(CreateOrder(1, 100.0f, 10, Side::Buy));
    orderbook_.Cancel_Order_Entry(1);

    EXPECT_TRUE(orderbook_.Get_Buy_Orders_Map().empty());
}

// ----------------------
// MODIFY ORDER TESTS
// ----------------------

TEST_F(OrderBookTest, ModifyOrderQuantity)
{
    orderbook_.Add_Order_Entry(CreateOrder(1, 100.0f, 10, Side::Buy));

    auto modified = CreateOrder(1, 100.0f, 50, Side::Buy);

    EXPECT_EQ(orderbook_.Modify_Order_Entry(modified), 0);

    auto stored = orderbook_.Get_Order_Entry(1);
    EXPECT_EQ(stored.quantity, 50);
}

TEST_F(OrderBookTest, ModifyOrderPriceMovesLevel)
{
    orderbook_.Add_Order_Entry(CreateOrder(1, 100.0f, 10, Side::Buy));

    auto modified = CreateOrder(1, 105.0f, 10, Side::Buy);
    orderbook_.Modify_Order_Entry(modified);

    const auto& buy_map = orderbook_.Get_Buy_Orders_Map();

    EXPECT_TRUE(buy_map.find(100.0f) == buy_map.end());
    EXPECT_TRUE(buy_map.find(105.0f) != buy_map.end());
}

TEST_F(OrderBookTest, ModifyNonExistentOrder)
{
    auto order = CreateOrder(999, 100.0f, 10, Side::Buy);

    EXPECT_NE(orderbook_.Modify_Order_Entry(order), 0);
}

// ----------------------
// BEST BID / ASK
// ----------------------

TEST_F(OrderBookTest, BestBidAskBasic)
{
    orderbook_.Add_Order_Entry(CreateOrder(1, 100.0f, 10, Side::Buy));
    orderbook_.Add_Order_Entry(CreateOrder(2, 101.0f, 10, Side::Buy));
    orderbook_.Add_Order_Entry(CreateOrder(3, 102.0f, 10, Side::Sell));

    auto bba = orderbook_.Get_Best_Bid_Ask();

    EXPECT_EQ(bba.best_bid_price, 101.0f);
    EXPECT_EQ(bba.best_ask_price, 102.0f);
}

TEST_F(OrderBookTest, BestBidAskEmpty)
{
    auto bba = orderbook_.Get_Best_Bid_Ask();

    EXPECT_EQ(bba.best_bid_price, 0);
    EXPECT_EQ(bba.best_ask_price, 0);
}

// ----------------------
// ORDER DEPTH
// ----------------------

TEST_F(OrderBookTest, OrderDepthBasic)
{
    orderbook_.Add_Order_Entry(CreateOrder(1, 100.0f, 10, Side::Buy));
    orderbook_.Add_Order_Entry(CreateOrder(2, 99.0f, 20, Side::Buy));
    orderbook_.Add_Order_Entry(CreateOrder(3, 101.0f, 10, Side::Sell));

    auto depth = orderbook_.Get_Order_Depth(2);

    EXPECT_LE(depth.buy_depth.size(), 2);
    EXPECT_LE(depth.sell_depth.size(), 2);
}

// ----------------------
// SNAPSHOT TEST
// ----------------------

TEST_F(OrderBookTest, SnapshotConsistency)
{
    orderbook_.Add_Order_Entry(CreateOrder(1, 100.0f, 10, Side::Buy));
    orderbook_.Add_Order_Entry(CreateOrder(2, 101.0f, 5, Side::Sell));

    auto snapshot = orderbook_.Get_Order_Book_Snapshot();

    EXPECT_FALSE(snapshot.buy_orders.empty());
    EXPECT_FALSE(snapshot.sell_orders.empty());
}

// ----------------------
// FIFO TEST
// ----------------------

TEST_F(OrderBookTest, FIFOWithinPriceLevel)
{
    orderbook_.Add_Order_Entry(CreateOrder(1, 100.0f, 10, Side::Buy));
    orderbook_.Add_Order_Entry(CreateOrder(2, 100.0f, 20, Side::Buy));

    const auto& orders = orderbook_.Get_Buy_Orders_Map().at(100.0f);

    ASSERT_EQ(orders.size(), 2);
    EXPECT_EQ(orders[0].order_id, 1);
    EXPECT_EQ(orders[1].order_id, 2);
}

// ----------------------
// DUPLICATE ORDER TEST
// ----------------------

TEST_F(OrderBookTest, DuplicateOrderIdRejected)
{
    orderbook_.Add_Order_Entry(CreateOrder(1, 100.0f, 10, Side::Buy));

    EXPECT_NE(orderbook_.Add_Order_Entry(CreateOrder(1, 101.0f, 10, Side::Buy)), 0);
}

// ----------------------
// STRESS TEST
// ----------------------

TEST_F(OrderBookTest, StressAddCancel)
{
    const int N = 10000;

    for (int i = 0; i < N; i++)
    {
        orderbook_.Add_Order_Entry(CreateOrder(i, 100.0f + (i % 10), 10, Side::Buy));
    }

    for (int i = 0; i < N; i++)
    {
        EXPECT_EQ(orderbook_.Cancel_Order_Entry(i), 0);
    }

    EXPECT_TRUE(orderbook_.Get_Buy_Orders_Map().empty());
}
