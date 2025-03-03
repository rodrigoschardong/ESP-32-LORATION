#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include <stdint.h>
#include <stdbool.h>
#include "../main/main.h"

// Mock das funções
void Set_Pin_Level(uint8_t pin, bool level) {
    mock().actualCall("Set_Pin_Level")
          .withParameter("pin", pin)
          .withParameter("level", level);
}

void delay(uint16_t ms) {
    mock().actualCall("delay").withParameter("ms", ms);
}

void Pin_As_Output(uint8_t pin) {
    mock().actualCall("Pin_As_Output").withParameter("pin", pin);
}

void Pin_As_IRAM_Input(uint8_t pin, uint8_t* interrupt_flag) {
    mock().actualCall("Pin_As_IRAM_Input")
          .withParameter("pin", pin)
          .withParameter("interrupt_flag", interrupt_flag);
}

// Mock para simular a função get_time
static uint64_t mock_time = 0;
uint64_t get_time() {
    return mock_time;
}

// Grupo de testes para Timeout_Expired
TEST_GROUP(TimeoutExpiredGroup) {
    void setup() {
        mock_time = 0;
    }

    void teardown() {}
};

TEST(TimeoutExpiredGroup, TimeNotExpired) {
    uint64_t start = 100;
    uint64_t duration = 50;
    mock_time = 120;
    bool result = Timeout_Expired(start, duration);
    CHECK_FALSE(result);
}

TEST(TimeoutExpiredGroup, TimeExpiredExact) {
    uint64_t start = 100;
    uint64_t duration = 50;
    mock_time = 150;
    bool result = Timeout_Expired(start, duration);
    CHECK_TRUE(result);
}

TEST(TimeoutExpiredGroup, TimeExpiredAfter) {
    uint64_t start = 100;
    uint64_t duration = 50;
    mock_time = 160;
    bool result = Timeout_Expired(start, duration);
    CHECK_TRUE(result);
}

TEST(TimeoutExpiredGroup, HandleOverflowFalse) {
    uint64_t start = UINT64_MAX - 10;
    uint64_t duration = 20;
    mock_time = 5;
    bool result = Timeout_Expired(start, duration);
    CHECK_FALSE(result);
}

TEST(TimeoutExpiredGroup, HandleOverflowTrue) {
    uint64_t start = UINT64_MAX - 10;
    uint64_t duration = 20;
    mock_time = 20;
    bool result = Timeout_Expired(start, duration);
    CHECK_TRUE(result);
}

// Novo grupo para TestWithMocks
TEST_GROUP(SomeTestGroup) {
    void setup() {}

    void teardown() {
        mock().clear();
    }
};

TEST(SomeTestGroup, TestWithMocks) {
    mock().expectOneCall("Set_Pin_Level").withParameter("pin", 2).withParameter("level", true);
    mock().expectOneCall("delay").withParameter("ms", 10);

    Set_Pin_Level(2, true);
    delay(10);

    mock().checkExpectations();
}

int main(int argc, char** argv) {
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
