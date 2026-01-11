# Market_Engine

A high-performance C++ limit order book and matching engine designed to model real-world exchange microstructure and serve as a research and benchmarking platform for low-latency trading systems.

## Motivation

Modern quantitative trading depends on precise modeling of market microstructure and extremely low-latency order processing. Most academic and retail trading simulators abstract away the order book. This project builds the actual exchange core: a price-time-priority limit order book, matching engine, and replay system.

## Key Features

- **Price-time priority limit order book** - Maintains order priority based on price and arrival time
- **FIFO matching at each price level** - Fair order matching at identical price levels
- **Order management** - Support for add, cancel, and modify operations
- **Flexible fill types** - Handles both partial and full order fills
- **Market data queries** - Top-of-book and depth-of-book queries
- **Historical replay** (planned) - Replay of historical market data for backtesting
- **Performance instrumentation** (planned) - Latency profiling and optimization tools

## Architecture

The Market Engine follows a modular, layered architecture designed for high performance and low latency. The system is divided into three main components:

### System Overview

![Market Engine Architecture](docs/images/Basic%20Architecture.png)

**Key Components:**

1. **Market Data Feed** - External data source that supplies order and trade events to the system
2. **Limit Order Book (LOB)** - Core component that manages order storage, matching, and state
3. **Performance Profiler** - Monitors system metrics, latency, and throughput

For detailed design specifications, component interactions, and implementation details, see [DESIGN.md](docs/DESIGN.md).


## Project Structure

```
Market_Engine/
├── src/                 # Source code
│   ├── orderbook/      # Limit order book implementation
│   ├── engine/         # Matching engine
│   └── utils/          # Utility functions
├── tests/              # Unit tests
├── docs/               # Documentation
│   └── images/         # Architecture diagrams and images
│   └── DESIGN.md       # Design Document  
├── examples/           # Example usage and demos
└── README.md           # This file
```

## Building and Running

<!-- Add build instructions here -->