#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Lex/PreprocessorOptions.h>
#include <clang/Driver/Driver.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Demangle/Demangle.h>
#include <llvm/IR/Mangler.h>

using namespace std;
using namespace clang;
using namespace llvm;

int main() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    constexpr auto testCodeFileName = "test.cpp";
    constexpr auto testCode = "class A {public: const static int x=5;}; int test() { return 2+A::x; }";

    // Prepare compilation arguments
    vector<const char *> args;
    args.push_back(testCodeFileName);

    // Prepare DiagnosticEngine
    DiagnosticOptions *DiagOpts = new DiagnosticOptions;
    TextDiagnosticPrinter *textDiagPrinter =
            new clang::TextDiagnosticPrinter(errs(),
                                             DiagOpts);
    IntrusiveRefCntPtr<clang::DiagnosticIDs> pDiagIDs;
    auto pDiagnosticsEngine =
            new DiagnosticsEngine(pDiagIDs,
                                  DiagOpts,
                                  textDiagPrinter);

    // Initialize CompilerInvocation
    auto CI = std::make_shared<CompilerInvocation>();
    CompilerInvocation::CreateFromArgs(*CI, ArrayRef(args), *pDiagnosticsEngine);

    // Map code filename to a memoryBuffer
    StringRef testCodeData(testCode);
    unique_ptr<MemoryBuffer> buffer = MemoryBuffer::getMemBufferCopy(testCodeData);

    // Create and initialize CompilerInstance
    CompilerInstance Clang;
    Clang.setInvocation(CI);
    Clang.createDiagnostics();

    auto fs = makeIntrusiveRefCnt<vfs::InMemoryFileSystem>();
    Clang.setFileManager(new clang::FileManager(clang::FileSystemOptions{}, fs));
    Clang.createSourceManager(Clang.getFileManager());
    fs->addFile(llvm::Twine(testCodeFileName), 0, std::move(buffer));


    const std::shared_ptr<clang::TargetOptions> targetOptions = std::make_shared<clang::TargetOptions>();
    targetOptions->Triple = LLVM_HOST_TRIPLE;
    cout << targetOptions->Triple << endl;
    TargetInfo *pTargetInfo = TargetInfo::CreateTargetInfo(*pDiagnosticsEngine,targetOptions);
    Clang.setTarget(pTargetInfo);

    // Create and execute action
    EmitLLVMOnlyAction *compilerAction = new EmitLLVMOnlyAction();
    if (!Clang.ExecuteAction(*compilerAction)) {
        return -1;
    }

    auto llvmModule = compilerAction->takeModule();

    if (!llvmModule) {
        cout << "No module" << endl;
        return -1;
    }

    std::string error;

    cout << "Module function names: " << endl;
    std::string targetName = "test()";
    std::string mangledTargetName;
    for (const auto &f  : llvmModule->getFunctionList()) {
        const auto demangled = llvm::demangle(f.getName().str());
        if (demangled == targetName) {
            mangledTargetName = f.getName().str();
        }
    }
    cout << "targetName: " << targetName << endl;
    cout << "mangledTargetName: " << mangledTargetName << endl;

    llvm::Function *otherTest = llvmModule->getFunction(mangledTargetName);
    if (otherTest) {
        cout << "Found in module with " << mangledTargetName << endl;
    }

    auto &moduleRef = *llvmModule;
    auto engine = llvm::EngineBuilder(std::move(llvmModule)).setEngineKind(llvm::EngineKind::Either).setErrorStr(&error).create();
    engine->finalizeObject();
    {
        std::string mangledTestName;
        raw_string_ostream mangledNameStream(mangledTestName);
        Mangler::getNameWithPrefix(mangledNameStream, "test()", engine->getDataLayout());
        std::cout << "engine Datalayout mangle: " << mangledTestName << endl;
    }
    {
        std::string mangledTestName;
        raw_string_ostream mangledNameStream(mangledTestName);
        Mangler::getNameWithPrefix(mangledNameStream, "test()", moduleRef.getDataLayout());
        std::cout << "module Datalayout mangle: " << mangledTestName << endl;
    }

    llvm::Function *testFn = engine->FindFunctionNamed(mangledTargetName);
    if (testFn) {
        cout << "Found in engine with " << mangledTargetName << endl;
        auto v = ArrayRef<GenericValue>();
        auto res = engine->runFunction(testFn, v);
        cout << *res.IntVal.getRawData() << endl;
    }

}
