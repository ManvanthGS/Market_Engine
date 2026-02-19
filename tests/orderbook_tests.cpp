#include <gtest/gtest.h>
#include "orderbook/orderbook.hpp"

// Sample test case to verify that print_hello function works correctly
TEST(OrderBookTest, AddOrder) {
    OrderBook book;
    Order order{1, 100, 10, 10, Side::Buy, 1};
    EXPECT_TRUE(book.AddOrder(std::move(order)));
}

TEST(OrderBookTest, BestBid) {
    OrderBook book;
    book.AddOrder({1, 100, 10, 10, Side::Buy, 1});
    book.AddOrder({2, 100, 5, 5, Side::Buy, 2});
    book.AddOrder({3, 101, 7, 7, Side::Sell, 3});

    auto bestBid = book.BestBid();
    EXPECT_NE(bestBid, nullptr);
}

TEST(OrderBookTest, BestAsk) {
    OrderBook book;
    book.AddOrder({1, 100, 10, 10, Side::Buy, 1});
    book.AddOrder({2, 100, 5, 5, Side::Buy, 2});
    book.AddOrder({3, 101, 7, 7, Side::Sell, 3});

    auto bestAsk = book.BestAsk();
    EXPECT_NE(bestAsk, nullptr);
}
