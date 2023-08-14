// MIT License
//
// Copyright (c) 2023 Missing Deadlines (Benjamin Wrensch)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "common.h"
#include "sparse_volume.h"

//----------------------------------------------------------------------------//
namespace undo_redo
{

//----------------------------------------------------------------------------//
struct undo_redo_stack_t
{
  inline undo_redo_stack_t(io_ref_t shape) : shape(shape) {}

  inline auto can_undo() const -> bool { return stack_index >= 0; }
  inline auto can_redo() const -> bool
  {
    return stack_index < (int32_t)stack.size() - 1;
  }

  inline void push(const sparse_volume_t& change)
  {
    // Cut off past history
    stack.resize(++stack_index);
    stack.emplace_back(change);
  }

  inline void undo(io_ref_t shape)
  {
    // Fetch current state
    auto curr = stack[stack_index];
    curr.update_from_shape(shape);
    // Apply the previous state
    stack[stack_index].apply(shape);
    // Store the current state
    stack[stack_index--] = std::move(curr);
  }

  inline void redo(io_ref_t shape)
  {
    // Fetch the current state
    auto prev = stack[++stack_index];
    prev.update_from_shape(shape);
    // Apply the previous state
    stack[stack_index].apply(shape);
    // Store the current state
    stack[stack_index] = std::move(prev);
  }

  io_ref_t shape;
  int32_t stack_index{-1};
  std::vector<sparse_volume_t> stack;
};

//----------------------------------------------------------------------------//
static std::vector<undo_redo_stack_t> undo_redo_stacks;

//----------------------------------------------------------------------------//
inline static auto find_or_create_stack_for_shape(io_ref_t shape)
    -> undo_redo_stack_t&
{
  for (auto& s : undo_redo_stacks)
  {
    if (s.shape.internal == shape.internal)
      return s;
  }

  undo_redo_stacks.push_back(shape);
  return undo_redo_stacks.back();
}

//----------------------------------------------------------------------------//
inline static auto can_undo(io_ref_t shape) -> bool
{
  auto& s = find_or_create_stack_for_shape(shape);
  return s.can_undo();
}

//----------------------------------------------------------------------------//
inline static auto can_redo(io_ref_t shape) -> bool
{
  auto& s = find_or_create_stack_for_shape(shape);
  return s.can_redo();
}

//----------------------------------------------------------------------------//
inline static void push_change(io_ref_t shape, const sparse_volume_t& change)
{
  auto& s = find_or_create_stack_for_shape(shape);
  s.push(change);
}

//----------------------------------------------------------------------------//
inline static void undo(io_ref_t shape)
{
  auto& s = find_or_create_stack_for_shape(shape);
  s.undo(shape);
}

//----------------------------------------------------------------------------//
void redo(io_ref_t shape)
{
  auto& s = find_or_create_stack_for_shape(shape);
  s.redo(shape);
}
} // namespace undo_redo