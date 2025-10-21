#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

namespace {

struct SkeletonPass : public PassInfoMixin<SkeletonPass> {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        // declare ext function in this module
        LLVMContext &Ctx = M.getContext();
        auto retType = Type::getVoidTy(Ctx);
        std::vector<Type*> paramTypes = {Type::getInt32Ty(Ctx)};
        auto logopType = FunctionType::get(retType, paramTypes, false);
        auto logop_function = M.getOrInsertFunction("logop", logopType);
        for (auto &F : M) {
            for (auto &B : F) {
                for (auto &I : B) {
                    if (auto *op = dyn_cast<BinaryOperator>(&I)) {
                        if (op->getOpcode() == Instruction::FDiv) {
                            errs() << "Found a floating point division: " << *op << "\n";
                            continue;
                        } else if (op->getOpcode() == Instruction::FSub) {
                            errs() << "Found a floating point subtraction: " << *op << "\n";
                            continue;
                        } else if (op->getOpcode() == Instruction::FAdd) {
                            errs() << "Found a floating point addition: " << *op << "\n";
                            continue;
                        } else if (op->getOpcode() == Instruction::FMul) {
                            errs() << "Found a floating point multiplication: " << *op << "\n";
                            continue;
                        }
                    } 
                    errs() << "Visiting instruction: " << I << "\n";
                    /*
                    if (auto *op = dyn_cast<BinaryOperator>(&I)) {
                        //errs() << "I saw a binary operator: " << *op << "\n";
                        
                        IRBuilder<> builder(op);
                        Value *lhs = op->getOperand(0);
                        Value *rhs = op->getOperand(1);
                        Value *mul = builder.CreateMul(lhs, rhs);

                        for (auto& U : op->uses()) {
                            User* user = U.getUser();  // A User is anything with operands.
                            user->setOperand(U.getOperandNo(), mul);
                            builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
                            Value *args[] = {op};
                            builder.CreateCall(logop_function, args);
                        }
                    }
                    */
                }
            }
        }
        return PreservedAnalyses::all();
    };
};

}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "Skeleton pass",
        .PluginVersion = "v0.1",
        .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                    MPM.addPass(SkeletonPass());
                });
        }
    };
}
