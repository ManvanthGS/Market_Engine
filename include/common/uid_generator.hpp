#pragma once

#include <atomic>
#include "types.hpp"

class UIDGenerator
{
  private:
    std::atomic<u64> current_id_;

  public:
    UIDGenerator() : current_id_(0) {}

    static u64 Get_Next_ID()
    {
        static UIDGenerator generator;
        return generator.current_id_++;
    }
};
