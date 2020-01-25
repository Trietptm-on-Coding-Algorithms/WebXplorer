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

#pragma once

#include <WAVM/Runtime/Runtime.h>
#include <WAVM/IR/Module.h>
#include <WAVM/RuntimeABI/RuntimeABI.h>

#include <map>
#include <string>

#include "WebXplorer.h"

using namespace WAVM;
using namespace WAVM::IR;
using namespace WAVM::Runtime;

class WASMLinker {
private:
    IR::Module& module;
    std::map<std::string, std::map<std::string, Function*>> functions;

public:
    explicit WASMLinker(IR::Module& module) : module(module) {

    }

    ImportBindings GetImportBindings() {
        ImportBindings import_bindings;

        for (auto& import : module.functions.imports) {
            if (functions.count(import.moduleName)) {
                if (functions[import.moduleName].count(import.exportName)) {
                    Function* f = functions[import.moduleName][import.exportName];
                    if (module.types[import.type.index] == f->encodedType) {
                        import_bindings.push_back(asObject(f));
                        continue;
                    }
                }
            }

            // The WAVM runtime will throw an exception, so we do not need to throw an error here
            msg("WebAssembly (import \"%s\" \"%s\") not found.\n", import.moduleName.c_str(), import.exportName.c_str());
        }

        return import_bindings;
    }

    void Add(std::string const& module_name, std::string const& name, Function* function) {
        functions[module_name][name] = function;
    }
};