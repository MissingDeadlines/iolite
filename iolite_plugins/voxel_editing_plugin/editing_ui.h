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
#include "undo_redo.h"
#include "editing.h"
#include "editing_tools.h"

namespace editing_ui
{
//----------------------------------------------------------------------------//
ImVec4 to_imgui(const io_vec4_t v) { return *((ImVec4*)&v); }

//----------------------------------------------------------------------------//
void show_palette_index_picker(io_ref_t palette, palette_range_t& range,
                               ImVec2 button_size)
{
  ImGui::SetNextItemWidth(button_size.x);
  if (!io_base->ref_is_valid(palette) ||
      !io_resource_palette->base.is_alive(palette))
  {
    ImGui::ColorButton("###no_palette", ImVec4(0.5f, 0.5f, 0.5f, 1.0f), 0,
                       button_size);

    return;
  }

  const auto current_color =
      io_resource_palette->get_color(palette, range.index_start);

  bool should_scroll = false;
  if (ImGui::ColorButton("###color", to_imgui(current_color), 0, button_size))
  {
    ImGui::OpenPopup("palette_picker");
    should_scroll = true;
  }

  ImGui::SetNextWindowSize(ImVec2(-1, 300));
  if (ImGui::BeginPopup("palette_picker"))
  {
    for (uint32_t i = 0u; i < 255u; ++i)
    {
      ImGui::PushID(i);

      if (i % 4u != 0u)
        ImGui::SameLine();

      const bool is_current = i >= range.index_start && i <= range.index_end;
      if (is_current)
      {
        // Highlight picked color
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

        // Scroll to current once when the popup got opened
        if (should_scroll)
        {
          ImGui::SetScrollHereY();
          should_scroll = false;
        }
      }

      const auto color = io_resource_palette->get_color(palette, i);
      if (ImGui::ColorButton("###button", to_imgui(color)))
      {
        if (!ImGui::GetIO().KeyShift)
          range.index_start = range.index_end = i;
        else
          range.index_end = i;

        // Ensure start <= end
        if (range.index_start > range.index_end)
          std::swap(range.index_start, range.index_end);
      }

      if (is_current)
      {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
      }

      ImGui::PopID();
    }

    ImGui::EndPopup();
  }
}

// TODO: Move helpers like this to a shared header file
//----------------------------------------------------------------------------//
inline static auto calc_tb_button_size() -> ImVec2
{
  return ImVec2(ImGui::CalcTextSize(ICON_FA_ROTATE).x * 2.0f, 0);
}

//----------------------------------------------------------------------------//
inline static void toggle_button(const char* label, ImVec2 size, bool& state)
{
  const ImVec4& color_selected = ImGui::GetStyle().Colors[ImGuiCol_CheckMark];
  const bool initial_state = state;
  if (initial_state)
    ImGui::PushStyleColor(ImGuiCol_Text, color_selected);
  if (ImGui::Button(label, size))
    state = !state;
  if (initial_state)
    ImGui::PopStyleColor();
}

//----------------------------------------------------------------------------//
inline static void show_tooltip(const char* tooltip)
{
  if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort |
                           ImGuiHoveredFlags_AllowWhenDisabled))
  {
    ImGui::SetNextWindowSize(ImVec2(150, -1));
    if (ImGui::BeginTooltip())
    {
      ImGui::TextWrapped("%s", tooltip);
      ImGui::EndTooltip();
    }
  }
}

//----------------------------------------------------------------------------//
template <typename T, typename E>
static void toggle_button_group(const char* label, ImVec2 size, T& enum_state,
                                E enum_value)
{
  const ImVec4& color_selected = ImGui::GetStyle().Colors[ImGuiCol_CheckMark];
  const T initial_state = enum_state;
  if (initial_state == enum_value)
    ImGui::PushStyleColor(ImGuiCol_Text, color_selected);
  if (ImGui::Button(label, size))
    enum_state = enum_value;
  if (initial_state == enum_value)
    ImGui::PopStyleColor();
}

//----------------------------------------------------------------------------//
static editing_tools::tool_parameters_t current_tool_params;

//----------------------------------------------------------------------------//
void show_editing_toolbar()
{
  const auto tb_button_size = calc_tb_button_size();

  ImGui::SetNextWindowPos(
      ImVec2(ImGui::GetMainViewport()->Size.x * 0.25f + 50.0f, 50.0f),
      ImGuiCond_FirstUseEver);
  if (ImGui::Begin("###voxel_shape_toolbar", nullptr,
                   ImGuiWindowFlags_AlwaysAutoResize |
                       ImGuiWindowFlags_NoDocking))
  {
    io_ref_t shape = io_base->ref_invalid();
    io_ref_t entity = io_editor->get_first_selected_entity();
    if (io_base->ref_is_valid(entity))
      shape = io_component_voxel_shape->base.get_component_for_entity(entity);
    io_ref_t palette = io_base->ref_invalid();
    if (io_base->ref_is_valid(shape))
      palette = io_component_voxel_shape->get_palette(shape);

    static auto op = io_base->ref_invalid();

    // Validate current operator
    if (io_base->ref_is_valid(op) &&
        !io_component_voxel_shape->base.is_alive(op))
      op = io_base->ref_invalid();

    bool shape_changed = false;
    uint32_t group = 0u;

#define SAME_LINE_GROUP()                                                      \
  if (group++ % 3u != 0u)                                                      \
  ImGui::SameLine()
#define SAME_LINE_RESET() group = 0u

    SAME_LINE_RESET();

    // Color picker
    {
      ImGui::BeginDisabled(!io_base->ref_is_valid(palette));
      SAME_LINE_GROUP();
      show_palette_index_picker(palette, current_tool_params.palette_range,
                                tb_button_size);
      show_tooltip(
          "The color or color range for the editing operations. Press "
          "[SHIFT] while picking a color to select a range of colors.");
      ImGui::EndDisabled();
    }

    // Undo/redo
    {
      ImGui::BeginDisabled(!io_base->ref_is_valid(shape) ||
                           !undo_redo::can_undo(shape));
      SAME_LINE_GROUP();
      if (ImGui::Button(ICON_FA_ROTATE_LEFT, tb_button_size))
        undo_redo::undo(shape);
      show_tooltip("Undo changes.");
      ImGui::EndDisabled();

      ImGui::BeginDisabled(!io_base->ref_is_valid(shape) ||
                           !undo_redo::can_redo(shape));
      SAME_LINE_GROUP();
      if (ImGui::Button(ICON_FA_ROTATE_RIGHT, tb_button_size))
        undo_redo::redo(shape);
      show_tooltip("Redo changes.");
      ImGui::EndDisabled();
    }

    ImGui::BeginDisabled(!io_base->ref_is_valid(shape));

    ImGui::Text("Tools");
    {
      ImGui::Spacing();

      SAME_LINE_RESET();
      SAME_LINE_GROUP();
      toggle_button_group(ICON_FA_CUBE "###tool_modify", tb_button_size,
                          current_tool_params.tool, editing_tools::tool_modify);
      show_tooltip("Modify: Place, paint, and erase voxels.");
      SAME_LINE_GROUP();
      toggle_button_group(ICON_FA_ARROWS_UP_TO_LINE "###tool_extrude",
                          tb_button_size, current_tool_params.tool,
                          editing_tools::tool_extrude);
      show_tooltip("Extrude: Extrude connected regions.");
      SAME_LINE_GROUP();
      toggle_button_group(ICON_FA_LEAF "###tool_grass", tb_button_size,
                          current_tool_params.tool, editing_tools::tool_grass);
      show_tooltip("Grass: Grass tool. Behaves similar to the extrude tool.");

      SAME_LINE_RESET();
      SAME_LINE_GROUP();
      toggle_button_group(ICON_FA_EYE_DROPPER "###tool_eyedropper",
                          tb_button_size, current_tool_params.tool,
                          editing_tools::tool_eyedropper);
      show_tooltip("Eyedropper: Quickly select the palette color of voxels.");
      SAME_LINE_GROUP();
      toggle_button_group(ICON_FA_SQUARE_DASHED "###tool_select_box",
                          tb_button_size, current_tool_params.tool,
                          editing_tools::tool_select_box);
      show_tooltip("Selection: Select voxels and perform local operations.");
      SAME_LINE_GROUP();
      toggle_button_group(ICON_FA_WAND_MAGIC_SPARKLES "###tool_select_wand",
                          tb_button_size, current_tool_params.tool,
                          editing_tools::tool_select_wand);
      show_tooltip("Wand: Select regions of voxels.");

      SAME_LINE_RESET();
      SAME_LINE_GROUP();
      toggle_button_group(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT "###tool_move",
                          tb_button_size, current_tool_params.tool,
                          editing_tools::tool_move);
      show_tooltip("Move: Move the current selection of voxels.");

      if (current_tool_params.tool == editing_tools::tool_modify ||
          current_tool_params.tool == editing_tools::tool_extrude ||
          current_tool_params.tool == editing_tools::tool_grass)
      {
        ImGui::Text("Mode");
        {
          ImGui::Spacing();

          SAME_LINE_RESET();
          SAME_LINE_GROUP();
          toggle_button_group(ICON_FA_CUBE, tb_button_size,
                              current_tool_params.placement_mode,
                              editing_tools::placement_mode_attach);
          show_tooltip("Attach: Attach new voxels.");
          SAME_LINE_GROUP();
          toggle_button_group(ICON_FA_PAINTBRUSH, tb_button_size,
                              current_tool_params.placement_mode,
                              editing_tools::placement_mode_paint);
          show_tooltip("Paint: Change the color of existing voxels.");
          SAME_LINE_GROUP();
          toggle_button_group(ICON_FA_ERASER, tb_button_size,
                              current_tool_params.placement_mode,
                              editing_tools::placement_mode_erase);
          show_tooltip("Erase: Remove solid voxels.");
        }
      }

      if (current_tool_params.tool == editing_tools::tool_modify)
      {
        ImGui::Text("Shape");
        {
          ImGui::Spacing();

          SAME_LINE_RESET();
          SAME_LINE_GROUP();
          toggle_button_group(ICON_FA_CUBE "###shape_voxel_box", tb_button_size,
                              current_tool_params.tool_shape,
                              editing_tools::tool_shape_voxel_box);
          show_tooltip("Voxel box shape.");
          SAME_LINE_GROUP();
          toggle_button_group(ICON_FA_CIRCLE "###shape_voxel_sphere",
                              tb_button_size, current_tool_params.tool_shape,
                              editing_tools::tool_shape_voxel_sphere);
          show_tooltip("Voxel sphere shape.");
          SAME_LINE_GROUP();
          toggle_button_group(ICON_FA_EXPAND "###shape_box", tb_button_size,
                              current_tool_params.tool_shape,
                              editing_tools::tool_shape_box);
          show_tooltip("Box shape.");

          ImGui::PushItemWidth(tb_button_size.x * 3.0f +
                               ImGui::GetStyle().ItemInnerSpacing.x * 4.0f);

          if (current_tool_params.tool_shape ==
              editing_tools::tool_shape_voxel_box)
          {
            ImGui::DragInt3("###extent_box", &current_tool_params.extent_box.x,
                            1.0f, 1, 32);
            show_tooltip("The extent of the box (in voxels).");
          }
          else if (current_tool_params.tool_shape ==
                   editing_tools::tool_shape_voxel_sphere)
          {
            ImGui::DragInt("###extent_sphere",
                           &current_tool_params.extent_sphere, 1.0f, 1, 32);
            show_tooltip("The extent of the sphere (in voxels).");
          }

          ImGui::PopItemWidth();
        }
      }

      if (current_tool_params.tool == editing_tools::tool_extrude ||
          current_tool_params.tool == editing_tools::tool_modify ||
          current_tool_params.tool == editing_tools::tool_grass ||
          current_tool_params.tool == editing_tools::tool_select_box)
      {
        ImGui::Text("Mirror");
        {
          ImGui::Spacing();

          SAME_LINE_RESET();
          SAME_LINE_GROUP();
          toggle_button(ICON_FA_CIRCLE_X, tb_button_size,
                        current_tool_params.mirror_x);
          show_tooltip("Mirror along the X axis.");
          SAME_LINE_GROUP();
          toggle_button(ICON_FA_CIRCLE_Y, tb_button_size,
                        current_tool_params.mirror_y);
          show_tooltip("Mirror along the Y axis.");
          SAME_LINE_GROUP();
          toggle_button(ICON_FA_CIRCLE_Z, tb_button_size,
                        current_tool_params.mirror_z);
          show_tooltip("Mirror along the Z axis.");
        }
      }

      if (current_tool_params.tool >= editing_tools::tool_select_begin &&
          current_tool_params.tool < editing_tools::tool_select_end)
      {
        ImGui::Text("Selection");
        {
          ImGui::Spacing();

          SAME_LINE_RESET();
          SAME_LINE_GROUP();
          if (ImGui::Button(ICON_FA_FILL_DRIP "###fill_selection",
                            tb_button_size))
          {
            const auto change = current_tool_params.selection.prepare_fill(
                shape, current_tool_params.palette_range);

            // Undo/redo
            {
              auto prev = change;
              prev.update_from_shape(shape);
              undo_redo::push_change(shape, prev);
            }

            change.apply(shape);
          }
          show_tooltip("Fill: Fills the selected voxels.");
          SAME_LINE_GROUP();
          if (ImGui::Button(ICON_FA_ERASER "###erase_selection",
                            tb_button_size))
          {
            const auto change =
                current_tool_params.selection.prepare_erase(shape);

            // Undo/redo
            {
              auto prev = change;
              prev.update_from_shape(shape);
              undo_redo::push_change(shape, prev);
            }

            change.apply(shape);
          }
          show_tooltip("Erase: Erases the selected voxels.");

          SAME_LINE_RESET();
          SAME_LINE_GROUP();
          if (ImGui::Button(ICON_FA_COPY "###copy_selection", tb_button_size))
          {
            current_tool_params.clipboard = current_tool_params.selection;
            current_tool_params.clipboard.update_from_shape(shape);
          }
          show_tooltip("Copy: Copies the selected voxels.");

          SAME_LINE_GROUP();
          if (ImGui::Button(ICON_FA_SCISSORS "###cut_selection",
                            tb_button_size))
          {
            current_tool_params.clipboard = current_tool_params.selection;
            current_tool_params.clipboard.update_from_shape(shape);

            const auto change =
                current_tool_params.clipboard.prepare_erase(shape);

            // Undo/redo
            {
              auto prev = change;
              prev.update_from_shape(shape);
              undo_redo::push_change(shape, prev);
            }

            change.apply(shape);
          }
          show_tooltip("Cut: Cuts the selected voxels.");

          SAME_LINE_GROUP();
          ImGui::BeginDisabled(current_tool_params.clipboard.empty());
          if (ImGui::Button(ICON_FA_PASTE "###paste_selection", tb_button_size))
          {
            // Undo/redo
            {
              auto prev = current_tool_params.clipboard;
              prev.update_from_shape(shape);
              undo_redo::push_change(shape, prev);
            }

            current_tool_params.clipboard.apply(shape);
          }
          show_tooltip("Paste: Pastes the previously copied/cut voxels.");
          ImGui::EndDisabled();
        }
      }
    }

    if (current_tool_params.tool == editing_tools::tool_select_wand)
    {
      ImGui::Text("Region");
      {
        ImGui::Spacing();

        SAME_LINE_RESET();
        SAME_LINE_GROUP();
        toggle_button_group(ICON_FA_PAINT_ROLLER, tb_button_size,
                            current_tool_params.region_type,
                            editing_tools::region_type_color);
        show_tooltip("Select all voxels with the same palette color.");
        SAME_LINE_GROUP();
        toggle_button_group(ICON_FA_CUBES, tb_button_size,
                            current_tool_params.region_type,
                            editing_tools::region_type_region);
        show_tooltip("Select voxels which are connected.");
        SAME_LINE_GROUP();
        toggle_button_group(ICON_FA_FILL, tb_button_size,
                            current_tool_params.region_type,
                            editing_tools::region_type_region_color);
        show_tooltip("Select voxels which are connected and share the same "
                     "palette color.");

        if (current_tool_params.region_type ==
            editing_tools::region_type_region_color)
        {
          ImGui::SetNextItemWidth(-1.0f);
          ImGui::SliderFloat("###region_fuziness",
                             &current_tool_params.region_fuziness, 0.0f, 1.0f);
          show_tooltip("Set to a value > 0 to fuzzy match colors.");
        }

        if (current_tool_params.region_type != editing_tools::region_type_color)
        {
          ImGui::Spacing();

          SAME_LINE_RESET();
          SAME_LINE_GROUP();
          toggle_button_group("6", tb_button_size,
                              current_tool_params.region_neighborhood,
                              region_neighborhood_6);
          show_tooltip("Select voxels connected to the six sides of the "
                       "center voxel.");
          SAME_LINE_GROUP();
          toggle_button_group("18", tb_button_size,
                              current_tool_params.region_neighborhood,
                              region_neighborhood_18);
          show_tooltip("Select voxels connected to the six sides and the "
                       "12 edges of the center voxel.");
          SAME_LINE_GROUP();
          toggle_button_group("26", tb_button_size,
                              current_tool_params.region_neighborhood,
                              region_neighborhood_26);
          show_tooltip("Select voxels connected to the six sides, the "
                       "12 edges, and the eight corners of the center voxel.");
        }
      }
    }

    if (current_tool_params.tool == editing_tools::tool_grass)
    {
      ImGui::Text("Grass");
      {
        ImGui::Spacing();

        ImGui::SetNextItemWidth(-1.0f);
        ImGui::SliderFloat("###grass_density",
                           &current_tool_params.tool_grass_density, 0.0f, 1.0f);
        show_tooltip("The density of the grass. A higher value indicates a "
                     "higher density.");
      }
    }

    if (current_tool_params.tool == editing_tools::tool_extrude ||
        current_tool_params.tool == editing_tools::tool_grass)
    {
      ImGui::Text("Face");
      {
        ImGui::Spacing();

        SAME_LINE_RESET();
        SAME_LINE_GROUP();
        toggle_button(ICON_FA_PALETTE, tb_button_size,
                      current_tool_params.face_palette_fill);
        show_tooltip("Fill voxels using the selected palette color range, "
                     "instead of the voxel colors.");
        SAME_LINE_GROUP();
        toggle_button_group(ICON_FA_CUBES, tb_button_size,
                            current_tool_params.face_type,
                            editing_tools::face_type_region);
        show_tooltip("Select voxels which are connected.");
        SAME_LINE_GROUP();
        toggle_button_group(ICON_FA_FILL, tb_button_size,
                            current_tool_params.face_type,
                            editing_tools::face_type_region_color);
        show_tooltip("Select voxels which are connected and share the same "
                     "palette color.");

        if (current_tool_params.face_type ==
            editing_tools::face_type_region_color)
        {
          ImGui::SetNextItemWidth(-1.0f);
          ImGui::SliderFloat("###face_fuziness",
                             &current_tool_params.face_fuzziness, 0.0f, 1.0f);
          show_tooltip("Set to a value > 0 to fuzzy match colors.");
        }

        ImGui::Spacing();

        SAME_LINE_RESET();
        SAME_LINE_GROUP();
        toggle_button_group("6", tb_button_size,
                            current_tool_params.face_neighborhood,
                            region_neighborhood_6);
        show_tooltip("Select voxels connected to the six sides of the "
                     "center voxel.");
        SAME_LINE_GROUP();
        toggle_button_group("18", tb_button_size,
                            current_tool_params.face_neighborhood,
                            region_neighborhood_18);
        show_tooltip("Select voxels connected to the six sides and the "
                     "12 edges of the center voxel.");
        SAME_LINE_GROUP();
        toggle_button_group("26", tb_button_size,
                            current_tool_params.face_neighborhood,
                            region_neighborhood_26);
        show_tooltip("Select voxels connected to the six sides, the "
                     "12 edges, and the eight corners of the center voxel.");
      }
    }

    ImGui::Text("Global");
    {
      ImGui::Spacing();

      SAME_LINE_RESET();
      SAME_LINE_GROUP();
      if (ImGui::Button(ICON_FA_CROP_SIMPLE, tb_button_size))
      {
        io_component_voxel_shape->compact(shape);
        shape_changed = true;
      }
      show_tooltip(
          "Compact: Shrinks the shape in size so it utilizes space most "
          "efficiently.");

      SAME_LINE_GROUP();
      if (ImGui::Button(ICON_FA_ERASER "###global_erase", tb_button_size))
      {
        editing::global_erase(shape);
        shape_changed = true;
      }
      show_tooltip("Erase: Erases all voxels of the currently selected shape.");

      SAME_LINE_GROUP();
      if (ImGui::Button(ICON_FA_SQUARE, tb_button_size))
      {
        editing::global_full(shape, current_tool_params.palette_range);
        shape_changed = true;
      }
      show_tooltip("Full: Fills all voxels of the selected shape.");

      SAME_LINE_GROUP();
      if (ImGui::Button(ICON_FA_FILL_DRIP, tb_button_size))
      {
        editing::global_fill(shape, current_tool_params.palette_range);
        shape_changed = true;
      }
      show_tooltip("Fill: Fills all solid voxels of the selected shape.");

      SAME_LINE_GROUP();
      if (ImGui::Button(ICON_FA_CIRCLE_HALF_STROKE, tb_button_size))
      {
        editing::global_invert(shape, current_tool_params.palette_range);
        shape_changed = true;
      }
      show_tooltip(
          "Invert: Fills all empty voxels and removes all solid voxels.");
    }

    ImGui::Text("Boolean");
    {
      ImGui::Spacing();

      SAME_LINE_RESET();
      SAME_LINE_GROUP();
      if (ImGui::Button(ICON_FA_LOCATION_CROSSHAIRS, tb_button_size))
        op = shape;
      show_tooltip("Sets the current shape as the active operator.");

      // TODO: Comparisons of refs are ugly
      ImGui::BeginDisabled(!io_base->ref_is_valid(op) ||
                           op.internal == shape.internal);

      SAME_LINE_GROUP();
      if (ImGui::Button(ICON_FA_CLONE, tb_button_size))
      {
        editing::global_boolean<editing::bool_op_replacement>(shape, op);
        shape_changed = true;
      }
      show_tooltip("Replacement: Replaces solid voxels of "
                   "the current shape with the "
                   "intersecting solid voxels of the operator.");

      SAME_LINE_GROUP();
      if (ImGui::Button(ICON_FA_OBJECT_SUBTRACT, tb_button_size))
      {
        editing::global_boolean<editing::bool_op_subtraction>(shape, op);
        shape_changed = true;
      }
      show_tooltip("Subtraction: Removes solid voxels of the current shape "
                   "intersecting with "
                   "solid voxels of the operator.");

      SAME_LINE_GROUP();
      if (ImGui::Button(ICON_FA_OBJECT_UNION, tb_button_size))
      {
        editing::global_boolean<editing::bool_op_union>(shape, op);
        shape_changed = true;
      }
      show_tooltip("Union: Adds the solid voxels of the operator to the "
                   "current shape.");

      SAME_LINE_GROUP();
      if (ImGui::Button(ICON_FA_OBJECT_INTERSECT, tb_button_size))
      {
        editing::global_boolean<editing::bool_op_intersection>(shape, op);
        shape_changed = true;
      }
      show_tooltip(
          "Intersection: Only keeps intersecting voxels of the current "
          "shape and the operator.");

      ImGui::EndDisabled();
    }

    ImGui::EndDisabled();

    if (shape_changed)
      io_component_voxel_shape->voxelize(shape);
  }
  ImGui::End();
}
} // namespace editing_ui