/*
 * This file is part of WebXplorer.
 *
 * WebXplorer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * WebXplorer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WebXplorer.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <fstream>
#include <vector>

#include <WAVM/IR/Types.h>
#include <WAVM/Runtime/Intrinsics.h>
#include <WAVM/Runtime/Runtime.h>
#include <auto.hpp>
#include "WAVM/Lib/Runtime/RuntimePrivate.h"

#include "WebXplorer.h"
#include "WASMLinker.h"

using namespace WAVM;
using namespace WAVM::IR;
using namespace WAVM::Runtime;

// AssemblyScript CommonHeader, must be 16 bytes in size
extern "C" struct CommonHeader {
    U32 mmInfo;
    U32 gcInfo;
    U32 rtld;
    U32 rtSize;
};
static_assert(sizeof(CommonHeader) == 16);

WAVM_DEFINE_INTRINSIC_MODULE(env)
WAVM_DEFINE_INTRINSIC_FUNCTION(env, "abort", void, env_abort, I32 message, I32 file, I32 line, I32 column) {
    msg("WebAssembly program aborted.\n");
    throwException(Runtime::ExceptionTypes::calledAbort);
}

WAVM_DEFINE_INTRINSIC_MODULE(X)
WAVM_DEFINE_INTRINSIC_FUNCTION(X, "msg", void, X_msg, I32 message) {
    auto* memory = (Memory*) Runtime::getUserData(Runtime::getCompartmentFromContextRuntimeData(contextRuntimeData));
    auto* header = reinterpret_cast<CommonHeader *>(memoryArrayPtr<U8>(memory, message - sizeof(CommonHeader), sizeof(CommonHeader)));
    char* contents = reinterpret_cast<char*>(memoryArrayPtr<U8>(memory, message, header->rtSize));
    msg("%s\n", contents);
}

WAVM_DEFINE_INTRINSIC_FUNCTION(X, "get_next_func", I64, X_get_next_func, I64 ea) {
    static_assert(sizeof(ea_t) == sizeof(I64));

    func_t* f = get_next_func(ea);
    if (f == nullptr) {
        return -1;
    } else {
        return static_cast<I64>(f->start_ea);
    }
}

WAVM_DEFINE_INTRINSIC_FUNCTION(X, "auto_wait", I32, X_auto_wait) {
    return auto_wait();
}

bool ExecuteWASM(const U8* data, size_t data_size) {
    ModuleRef module;

    if (!loadBinaryModule(data, data_size, module)) {
        return false;
    }

    GCPointer<Compartment> compartment = createCompartment();
    Context* context = createContext(compartment);

    // AssemblyScript
    Instance* env = WAVM::Intrinsics::instantiateModule(
            compartment, {WAVM_INTRINSIC_MODULE_REF(env)}, "env");
    Function* abort_instr = getTypedInstanceExport(env, "abort", FunctionType({}, {ValueType::i32, ValueType::i32, ValueType::i32, ValueType::i32}));

    // IDC
    Instance* X = WAVM::Intrinsics::instantiateModule(
            compartment, {WAVM_INTRINSIC_MODULE_REF(X)}, "X");

    Function* msg_instr = getTypedInstanceExport(X, "msg", FunctionType({}, {ValueType::i32}));
    Function* get_next_func_instr = getTypedInstanceExport(X, "get_next_func", FunctionType({ValueType::i64}, {ValueType::i64}));
    Function* auto_wait_instr = getTypedInstanceExport(X, "auto_wait", FunctionType({ValueType::i32}, {}));

    WASMLinker linker (module->ir);
    linker.Add("", "msg", msg_instr);
    linker.Add("", "get_next_func", get_next_func_instr);
    linker.Add("", "auto_wait", auto_wait_instr);
    linker.Add("env", "abort", abort_instr);

    Instance* instance = instantiateModule(compartment, module, linker.GetImportBindings(), "env");

    Memory* memory = asMemoryNullable(getInstanceExport(instance, "memory"));
    setUserData(compartment, memory);

    Function* main_instr = getTypedInstanceExport(instance, "main", FunctionType({ValueType::i32}, {}));
    UntaggedValue code;
    invokeFunction(context, main_instr, FunctionType({ValueType::i32}, {}), nullptr, &code);

    tryCollectCompartment(std::move(compartment));

    return true;
}

int idaapi init() {
    // FIXME(keegan) support multiple file options, -Owasm:one.wasm -Owasm:two.wasm is one.wasm:two.wasm
    const char* path = get_plugin_options("wasm");

    if (path == nullptr) {
        msg("No path specified.\n");
        return PLUGIN_SKIP;
    }

    std::ifstream f (path, std::ios::binary | std::ios::ate);

    if (!f.good()) {
        msg("Failed to read specified file.\n");
        return PLUGIN_SKIP;
    }

    std::streamsize size = f.tellg();
    f.seekg(0, std::ios::beg);

    U8* data = new U8[size];
    if (f.read(reinterpret_cast<char*>(data), size)) {
        if (!ExecuteWASM(data, size)) {
            msg("Failed to execute specified file.\n");
            return PLUGIN_SKIP;
        }
    } else {
        msg("Failed to read specified file.\n");
        return PLUGIN_SKIP;
    }
    delete[] data;

    msg("Executed specified file.\n");

    return PLUGIN_KEEP;
}

bool idaapi run(size_t) {
    // Never called because the plugin is under PLUGIN_HIDE
    return true;
}

void idaapi term() {

}

plugin_t PLUGIN = {
    IDP_INTERFACE_VERSION,
    PLUGIN_HIDE,
    &init,
    &term,
    &run,
    "WebXplorer",
    "IDA WASM scripting environment",
    "",
    nullptr, // plugin hotkey, e.g. "Ctrl-Shift-A"
};