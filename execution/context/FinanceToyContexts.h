//
// Created by jolechiw on 7/7/23.
//

#ifndef TESTPROJECT_FINANCETOYCONTEXTS_H
#define TESTPROJECT_FINANCETOYCONTEXTS_H

#include <variant>
#include <string>
#include <span>

#include "imgui.h"
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
    void renderImpl(QueueType &queue);

    void runImpl(JITCompiler::CompiledCode &compiledCode, QueueType &messageQueue);
};


#endif //TESTPROJECT_FINANCETOYCONTEXTS_H
