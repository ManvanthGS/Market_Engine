# Market Engine - Design Document

## Overview

This document provides detailed design specifications for the Market Engine, a high-performance C++ limit order book and matching engine. It covers the system architecture, component design, data structures, algorithms, and implementation considerations.

## Table of Contents

1. [Market Model](#market-model)
2. [Architecture](#architecture)
3. [Core Components](#core-components)
4. [Data Structures](#data-structures)
5. [Order Matching Algorithm](#order-matching-algorithm)
6. [Performance Considerations](#performance-considerations)
7. [Thread Safety](#thread-safety)
8. [Testing Strategy](#testing-strategy)
9. [Development Phases](#development-phases)
10. [Future Extensions](#future-extensions)

## Market Model

Market Engine models a continuous double-auction market identical to those used by modern electronic exchanges (e.g., NASDAQ, NYSE).

The market operates using:
- Limit orders and market orders
- Price–time priority
- FIFO matching at each price level

Trades occur when a new order is willing to buy at the seller’s price or sell at the buyer’s price. It takes whatever quantity is available on the other side of the book. If the order is bigger than what’s available, part of it gets filled right away, and the rest stays in the order book waiting for future matches.

## Architecture
The architecture is designed to prioritize deterministic behavior, low-latency execution, and clear separation of responsibilities, with future extensibility toward multithreaded execution.

### Layered Architecture

The system is organized in three layers:

```
┌────────────────────────────────────────┐
│        Application/API Layer           │
│     (Order submission, queries)        │
├────────────────────────────────────────┤
│       Matching Engine Layer            │
│  (Order processing & execution)        │
├────────────────────────────────────────┤
│      Data Structure Layer              │
│  (Order storage & retrieval)           │
└────────────────────────────────────────┘
```

### Component Interaction Flow

```
Market Data Feed
        ↓
    ┌───────────────────────┐
    │  Order Processor      │
    │ (Validate, route)     │
    └─────────┬─────────────┘
              ↓
    ┌───────────────────────┐
    │  Matching Engine      │
    │ (Match, execute)      │
    └─────────┬─────────────┘
              ↓
    ┌───────────────────────┐
    │  Limit Order Book     │
    │ (Maintain state)      │
    └─────────┬─────────────┘
              ↓
    ┌───────────────────────┐
    │  Output Generation    │
    │ (Trades, fills)       │
    └───────────────────────┘
              ↓
    ┌───────────────────────┐
    │ Performance Profiler  │
    │ (Metrics & stats)     │
    └───────────────────────┘
```

## Core Components

### 1. Order Book (`orderbook/`)

**Responsibilities:**
- Maintain the current state of all active orders
- Support efficient price-time priority queries
- Track order quantities at each price level

**Key Classes:**
- `OrderBook` - Main order book manager
- `PriceLevelContainer` - Manages orders at a single price level
- `Order` - Individual order representation

**Operations:**
- `addOrder()` - Insert new order with O(log n) complexity
- `cancelOrder()` - Remove order in O(log n) time
- `modifyOrder()` - Update order size/price
- `getBestBid()` / `getBestAsk()` - O(1) top-of-book queries
- `getDepth(levels)` - Get market depth snapshot

### 2. Matching Engine (`engine/`)

**Responsibilities:**
- Match incoming orders against the order book
- Execute trades according to price-time priority
- Handle partial and full fills

**Matching Algorithm:**
1. For each incoming order:
   - Check if it can be matched against standing orders
   - Execute matches in price-time priority order
   - Generate trade records for each execution
   - Place remaining quantity as standing order (if any)

**Key Operations:**
- `submitOrder()` - Process new order request
- `cancelOrder()` - Remove pending order
- `modifyOrder()` - Adjust existing order
- `executeMatch()` - Execute individual trade

### 3. Performance Profiler (`utils/profiler/`)

**Responsibilities:**
- Measure operation latencies
- Track throughput metrics
- Record order book depth statistics
- Generate performance reports

**Metrics Tracked:**
- Order processing latency (ns)
- Order book update time
- Match execution time
- Fill notification latency
- Memory usage

## Data Structures

The Market Engine is built around data structures designed to provide:

- Price–time priority
- O(log N) access to price levels
- O(1) access to the next order to be matched
- Cache-friendly traversal for hot paths

The design separates price discovery from order sequencing.

### Order Representation

```cpp
struct Order {
    uint64_t orderId;           // Unique identifier
    uint64_t timestamp;         // Submission time (nanoseconds)
    Side side;                  // BUY or SELL
    uint32_t price;             // Order price
    uint64_t quantity;          // Initial quantity
    uint64_t remainingQuantity; // Unfilled quantity
    OrderStatus status;         // PENDING, PARTIAL_FILL, FILLED, CANCELLED
    uint32_t parentId;          // Link to original order (for modifications)
};
```

### Price Level Structure

```cpp
struct PriceLevel {
    uint32_t price;
    std::deque<Order> orders;   // FIFO queue of orders at this price
    uint64_t totalQuantity;     // Sum of all order quantities
    uint64_t timestamp;         // Most recent order timestamp
};
```

### Order Book State

The order book maintains two independent sorted structures:

1. Bids: prices sorted in descending order
2. Asks: prices sorted in ascending order

Each price maps to a PriceLevel.

```
bids: map<Price, PriceLevel, descending>
asks: map<Price, PriceLevel, ascending>
```

A separate hash map provides O(1) lookup from orderId → Order* for cancellation and modification.

This structure allows:
- O(log N) insertion into price levels
- O(1) access to best bid and best ask
- O(1) access to the next order to match at any price

## Order Matching Algorithm

Market Engine implements a strict **price–time priority** matching algorithm identical to that used by modern electronic exchanges.

### Matching Rules

An incoming order is matched against the best available prices on the opposite side of the book:

- Buy orders match against the lowest ask prices
- Sell orders match against the highest bid prices

Within a price level, orders are matched in FIFO (time) order.

Partial fills are supported. If an order is not fully filled, its remaining quantity either continues matching or is placed into the book as a resting order.

---

### Matching Process

For each incoming order:

1. Identify the best price on the opposite side of the book
2. If the price crosses the incoming order’s limit, matching begins
3. Orders at that price are consumed in FIFO order
4. Trades are generated for each fill
5. Fully filled orders are removed
6. Partially filled orders remain with updated quantity
7. If the incoming order is not fully filled, it becomes a resting order

### High-Level Pseudocode

```
function submitOrder(order):
    if order.side == BUY:
        bestAsk = orderBook.getLowestAsk()
        while order.remainingQuantity > 0 and bestAsk <= order.price:
            matchQty = min(bestAsk.quantity, order.remainingQuantity)
            executeTrade(order, bestAsk, matchQty)
            bestAsk = orderBook.getNextAsk()
    else:  // SELL
        bestBid = orderBook.getHighestBid()
        while order.remainingQuantity > 0 and bestBid >= order.price:
            matchQty = min(bestBid.quantity, order.remainingQuantity)
            executeTrade(order, bestBid, matchQty)
            bestBid = orderBook.getPreviousBid()
    
    if order.remainingQuantity > 0:
        orderBook.addOrder(order)
    
    return order
```

## Performance Considerations

The engine is designed to support low-latency and high-throughput market simulation and benchmarking.

Initial phases prioritize correctness and determinism. Later phases optimize tail latency, cache locality, and throughput.

---

### Design Goals

- Deterministic matching behavior
- O(log N) price-level operations
- O(1) FIFO order access
- Minimal allocation on the hot path
- Low tail latency (P99, P999)

---

### Memory Management

- Orders are allocated from pre-allocated pools
- No dynamic allocation occurs during matching
- Order objects are reused after completion
- Queues store pointers, not objects

---

### Cache Locality

- Best bid and best ask are cached
- Price levels are stored contiguously when possible
- Hot data paths minimize pointer chasing

---

### Profiling Integration

Every stage of the order lifecycle is instrumented:

- Ingestion
- Matching
- Book update
- Trade generation

This allows precise measurement of latency distribution and jitter.


## Thread Safety

Threading is introduced incrementally to preserve correctness and to enable accurate latency measurement.

---

### Phase 1 – Single-Threaded Baseline

The initial implementation is single-threaded. This ensures:

- Deterministic behavior
- Simplified debugging
- Accurate baseline latency measurement

All book and matching operations execute on one thread.

---

### Future Phases – Concurrent Execution

Later phases introduce concurrency using a staged pipeline:

- Feed ingestion thread
- Matching engine thread
- Statistics and output thread

The order book will follow a single-writer model with lock-free queues for input and event propagation.

## Testing Strategy

### Unit Tests

- **Order Book Tests:**
  - Add/remove/modify operations
  - Price-time priority verification
  - Boundary conditions (empty book, single level, etc.)

- **Matching Engine Tests:**
  - Full fills, partial fills, no fills
  - Multiple price levels
  - Order modification during execution

- **Performance Tests:**
  - Latency benchmarks
  - Throughput measurements
  - Memory usage profiling

### Integration Tests

- Multi-order scenarios
- Market data replay
- Stress tests with high volume

### Validation Tests

- Verify price-time priority correctness
- Check trade execution accuracy
- Validate order book consistency

## Development Phases

- **Phase 1:** Correctness-first single-threaded limit order book
- **Phase 2:** Matching engine and trade generation
- **Phase 3:** Historical market data replay
- **Phase 4:** Latency instrumentation and optimization
- **Phase 5:** Multithreaded market engine
- **Phase 6:** Visualization and advanced analytics

## Future Extensions

1. **Historical Data Replay** - Playback of market data for backtesting
2. **Advanced Order Types** - IOC, FOK, STOP, STOP-LIMIT orders
3. **Multi-instrument Support** - Handle multiple symbols simultaneously
4. **Risk Management** - Position limits, exposure checks
5. **Analytics** - VWAP, TWAP, order book depth analysis
6. **Distributed System** - Multi-venue support with cross-venue matching
