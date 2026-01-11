include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(market_engine_tests
    test_app.cpp
)

target_link_libraries(market_engine_tests
    PRIVATE
        market_engine
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(market_engine_tests)
