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
#include <llvm/Support/TargetSelect.h>

using namespace std;
using namespace clang;
using namespace llvm;

int main() {

    constexpr auto testCodeFileName = "test.cpp";
    constexpr auto testCode = "int test() { return 2+2; }";

    InitializeAllTargetMCs();
/*    InitializeAllAsmPrinters();

    // Prepare compilation arguments
    vector<const char *> args;
    args.push_back(testCodeFileName);

    // Prepare DiagnosticEngine
    DiagnosticOptions DiagOpts;
    TextDiagnosticPrinter *textDiagPrinter =
            new clang::TextDiagnosticPrinter(errs(),
                                             &DiagOpts);
    IntrusiveRefCntPtr<clang::DiagnosticIDs> pDiagIDs;
    DiagnosticsEngine *pDiagnosticsEngine =
            new DiagnosticsEngine(pDiagIDs,
                                  &DiagOpts,
                                  textDiagPrinter);

    // Initialize CompilerInvocation
    auto CI = std::make_shared<CompilerInvocation>();
    CompilerInvocation::CreateFromArgs(*CI, ArrayRef(args), *pDiagnosticsEngine);

    // Map code filename to a memoryBuffer
    StringRef testCodeData(testCode);
    unique_ptr<MemoryBuffer> buffer = MemoryBuffer::getMemBufferCopy(testCodeData);
    CI->getPreprocessorOpts().addRemappedFile(testCodeFileName, buffer.get());


    // Create and initialize CompilerInstance
    CompilerInstance Clang;
    Clang.setInvocation(CI);
    Clang.createDiagnostics();

    // Set target (I guess I can initialize only the BPF target, but I don't know how)
    InitializeAllTargets();
    const std::shared_ptr<clang::TargetOptions> targetOptions = std::make_shared<clang::TargetOptions>();
    targetOptions->Triple = targetOptions->HostTriple;
    cout << targetOptions->Triple << endl;
    TargetInfo *pTargetInfo = TargetInfo::CreateTargetInfo(*pDiagnosticsEngine,targetOptions);
    Clang.setTarget(pTargetInfo);

    // Create and execute action
    // CodeGenAction *compilerAction = new EmitLLVMOnlyAction();
    CodeGenAction *compilerAction = new EmitAssemblyAction();
    Clang.ExecuteAction(*compilerAction);

    buffer.release();*/
}
