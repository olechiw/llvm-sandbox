//
// Created by jolechiw on 7/7/23.
//

#ifndef TESTPROJECT_FINANCETOYCONTEXTS_H
#define TESTPROJECT_FINANCETOYCONTEXTS_H

#include <variant>
#include <string>

#include "ExecutionContext.h"
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

static constexpr auto StarterFile =
        "#include <stdint.h>\n"
        "void handle_price_change(uint32_t new_price) {\n"
        "\n"
        "}\n"
        "uint32_t get_buy_quote() {\n"
        "\n"
        "}\n"
        "uint32_t get_sell_quote() {\n"
        "\n"
        "}\n";

class TestFinanceToy : public ExecutionContext<TestFinanceToy> {
    using SPType = SubProcess<FinanceToyPOD>;
    using QueueType = InterprocessQueue<FinanceToyPOD>::lockfree_ipc_queue;
public:
    const Files StarterFiles {
            {"main.cpp",
             {
                     {
                             "main.cpp",
                             File::Type::CPP,
                             false},
                    std::string(StarterFile)}
            }
    };
    const Files HelperFiles {};
    const JITCompiler::DynamicLibraries DynamicLibraries {};

    void run(JITCompiler::CompiledCode &compiledCode) {
        assert(!isRunning());
        auto runFunction = [&](QueueType &messageQueue) {
            runImpl(messageQueue, compiledCode);
        };
        _subProcess = SPType::spawn(runFunction);
    }

    bool isRunning() {
        return _subProcess && _subProcess->isRunning();
    }

    void render() {
        // TODO: render
        if (!_subProcess) return;
        bool isRunning = _subProcess->isRunning();

        FinanceToyPOD nextValue {};
        while (_subProcess->getQueue().pop(nextValue)) {
            auto visitor = [&](auto &&value) {
                using T = std::decay_t<decltype(value)>;

                if constexpr (std::is_same_v<T, MarketPrice>) {
                    output.push_back("Market " + std::to_string(value.price) + "\n");
                } else if constexpr (std::is_same_v<T, TradedPrice>) {
                    output.push_back("Traded " + std::to_string(value.price) + "\n");
                } else if constexpr (std::is_same_v<T, Output>) {
                    output.push_back(std::string(value.message));
                }
            };
            std::visit(visitor, nextValue);
        }
        if (!isRunning) {
            switch (_subProcess->getTermSignal()) {
                case SIGSEGV:
                    output.push_back("Terminated with SIGSEGV\n");
                    break;
                case SIGABRT:
                    output.push_back("Terminated with SIGABRT\n");
                    break;
                case SIGKILL:
                    output.push_back("Terminated with SIGKILL\n");
                    break;
                default:
                    output.push_back("Exited normally\n");
                    break;
            }
            _subProcess = nullptr;
        }
    }

private:
    std::shared_ptr<SPType> _subProcess { nullptr };
    void runImpl(QueueType &messageQueue, JITCompiler::CompiledCode &compiledCode) {
        assert(!_subProcess);
        for (uint32_t i = 0; i != 10; ++i) {
            messageQueue.push({MarketPrice{i * 5, i}});
        }
    }
};




#endif //TESTPROJECT_FINANCETOYCONTEXTS_H
