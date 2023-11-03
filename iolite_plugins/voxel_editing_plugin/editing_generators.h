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
#include "lua_plugin.h"

//----------------------------------------------------------------------------//
namespace editing_generators
{
namespace tree_generator
{
// Tree generator based on
// https://nickmcd.me/2020/10/19/transport-oriented-growth-and-procedural-trees/

//----------------------------------------------------------------------------//
using branch_index_t = uint32_t;

//----------------------------------------------------------------------------//
struct branch_t
{
  branch_index_t parent{(uint32_t)-1}, child_a{(uint32_t)-1},
      child_b{(uint32_t)-1};

  uint32_t depth{0u};
  io_vec3_t dir{0.0f, 1.0f, 0.0f};
  io_float32_t length{0.0f}, radius{0.0f}, area{0.1f}, ratio{0.0f},
      spread{0.0f}, split_size{0.0f};
};

//----------------------------------------------------------------------------//
struct context_t
{
  std::vector<branch_t> branches{1u};
  uint32_t next_branch_index{0u};
  branch_index_t root{(uint32_t)-1};
};

//----------------------------------------------------------------------------//
static auto branch(context_t& context, io_float32_t ratio, io_float32_t spread,
                   io_float32_t split_size) -> branch_index_t
{
  if (context.next_branch_index >= context.branches.size())
    context.branches.resize(context.branches.size() * 2u);

  const branch_index_t branch_idx = context.next_branch_index++;
  branch_t& b = context.branches[branch_idx];
  {
    b.ratio = ratio;
    b.spread = spread;
    b.split_size = split_size;
  };
  return branch_idx;
}

//----------------------------------------------------------------------------//
static auto branch_from_parent(context_t& context,
                               branch_index_t parent_branch_index)
    -> branch_index_t
{
  if (context.next_branch_index >= context.branches.size())
    context.branches.resize(context.branches.size() * 2u);

  const branch_index_t branch_idx = context.next_branch_index++;
  branch_t& b = context.branches[branch_idx];
  {
    b.ratio = context.branches[parent_branch_index].ratio;
    b.spread = context.branches[parent_branch_index].spread;
    b.split_size = context.branches[parent_branch_index].split_size;
    b.depth = context.branches[parent_branch_index].depth + 1u;
    b.parent = parent_branch_index;
  }
  return branch_idx;
}

//----------------------------------------------------------------------------//
static auto is_leaf(const branch_t& branch) -> bool
{
  return branch.child_a == (uint32_t)-1;
}

//----------------------------------------------------------------------------//
static auto is_root(const branch_t& branch) -> bool
{
  return branch.parent == (uint32_t)-1;
}

//----------------------------------------------------------------------------//
static auto leaf_average(context_t& c, branch_index_t base_branch_index,
                         branch_index_t branch_index) -> io_vec3_t
{
  const auto a = io_vec3_t{
      c.branches[branch_index].length * c.branches[branch_index].dir.x,
      c.branches[branch_index].length * c.branches[branch_index].dir.y,
      c.branches[branch_index].length * c.branches[branch_index].dir.z};

  if (is_leaf(c.branches[branch_index]))
    return a;

  io_float32_t s = c.branches[base_branch_index].ratio;

  auto b = io_cvt(
      leaf_average(c, base_branch_index, c.branches[branch_index].child_a));
  b *= s;

  s = 1.0f - s;
  auto d = io_cvt(
      leaf_average(c, base_branch_index, c.branches[branch_index].child_b));
  d *= s;

  return {a.x + b.x + d.x, a.y + b.y + d.y, a.z + b.z + d.z};
}

//----------------------------------------------------------------------------//
static uint32_t local_depth = 2u;
static io_float32_t split_decay = 0.01f;
static io_float32_t directedness = 0.5f;

//----------------------------------------------------------------------------//
static auto leaf_density(context_t& c, branch_index_t branch_index) -> io_vec3_t
{
  const auto r = glm::normalize(glm::vec3{common::rand_float(-1.0f, 1.0f),
                                          common::rand_float(-1.0f, 1.0f),
                                          common::rand_float(-1.0f, 1.0f)});

  if ((uint32_t)-1 == c.branches[branch_index].parent)
    return io_cvt(r);

  auto b = branch_index;
  int32_t search_depth = local_depth;
  auto rel = glm::vec3{0.0f, 0.0f, 0.0f};

  while (b != (uint32_t)-1 && search_depth-- >= 0)
  {
    rel += c.branches[b].length * io_cvt(c.branches[b].dir);
    b = c.branches[b].parent;
  }

  const auto avg = io_cvt(leaf_average(c, branch_index, branch_index));
  const auto dir_0 = directedness * glm::normalize(avg - rel);
  const auto dir_1 = (1.0f - directedness) * r;

  return io_cvt(dir_0 + dir_1);
}

//----------------------------------------------------------------------------//
static auto split(context_t& c, branch_index_t branch_index)
{
  c.branches[branch_index].child_a = branch_from_parent(c, branch_index);
  c.branches[branch_index].child_b = branch_from_parent(c, branch_index);

  const auto d = leaf_density(c, branch_index);
  const auto n = glm::normalize(
      glm::cross(io_cvt(c.branches[branch_index].dir), io_cvt(d)));
  const auto m = -1.0f * n;

  const float flip = common::rand_float(0.0f, 1.0f) > 0.5f ? -1.0f : 1.0f;
  const auto d0 = flip * c.branches[branch_index].spread * n;
  const auto d1 = flip * c.branches[branch_index].spread * m;

  c.branches[c.branches[branch_index].child_a].dir =
      io_cvt(glm::normalize(glm::mix(d0, io_cvt(c.branches[branch_index].dir),
                                     c.branches[branch_index].ratio)));
  c.branches[c.branches[branch_index].child_b].dir =
      io_cvt(glm::normalize(glm::mix(d1, io_cvt(c.branches[branch_index].dir),
                                     1.0f - c.branches[branch_index].ratio)));
}

//----------------------------------------------------------------------------//
static auto grow(context_t& c, branch_index_t branch_index, io_float32_t feed)
{
  c.branches[branch_index].radius =
      glm::sqrt(c.branches[branch_index].area / glm::pi<float>());

  if (is_leaf(c.branches[branch_index]))
  {
    c.branches[branch_index].length =
        c.branches[branch_index].length + glm::pow(feed, 1.0f / 4.0f);
    feed = feed - glm::pow(feed, 1.0f / 4.0f) * c.branches[branch_index].area;
    c.branches[branch_index].area += feed / c.branches[branch_index].length;

    // Split?
    if (c.branches[branch_index].length >
        c.branches[branch_index].split_size *
            glm::exp(-split_decay * c.branches[branch_index].depth))
    {
      return split(c, branch_index);
    }
  }
  else
  {
    const auto aa = c.branches[c.branches[branch_index].child_a].area;
    const auto ab = c.branches[c.branches[branch_index].child_b].area;

    const auto pass = (aa + ab) / (aa + ab + c.branches[branch_index].area);
    c.branches[branch_index].area +=
        pass * feed / c.branches[branch_index].length;
    feed *= 1.0f - pass;

    if (feed < 1e-4f)
      return;

    grow(c, c.branches[branch_index].child_a,
         feed * c.branches[branch_index].ratio);
    grow(c, c.branches[branch_index].child_b,
         feed * (1.0f - c.branches[branch_index].ratio));
  }
}

//----------------------------------------------------------------------------//
static auto draw(context_t& c, branch_index_t branch_index,
                 const io_vec3_t& prev_pos) -> void
{
  const auto next_pos =
      io_cvt(prev_pos) +
      c.branches[branch_index].length * io_cvt(c.branches[branch_index].dir);
  const glm::vec4 color = glm::mix(
      glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
      glm::clamp(c.branches[branch_index].radius, 0.0f, 1.0f));

  io_debug_geometry->draw_line(prev_pos, io_cvt(next_pos), io_cvt(color),
                               false);

  if (!is_leaf(c.branches[branch_index]))
  {
    draw(c, c.branches[branch_index].child_a, io_cvt(next_pos));
    draw(c, c.branches[branch_index].child_b, io_cvt(next_pos));
  }
}

//----------------------------------------------------------------------------//
static auto init(context_t& context, io_float32_t ratio, io_float32_t spread,
                 io_float32_t split_size)
{
  context = {};
  context.root = branch(context, ratio, spread, split_size);
}

//----------------------------------------------------------------------------//
static auto show_ui(bool& tree_generator_visible)
{
  if (!tree_generator_visible)
    return;

  if (ImGui::Begin("Tree Generator", &tree_generator_visible))
  {
    using namespace editing_generators;

    static bool active = false;
    static editing_generators::tree_generator::context_t context;

    static float ratio = 0.6f;
    static float spread = 0.45f;
    static float feed = 0.1f;
    static float split_size = 0.1f;

    ImGui::DragFloat("Ratio", &ratio);
    ImGui::DragFloat("Spread", &spread);
    ImGui::DragFloat("Feed", &feed);
    ImGui::DragFloat("Split Size", &split_size);

    ImGui::Spacing();

    if (ImGui::Button(active ? "Stop Generating" : "Start Generating"))
    {
      active = !active;
      if (active)
        init(context, ratio, spread, split_size);
    }

    if (active)
      grow(context, context.root, feed);

    if ((uint32_t)-1 != context.root)
      draw(context, context.root, {0.0f, 0.0f, 0.0f});
  }
  ImGui::End();
}
} // namespace tree_generator

} // namespace editing_generators