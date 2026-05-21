// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include "codegen_llvm/llvm_ir_emitter.h"

#include <memory>
#include <utility>

#include "base/debug/check.h"
#include "base/debug/dlog.h"
#include "base/debug/fatal.h"
#include "build/build_config.h"
#include "ir/block.h"
#include "ir/common.h"
#include "ir/external_function.h"
#include "ir/function.h"
#include "ir/instruction.h"
#include "ir/opcode.h"
#include "ir/operand.h"
#include "ir/storage.h"
#include "ir/type.h"
#include "ir/type_util.h"
#include "llvm/IR/ConstantFolder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/IRBuilderFolder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#if BUILD_FLAG(IS_DEBUG)
#include "ir/formatter.h"  // IWYU pragma: keep
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#endif

namespace codegen_llvm {

LlvmIrEmitter::LlvmIrEmitter(llvm::Module* module,
                             std::unique_ptr<ir::Storage> storage,
                             str::StringInterner* interner)
    : module_(module),
      storage_(std::move(storage)),
      builder_(std::make_unique<IRBuilder>(module_->getContext())),
      interner_(interner) {}

inline void LlvmIrEmitter::init_value_map() {
  functions_.resize(storage_->functions().size(), nullptr);
  registers_.resize(storage_->registers().size(), nullptr);
  blocks_.resize(storage_->blocks().size(), nullptr);
  immutables_.resize(storage_->immutables().size(), nullptr);
  external_functions_.resize(storage_->external_functions().size(), nullptr);

  DCHECK_EQ(functions_.size(), storage_->functions().size());
  DCHECK_EQ(registers_.size(), storage_->registers().size());
  DCHECK_EQ(blocks_.size(), storage_->blocks().size());
  DCHECK_EQ(immutables_.size(), storage_->immutables().size());
  DCHECK_EQ(external_functions_.size(), storage_->external_functions().size());
}

inline void LlvmIrEmitter::check_state() {
  DCHECK_MSG(storage_, "IR Storage is null");
  DCHECK_MSG(module_, "LLVM Module is null");
  DCHECK_MSG(builder_, "LLVM IR Builder is null");
  DCHECK_MSG(interner_, "String Interner is null");
}

inline llvm::Type* LlvmIrEmitter::type(ir::Type type) const {
  using T = ir::Type;

  switch (type) {
    case T::Void: return builder_->getVoidTy();
    case T::I1: return builder_->getInt1Ty();
    case T::I8: return builder_->getInt8Ty();
    case T::I16: return builder_->getInt16Ty();
    case T::I32: return builder_->getInt32Ty();
    case T::I64: return builder_->getInt64Ty();
    // case T::I128: return builder_->getInt128Ty();
    case T::F32: return builder_->getFloatTy();
    case T::F64: return builder_->getDoubleTy();
    case T::Ptr: return builder_->getPtrTy();
    case T::Ref: return builder_->getPtrTy();
    case T::MutRef: return builder_->getPtrTy();
    default: {
      DLOG("unsupported type: {}", type);
      UNREACHABLE();
    }
  }
}

void LlvmIrEmitter::emit() && noexcept {
  check_state();

  init_value_map();
  setup_immutables();
  setup_external_functions();

  // PERF: Consider run this process concurrently.
  for (ir::FunctionId function_id(0);
       function_id.id < storage_->functions().size(); ++function_id) {
    const ir::Function& function = storage_->functions()[function_id];

    llvm::Function* llvm_function = create_function(function.meta);
    add_function(function_id, llvm_function);
    emit_function(llvm_function, function);
  }

#if BUILD_FLAG(IS_DEBUG)
  if (llvm::verifyModule(*module_, &llvm::errs())) [[unlikely]] {
    DLOG("LLVM verify module failed");
    module_->print(llvm::errs(), nullptr);
    UNREACHABLE();
  }
#endif
}

void LlvmIrEmitter::emit_function(llvm::Function* llvm_function,
                                  const ir::Function& function) {
  check_state();

  // 2 pass block emission(block declare -> blck define)
  for (ir::BlockId block_id = function.head_id; block_id < function.end();
       ++block_id) {
    llvm::BasicBlock* llvm_block =
        llvm::BasicBlock::Create(module_->getContext(), "", llvm_function);
    add_block(block_id, llvm_block);
  }
  for (ir::BlockId block_id = function.head_id; block_id < function.end();
       ++block_id) {
    const ir::Block& block = storage_->blocks()[block_id];
    llvm::BasicBlock* llvm_block = blocks_[block_id];
    builder_->SetInsertPoint(llvm_block);
    emit_block(block);
  }

#if BUILD_FLAG(IS_DEBUG)
  if (llvm::verifyFunction(*llvm_function, &llvm::errs())) [[unlikely]] {
    DLOG("LLVM verify function failed");
    llvm_function->viewCFG();
    llvm_function->print(llvm::errs());
    UNREACHABLE();
  }
#endif
}

void LlvmIrEmitter::emit_block(const ir::Block& block) {
  check_state();

  for (ir::InstructionId instr_id = block.head_id; instr_id < block.end();
       ++instr_id) {
    const ir::Instruction& instr = storage_->instructions()[instr_id];
    emit_instruction(instr);
  }
}

void LlvmIrEmitter::emit_instruction(const ir::Instruction& instr) {
  check_state();

  const ir::Instruction& i = instr;

  using Op = ir::Opcode;
  switch (i.op) {
    case Op::Noop: {
      // no-operation
      break;
    }
    case Op::Alloca: {
      add_register(i.dst, builder_->CreateAlloca(type(i.lhs.type),
                                                 resolve_operand_value(i.lhs)));
      break;
    }
    case Op::Load: {
      add_register(i.dst, builder_->CreateLoad(type(i.lhs.type),
                                               resolve_operand_value(i.lhs)));
      break;
    }
    case Op::Store: {
      add_register(i.dst,
                   builder_->CreateStore(resolve_operand_value(i.lhs),
                                         resolve_operand_value(i.rhs.op)));
      break;
    }
      // GetElementPtr
      // ExtractValue,
      // InsertValue,

    case Op::IntAdd: {
      add_register(i.dst, builder_->CreateAdd(resolve_operand_value(i.lhs),
                                              resolve_operand_value(i.rhs.op)));
      break;
    }
    case Op::IntSub: {
      add_register(i.dst, builder_->CreateSub(resolve_operand_value(i.lhs),
                                              resolve_operand_value(i.rhs.op)));
      break;
    }
    case Op::IntMul: {
      add_register(i.dst, builder_->CreateMul(resolve_operand_value(i.lhs),
                                              resolve_operand_value(i.rhs.op)));
      break;
    }
      // IntDiv,   // Signed devision
      // UintDiv,  // Unsigned devision
      // IntRem,   // Signed remainder
      // UintRem,  // Unsigned remainder

    case Op::And: {
      add_register(i.dst, builder_->CreateAnd(resolve_operand_value(i.lhs),
                                              resolve_operand_value(i.rhs.op)));
      break;
    }
    case Op::Or: {
      add_register(i.dst, builder_->CreateOr(resolve_operand_value(i.lhs),
                                             resolve_operand_value(i.rhs.op)));
      break;
    }
    case Op::Xor: {
      add_register(i.dst, builder_->CreateXor(resolve_operand_value(i.lhs),
                                              resolve_operand_value(i.rhs.op)));
      break;
    }
    case Op::ShiftLeft: {
      add_register(i.dst, builder_->CreateShl(resolve_operand_value(i.lhs),
                                              resolve_operand_value(i.rhs.op)));
      break;
    }
    case Op::ArithmeticShiftRight: {
      add_register(i.dst,
                   builder_->CreateAShr(resolve_operand_value(i.lhs),
                                        resolve_operand_value(i.rhs.op)));
      break;
    }
    case Op::LogicalShiftRight: {
      add_register(i.dst,
                   builder_->CreateLShr(resolve_operand_value(i.lhs),
                                        resolve_operand_value(i.rhs.op)));
      break;
    }
    case Op::Not: {
      add_register(i.dst, builder_->CreateNot(resolve_operand_value(i.lhs)));
      break;
    }
    // BitReverse,

    // Eq,
    // Ne,
    // Le,
    // Lt,
    // Ge,
    // Gt,

    // TypeCast,

    // Select,

    // Br,
    // CondBr,
    // Switch,
    case Op::Call: {
      // TODO: Support not only one argument
      // builder_->CreateCall(resolve_operand_function(i.lhs),
      //                      {resolve_operand_value(i.rhs.op)});

      llvm::Constant* test_str = builder_->CreateGlobalString("Hello, World");
      builder_->CreateCall(resolve_operand_function(i.lhs), {test_str});
      break;
    }
    case Op::Ret: {
      builder_->CreateRet(resolve_operand_value(i.lhs));
      break;
    }
    case Op::Unreachable: {
      builder_->CreateUnreachable();
      break;
    }

    // AtomicLoad,
    // AtomicStore,
    // AtomicRmw,
    // AtomicCompareExchange,
    // Fence,

    // Move,
    // Drop,
    // BorrowBegin,
    // BorrowEnd,

    // VecSplat,
    // VecExtract,
    // VecInsert,
    // VecReduce,
    default: {
#if BUILD_FLAG(IS_DEBUG)
      DLOG("Unknown opcode found: {}", i.op);
#endif
      UNREACHABLE();
      break;
    }
  }
}

llvm::Function* LlvmIrEmitter::create_function(
    const ir::FunctionMeta& function_meta) const {
  llvm::SmallVector<llvm::Type*, ir::kFunctionParameterTypesSooThreshold>
      parameter_types;
  parameter_types.reserve(function_meta.param_types_count);

  if (function_meta.param_types_count <=
      ir::kFunctionParameterTypesSooThreshold) [[likely]] {
    for (usize i = 0; i < function_meta.param_types_count; ++i) {
      parameter_types.emplace_back(type(function_meta.param_types.soo_buf[i]));
    }
  } else {
    using Id = ir::ParameterTypeId;
    const Id begin = function_meta.param_types.storage_id;
    const Id end = Id{begin.id + function_meta.param_types_count};
    for (Id id = begin; id < end; ++id) {
      parameter_types.emplace_back(type(storage_->parameter_types()[id]));
    }
  }

  llvm::FunctionType* func_type = llvm::FunctionType::get(
      type(function_meta.return_type),
      llvm::ArrayRef<llvm::Type*>(parameter_types), false);

  const std::string_view func_name = interner_->pool().get(function_meta.name);

  llvm::Function* llvm_function = llvm::Function::Create(
      func_type, llvm::Function::ExternalLinkage, func_name, module_);
  DCHECK_MSG(llvm_function, "Failed to create LLVM function");
  return llvm_function;
}

llvm::Value* LlvmIrEmitter::resolve_operand_value(const ir::Operand& op) const {
  switch (op.tag) {
    case ir::OperandTag::Register: return registers_[op.data.register_id];
    case ir::OperandTag::Block: return blocks_[op.data.block_id];
    case ir::OperandTag::Immutable: return immutables_[op.data.immutable_id];

    default:
      DLOG("Unknown operand tag found while resolving operand value.");
      UNREACHABLE();
  }
}

llvm::Function* LlvmIrEmitter::resolve_operand_function(
    const ir::Operand& op) const {
  switch (op.tag) {
    case ir::OperandTag::Function: return functions_[(op.data.function_id)];
    case ir::OperandTag::ExternalFunction:
      return external_functions_[(op.data.external_function_id)];
    default:
      DLOG("Unknown operand tag found while resolving operand function.");
      UNREACHABLE();
  }
}

inline void LlvmIrEmitter::add_function(ir::FunctionId id,
                                        llvm::Function* function) {
  DCHECK_MSG(function, "Function is null");
  functions_[id] = function;
}
inline void LlvmIrEmitter::add_register(ir::RegisterId id, llvm::Value* value) {
  DCHECK_MSG(value, "Value is null");
  registers_[id] = value;
}
inline void LlvmIrEmitter::add_block(ir::BlockId id, llvm::BasicBlock* block) {
  DCHECK_MSG(block, "Block is null");
  blocks_[id] = block;
}
inline void LlvmIrEmitter::add_immutable(ir::ImmutableId id,
                                         llvm::Constant* immutable) {
  DCHECK_MSG(immutable, "Immutable is null");
  immutables_[id] = immutable;
}
inline void LlvmIrEmitter::add_external_function(ir::ExternalFunctionId id,
                                                 llvm::Function* ex_function) {
  DCHECK_MSG(ex_function, "External function is null");
  external_functions_[id] = ex_function;
}

void LlvmIrEmitter::setup_immutables() {
  for (ir::ImmutableId immutable_id(0);
       immutable_id.id < storage_->immutables().size(); ++immutable_id) {
    const ir::Immutable& immutable = storage_->immutables()[immutable_id];

    if (ir::is_integer_type(immutable.type)) {
      llvm::Constant* c = llvm::ConstantInt::get(
          type(immutable.type), immutable.as_u64_integer(),
          ir::is_signed_integer_type(immutable.type));
      DCHECK_MSG(c, "Failed to get int constant from LLVM");

      add_immutable(immutable_id, c);
    } else if (ir::is_float_type(immutable.type)) {
      llvm::Constant* c =
          llvm::ConstantFP::get(type(immutable.type), immutable.as_f64_fp());
      DCHECK_MSG(c, "Failed to get fp constant from LLVM");

      add_immutable(immutable_id, c);
    } else {
      DCHECK_MSG(false, "Currently unsupported type found");
      UNREACHABLE();
    }
  }
}

void LlvmIrEmitter::setup_external_functions() {
  for (ir::ExternalFunctionId function_id(0);
       function_id.id < storage_->external_functions().size(); ++function_id) {
    const ir::ExternalFunction& function =
        storage_->external_functions()[function_id];

    add_external_function(function_id, create_function(function.meta));
  }
}

}  // namespace codegen_llvm
