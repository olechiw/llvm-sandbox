//
// Created by jolechiw on 7/7/23.
//

#ifndef TESTPROJECT_FINANCETOYCONTEXTS_H
#define TESTPROJECT_FINANCETOYCONTEXTS_H

#include <variant>
#include <string>
#include <span>

#include "BaseExecutionContext.hpp"
#include "../SubProcess.hpp"

using PriceType = uint32_t;
constexpr auto PricePrecision = 16;

using SequenceNumber = uint64_t;

struct MarketPrice {
    PriceType price;
    SequenceNumber sequenceNumber;
};

struct TradedPrice {
    PriceType price;
    SequenceNumber sequenceNumber;
};

struct Output {
    static constexpr auto MAXIMUM_LENGTH = 256;
    char message[MAXIMUM_LENGTH];
};

using FinanceToyPOD = std::variant<MarketPrice, TradedPrice, Output>;

static constexpr auto StarterFile = "#include <stdint.h>\n"
                                    "#include <iostream>\n"
                                    "void handle_price_change(uint32_t new_price) {\n"
                                    "\tstd::cout << \"Received: \" << new_price << std::endl;\n"
                                    "}\n"
                                    "uint32_t get_buy_quote() {\n"
                                    "\n"
                                    "}\n"
                                    "uint32_t get_sell_quote() {\n"
                                    "\n"
                                    "}\n";

class TestFinanceToy : public BaseExecutionContext<TestFinanceToy, FinanceToyPOD> {
public:
    using QueueType = InterprocessQueue<FinanceToyPOD>::ContainerType;
    static constexpr auto Name = "Finance Toy";
    const Files StarterFiles{{"main.cpp", {{"main.cpp", File::Type::CPP, false}, std::string(StarterFile)}}};
    const Files HelperFiles{};
    const JITCompiler::DynamicLibraries DynamicLibraries{};

private:
    friend class BaseExecutionContext<TestFinanceToy, FinanceToyPOD>;
    std::vector<float> data;
    void renderImpl(QueueType &queue) {
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


    void runImpl(JITCompiler::CompiledCode &compiledCode, QueueType &messageQueue) {
        for (uint32_t i = 0; i != 10; ++i) {
            using FunctionType = void (*)(uint32_t);
            void *function = compiledCode.functions["handle_price_change(unsigned int)"];
            ((FunctionType(function))(i * 5));
            messageQueue.push({MarketPrice{i * 5, i}});
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }
};


#endif //TESTPROJECT_FINANCETOYCONTEXTS_H
