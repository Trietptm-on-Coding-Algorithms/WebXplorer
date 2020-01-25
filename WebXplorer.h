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

#define USE_DANGEROUS_FUNCTIONS

#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>

// If we put this in the CMakeLists, IDA SDK will throw errors
#pragma GCC diagnostic error "-Wall"