//
// Created by jolechiw on 7/10/23.
//

#include "FinanceToyContexts.h"

void TestFinanceToy::renderImpl(TestFinanceToy::QueueType &queue) {
    FinanceToyPOD nextValue{};
    while (queue.pop(nextValue)) {
        auto visitor = [&](auto &&value) {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, MarketPrice>) {
                data.push_back(value.price);
            } else if constexpr (std::is_same_v<T, TradedPrice>) {
//                    pushOutput("Traded " + std::to_string(value.price) + "\n");
            } else if constexpr (std::is_same_v<T, Output>) {
//                    pushOutput(std::string(value.message));
            }
        };
        std::visit(visitor, nextValue);
    }
    bool is_open = true;
    ImGui::Begin("Trading Output", &is_open);
    ImGui::PlotLines("Market Price", &data.front(), data.size());
    ImGui::End();
}

void TestFinanceToy::runImpl(JITCompiler::CompiledCode &compiledCode, TestFinanceToy::QueueType &messageQueue) {
    for (uint32_t i = 0; i != 10; ++i) {
        using FunctionType = void (*)(uint32_t);
        void *function = compiledCode.functions["handle_price_change(unsigned int)"];
        ((FunctionType(function))(i * 5));
        messageQueue.push({MarketPrice{i * 5, i}});
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}