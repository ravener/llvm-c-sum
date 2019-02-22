#include <stdio.h>
#include <stdlib.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Analysis.h>

int main(int argc, char* argv[]) {
  if(argc != 3) {
    fprintf(stderr, "Usage: %s <x> <y>\n", argv[0]);
    return 1;
  }
  int x = atoi(argv[1]);
  int y = atoi(argv[2]);
  // Initialize module and builder.
  LLVMModuleRef module = LLVMModuleCreateWithName("main");
  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMTypeRef args[] = { LLVMInt32Type(), LLVMInt32Type() };
  LLVMTypeRef fnType = LLVMFunctionType(LLVMInt32Type(), args, 2, 0);
  LLVMValueRef fn = LLVMAddFunction(module, "sum", fnType);
  LLVMBasicBlockRef block = LLVMAppendBasicBlock(fn, "entry");
  LLVMPositionBuilderAtEnd(builder, block);
  // Get args and give them a name.
  LLVMValueRef one = LLVMGetParam(fn, 0);
  LLVMValueRef two = LLVMGetParam(fn, 1);
  LLVMSetValueName(one, "x");
  LLVMSetValueName(two, "y");
  LLVMValueRef results = LLVMBuildAdd(builder, one, two, "results");
  LLVMBuildRet(builder, results);
  char* err = NULL;
  LLVMVerifyModule(module, LLVMAbortProcessAction, &err);
  LLVMDisposeMessage(err);
  LLVMDumpModule(module);
  LLVMLinkInMCJIT();
  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  LLVMInitializeNativeAsmParser();
  LLVMExecutionEngineRef engine;
  if(LLVMCreateExecutionEngineForModule(&engine, module, &err)) {
    fprintf(stderr, "Failed to create ExecutionEngine: %s\n", err);
    LLVMDisposeMessage(err);
    return 1;
  }
  int (*sum)(int, int) = (int (*)(int, int)) LLVMGetFunctionAddress(engine, "sum");
  printf("%d + %d = %d\n", x, y, sum(x, y));
  // Cleanup
  LLVMDisposeExecutionEngine(engine);
  LLVMDisposeBuilder(builder);
  return 0;
}
