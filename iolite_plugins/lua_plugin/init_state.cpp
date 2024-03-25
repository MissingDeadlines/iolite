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

// clang-format off

#include "lua_plugin.h"

namespace internal
{

//----------------------------------------------------------------------------//
namespace math_helper
{
//----------------------------------------------------------------------------//
template <typename T> inline auto lerp(const T& x, const T& y, float a) -> T
{
  return io_cvt(glm::mix(io_cvt(x), io_cvt(y), a));
}

//----------------------------------------------------------------------------//
template <> inline auto lerp(const float& x, const float& y, float a) -> float
{
  return glm::mix(x, y, a);
}

//----------------------------------------------------------------------------//
template <typename T> inline auto slerp(const T& x, const T& y, float a) -> T
{
  return io_cvt(glm::slerp(io_cvt(x), io_cvt(y), a));
}

//----------------------------------------------------------------------------//
template <typename T>
inline auto vec_get_component(const T& vec, io_uint32_t idx) -> float
{
  return io_cvt(vec)[idx];
}

//----------------------------------------------------------------------------//
template <typename T> inline auto vec_mul(const T& l, const T& r) -> T
{
  return io_cvt(io_cvt(l) * io_cvt(r));
}

//----------------------------------------------------------------------------//
template <typename T> inline auto vec_scale(float s, const T& vec) -> T
{
  return io_cvt(s * io_cvt(vec));
}

//----------------------------------------------------------------------------//
template <typename T> inline auto vec_div(const T& x, const T& y) -> T
{
  return io_cvt(io_cvt(x) / io_cvt(y));
}

//----------------------------------------------------------------------------//
template <typename T> inline auto vec_add(const T& x, const T& y) -> T
{
  return io_cvt(io_cvt(x) + io_cvt(y));
}

//----------------------------------------------------------------------------//
template <typename T> inline auto vec_sub(const T& x, const T& y) -> T
{
  return io_cvt(io_cvt(x) - io_cvt(y));
}

//----------------------------------------------------------------------------//
template <typename T> inline auto vec_normalize(const T& x) -> T
{
  return io_cvt(glm::normalize(io_cvt(x)));
}

//----------------------------------------------------------------------------//
template <typename T> inline auto vec_length(const T& x) -> float
{
  return glm::length(io_cvt(x));
}

//----------------------------------------------------------------------------//
template <typename T> inline auto vec_length2(const T& x) -> float
{
  return glm::length2(io_cvt(x));
}

//----------------------------------------------------------------------------//
template <typename T> inline auto vec_distance(const T& x, const T& y) -> float
{
  return glm::distance(io_cvt(x), io_cvt(y));
}

//----------------------------------------------------------------------------//
template <typename T> inline auto vec_distance2(const T& x, const T& y) -> float
{
  return glm::distance2(io_cvt(x), io_cvt(y));
}

//----------------------------------------------------------------------------//
template <typename T> inline auto vec_cross(const T& x, const T& y) -> T
{
  return io_cvt(glm::cross(io_cvt(x), io_cvt(y)));
}

//----------------------------------------------------------------------------//
template <typename T> inline auto vec_dot(const T& x, const T& y) -> float
{
  return glm::dot(io_cvt(x), io_cvt(y));
}

//----------------------------------------------------------------------------//
inline auto quat_rotate(const io_quat_t& quat, const io_vec3_t& vec)
{
  return io_cvt(io_cvt(quat) * io_cvt(vec));
}

//----------------------------------------------------------------------------//
inline auto quat_inverse(const io_quat_t& quat)
{
  return io_cvt(glm::inverse(io_cvt(quat)));
}

//----------------------------------------------------------------------------//
inline auto quat_look_at(const io_vec3_t& dir, io_vec3_t& up)
{
  return io_cvt(glm::quatLookAt(io_cvt(dir), io_cvt(up)));
}

//----------------------------------------------------------------------------//
inline auto quat_from_angle_axis(float angle, io_vec3_t& axis)
{
  return io_cvt(glm::angleAxis(angle, io_cvt(axis)));
}

//----------------------------------------------------------------------------//
inline auto quat_to_euler_angles(const io_quat_t& quat)
{
  return io_cvt(glm::eulerAngles(io_cvt(quat)));
}

//----------------------------------------------------------------------------//
inline auto quat_from_euler_angles(const io_vec3_t& euler)
{
  return io_cvt(glm::quat(io_cvt(euler)));
}

//----------------------------------------------------------------------------//
inline auto quat_rotation(const io_vec3_t& x, io_vec3_t& y)
{
  return io_cvt(glm::rotation(io_cvt(x), io_cvt(y)));
}

//----------------------------------------------------------------------------//
inline auto calc_random_number_fast(io_uint64_t& seed)
{
  seed ^= seed << 13;
  seed ^= seed >> 7;
  seed ^= seed << 17;

  return seed;
}

//----------------------------------------------------------------------------//
inline auto calc_random_float_min_max_fast(float min, float max,
                                           io_uint64_t& seed) -> float
{
  return uint32_t(calc_random_number_fast(seed)) /
             static_cast<float>(std::numeric_limits<uint32_t>::max()) *
             (max - min) +
         min;
}

//----------------------------------------------------------------------------//
inline auto calc_random_float_fast(io_uint64_t& seed) -> float
{
  return calc_random_float_min_max_fast(0.0f, 1.0f, seed);
}

} // namespace math_helper

// @namespace Components
// @category Components Dummy category.
// @hidden

// @function get_type_id
// @summary Returns the type ID of the component.
// @param component Ref The component.
// @return number value The type ID of the component.

// @function create
// @summary Creates a new component and attaches it to the provided parent entity.
// @param parent_entity Ref The parent entity.

// @function destroy
// @summary Destroys the provided component.
// @param component Ref The component to destroy.

// @function commit_changes
// @summary Commits all changes to the properties of the provided component.
// @param component Ref The component to update.

// @function get_num_active_components
// @summary Returns the total number of active components of this type.
// @return number value Total total number of active components of this type.

// @function get_component_for_entity
// @summary Returns the component for the given entity.
// @param entity Ref The entity.
// @return Ref value The component for the given entity.

// @function is_alive
// @summary Returns true if the referenced component is alive.
// @param component Ref The component.
// @return boolean value True if the component is alive.

// @function get_entity
// @summary Returns the entity the component is assigned to.
// @param component Ref The component.
// @return Ref value The entity the component is assigned to.

// @function get_property
// @summary Returns the requested property as a variant.
// @param component Ref The component.
// @param property_name string The name of the property to retrieve.
// @return Variant value The property as a variant.

// @function set_property
// @summary Sets the requested property to the provided variant value.
// @param component Ref The component.
// @param property_name string The name of the property to set.
// @param value Variant The value to set.

// @function list_properties
// @summary Lists all properties exposed by this component interface.
// @return table value Table containing the names and types of all the exposed properties.

// @namespace Interface
// @category Interface Dummy category.
// @hidden

// @function load
// @summary Loads all functions and types for this scripting interface.

//----------------------------------------------------------------------------//
void script_init_state(sol::state& s)
{
  s.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string,
                   sol::lib::table, sol::lib::bit32);

  s["__ScriptIsActive"] = false;

  // @namespace Utils
  // @category Utils Various utility functions.

  s["Utils"] = s.create_table();

  // @function execute
  // @summary Executes the provided string as a Lua script.
  // @param script string The Lua script source to execute.
  // @return any value The result of the executed script.
  s["Utils"]["execute"] = [&s](const char* code) -> sol::reference {
    return s.script(code);
  };

  // @function load
  // @summary Executes the script with the given name.
  // @param script_name string The name of the script to load as a module (without the file extension).
  // @return any value The result of the executed script.
  s["Utils"]["load"] = [&s](const char* script_name) -> sol::reference {
    const std::string filepath =
        std::string("media/scripts/") + script_name + ".lua";
    auto script = internal::load_script(s, filepath.c_str());
    if (script.valid())
      return script();
    return sol::nil;
  };

  // Used to cache modules using the require function
  s["__MODULES"] = s.create_table();

  // @function require
  // @summary Executes the script with the given name. Executed once for each script. Successive calls return the cached script result.
  // @param script_name string The name of the script to execute (without the file extension).
  // @return any value The result of the executed script.
  s["Utils"]["require"] = [&s](const char* script_name) -> sol::reference {
    if (!s["__MODULES"][script_name].valid())
    {
      const std::string filepath =
          std::string("media/scripts/") + script_name + ".lua";
      auto script = internal::load_script(s, filepath.c_str());
      if (script.valid())
        s["__MODULES"][script_name] = script();
    }

    return s["__MODULES"][script_name];
  };

  // @namespace Global
  // @category Globals Provides various global functions and types.

  // @function InvalidRef
  // @summary Creates an invalid ref.
  // @return Ref value The invalid ref.
  s["InvalidRef"] = []() { return io_base->ref_invalid(); };

  // @type Handle
  // @summary Handles are used to reference elements of the various subsystems.

  // @type PropertyDesc
  // @summary Description of a property.
  // @member name string
  // @member type string
  s.new_usertype<io_property_desc_t>("PropertyDesc", sol::no_constructor,
                                     "name", &io_property_desc_t::name, "type",
                                     &io_property_desc_t::type);

  // @type Ref
  // @summary Reference to entities, components and resources.
  s.new_usertype<io_ref_t>("Ref", sol::no_constructor);

  // @type Variant
  // @summary Special data type that can contain different types of data.
  s.new_usertype<io_variant_t>("Variant", sol::no_constructor);

  // @type Vec2
  // @summary A vector storing two floating point components.
  // @member x number
  // @member y number
  s.new_usertype<io_vec2_t>("Vec2", sol::no_constructor, "x", &io_vec2_t::x,
                            "y", &io_vec2_t::y);

  // @function Vec2
  // @summary Initializes a new Vec2.
  // @param x number The scalar value to set the components to.
  // @return Vec2 value The new vector.

  // @function Vec2
  // @summary Initializes a new Vec2.
  // @param vec Vec2 The vector to copy from.
  // @return Vec2 value The new vector.

  // @function Vec2
  // @summary Initializes a new Vec2.
  // @param x number First component.
  // @param y number Second component.
  // @return Vec2 value The new vector.
  s["Vec2"] = sol::overload(
      [](float s) -> io_vec2_t {
        return {s, s};
      },
      [](const io_vec2_t& v) -> io_vec2_t {
        return {v.x, v.y};
      },
      [](float x, float y) -> io_vec2_t {
        return {x, y};
      });

  // @type UVec2
  // @summary A vector storing two unsigned integer components.
  // @member x number
  // @member y number
  s.new_usertype<io_uvec2_t>("UVec2", sol::no_constructor, "x", &io_uvec2_t::x,
                             "y", &io_uvec2_t::y);

  // @function UVec2
  // @summary Initializes a new UVec2.
  // @param x number The scalar value to set the components to.
  // @return UVec2 value The new vector.

  // @function UVec2
  // @summary Initializes a new UVec2.
  // @param vec UVec2 The vector to copy from.
  // @return UVec2 value The new vector.

  // @function UVec2
  // @summary Initializes a new UVec2.
  // @param x number First component.
  // @param y number Second component.
  // @return UVec2 value The new vector.
  s["UVec2"] = sol::overload(
      [](uint32_t s) -> io_uvec2_t {
        return {s, s};
      },
      [](const io_uvec2_t& v) -> io_uvec2_t {
        return {v.x, v.y};
      },
      [](uint32_t x, uint32_t y) -> io_uvec2_t {
        return {x, y};
      });

  // @type IVec2
  // @summary A vector storing two signed integer components.
  // @member x number
  // @member y number
  s.new_usertype<io_ivec2_t>("IVec2", sol::no_constructor, "x", &io_ivec2_t::x,
                             "y", &io_ivec2_t::y);

  // @function IVec2
  // @summary Initializes a new IVec2.
  // @param x number The scalar value to set the components to.
  // @return IVec2 value The new vector.

  // @function IVec2
  // @summary Initializes a new IVec2.
  // @param vec IVec2 The vector to copy from.
  // @return IVec2 value The new vector.

  // @function IVec2
  // @summary Initializes a new IVec2.
  // @param x number First component.
  // @param y number Second component.
  // @return IVec2 value The new vector.
  s["IVec2"] = sol::overload(
      [](int32_t s) -> io_ivec2_t {
        return {s, s};
      },
      [](const io_ivec2_t& v) -> io_ivec2_t {
        return {v.x, v.y};
      },
      [](int32_t x, int32_t y) -> io_ivec2_t {
        return {x, y};
      });

  // @type Vec3
  // @summary A vector storing three floating point components.
  // @member x number
  // @member y number
  // @member z number
  s.new_usertype<io_vec3_t>("Vec3", sol::no_constructor, "x", &io_vec3_t::x,
                            "y", &io_vec3_t::y, "z", &io_vec3_t::z);

  // @function Vec3
  // @summary Initializes a new Vec3.
  // @param x number The scalar value to set the components to.
  // @return Vec3 value The new vector.

  // @function Vec3
  // @summary Initializes a new Vec3.
  // @param vec Vec3 The vector to copy from.
  // @return Vec3 value The new vector.

  // @function Vec3
  // @summary Initializes a new Vec3.
  // @param x number First component.
  // @param y number Second component.
  // @param z number Third component.
  // @return Vec3 value The new vector.
  s["Vec3"] = sol::overload(
      [](float s) -> io_vec3_t {
        return {s, s, s};
      },
      [](const io_vec3_t& v) -> io_vec3_t {
        return {v.x, v.y, v.z};
      },
      [](float x, float y, float z) -> io_vec3_t {
        return {x, y, z};
      });

  // @type UVec3
  // @summary A vector storing three unsigned integer components.
  // @member x number
  // @member y number
  // @member z number
  s.new_usertype<io_uvec3_t>("UVec3", sol::no_constructor, "x", &io_uvec3_t::x,
                             "y", &io_uvec3_t::y, "z", &io_uvec3_t::z);

  // @function UVec3
  // @summary Initializes a new UVec3.
  // @param x number The scalar value to set the components to.
  // @return UVec3 value The new vector.

  // @function UVec3
  // @summary Initializes a new UVec3.
  // @param vec UVec3 The vector to copy from.
  // @return UVec3 value The new vector.

  // @function UVec3
  // @summary Initializes a new UVec3.
  // @param x number First component.
  // @param y number Second component.
  // @param z number Third component.
  // @return UVec3 value The new vector.
  s["UVec3"] = sol::overload(
      [](io_uint32_t s) -> io_uvec3_t {
        return {s, s, s};
      },
      [](const io_uvec3_t& v) -> io_uvec3_t {
        return {v.x, v.y, v.z};
      },
      [](io_uint32_t x, io_uint32_t y, io_uint32_t z) -> io_uvec3_t {
        return {x, y, z};
      });

  // @type U16Vec3
  // @summary A vector storing three unsigned integer components.
  // @member x number
  // @member y number
  // @member z number
  s.new_usertype<io_u16vec3_t>("U16Vec3", sol::no_constructor, "x",
                               &io_u16vec3_t::x, "y", &io_u16vec3_t::y, "z",
                               &io_u16vec3_t::z);

  // @function U16Vec3
  // @summary Initializes a new U16Vec3.
  // @param x number The scalar value to set the components to.
  // @return U16Vec3 value The new vector.

  // @function U16Vec3
  // @summary Initializes a new U16Vec3.
  // @param vec U16Vec3 The vector to copy from.
  // @return U16Vec3 value The new vector.

  // @function U16Vec3
  // @summary Initializes a new U16Vec3.
  // @param x number First component.
  // @param y number Second component.
  // @param z number Third component.
  // @return U16Vec3 value The new vector.
  s["U16Vec3"] = sol::overload(
      [](io_uint16_t s) -> io_u16vec3_t {
        return {s, s, s};
      },
      [](const io_u16vec3_t& v) -> io_u16vec3_t {
        return {v.x, v.y, v.z};
      },
      [](io_uint16_t x, io_uint16_t y, io_uint16_t z) -> io_u16vec3_t {
        return {x, y, z};
      });

  // @type U8Vec3
  // @summary A vector storing three unsigned integer components.
  // @member x number
  // @member y number
  // @member z number
  s.new_usertype<io_u8vec3_t>("U8Vec3", sol::no_constructor, "x",
                              &io_u8vec3_t::x, "y", &io_u8vec3_t::y, "z",
                              &io_u8vec3_t::z);

  // @function U8Vec3
  // @summary Initializes a new U8Vec3.
  // @param x number The scalar value to set the components to.
  // @return U8Vec3 value The new vector.

  // @function U8Vec3
  // @summary Initializes a new U8Vec3.
  // @param vec U8Vec3 The vector to copy from.
  // @return U8Vec3 value The new vector.

  // @function U8Vec3
  // @summary Initializes a new U8Vec3.
  // @param x number First component.
  // @param y number Second component.
  // @param z number Third component.
  // @return U8Vec3 value The new vector.
  s["U8Vec3"] = sol::overload(
      [](io_uint8_t s) -> io_u8vec3_t {
        return {s, s, s};
      },
      [](const io_u8vec3_t& v) -> io_u8vec3_t {
        return {v.x, v.y, v.z};
      },
      [](io_uint8_t x, io_uint8_t y, io_uint8_t z) -> io_u8vec3_t {
        return {x, y, z};
      });

  // @type IVec3
  // @summary A vector storing three signed integer components.
  // @member x number
  // @member y number
  // @member z number
  s.new_usertype<io_ivec3_t>("IVec3", sol::no_constructor, "x", &io_ivec3_t::x,
                             "y", &io_ivec3_t::y, "z", &io_ivec3_t::z);

  // @function IVec3
  // @summary Initializes a new IVec3.
  // @param x number The scalar value to set the components to.
  // @return IVec3 value The new vector.

  // @function IVec3
  // @summary Initializes a new IVec3.
  // @param vec IVec3 The vector to copy from.
  // @return IVec3 value The new vector.

  // @function IVec3
  // @summary Initializes a new IVec3.
  // @param x number First component.
  // @param y number Second component.
  // @param z number Third component.
  // @return IVec3 value The new vector.
  s["IVec3"] = sol::overload(
      [](io_int32_t s) -> io_ivec3_t {
        return {s, s, s};
      },
      [](const io_ivec3_t& v) -> io_ivec3_t {
        return {v.x, v.y, v.z};
      },
      [](io_int32_t x, io_int32_t y, io_int32_t z) -> io_ivec3_t {
        return {x, y, z};
      });

  // @type Vec4
  // @summary A vector storing four floating point components.
  // @member x number
  // @member y number
  // @member z number
  // @member w number
  s.new_usertype<io_vec4_t>("Vec4", sol::no_constructor, "x", &io_vec4_t::x,
                            "y", &io_vec4_t::y, "z", &io_vec4_t::z, "w",
                            &io_vec4_t::w);

  // @function Vec4
  // @summary Initializes a new Vec4.
  // @param x number The scalar value to set the components to.
  // @return Vec4 value The new vector.

  // @function Vec4
  // @summary Initializes a new Vec4.
  // @param vec Vec4 The vector to copy from.
  // @return Vec4 value The new vector.

  // @function Vec4
  // @summary Initializes a new Vec4.
  // @param x number First component.
  // @param y number Second component.
  // @param z number Third component.
  // @param w number Fourth component.
  // @return Vec4 value The new vector.
  s["Vec4"] = sol::overload(
      [](float s) -> io_vec4_t {
        return {s, s, s, s};
      },
      [](const io_vec4_t& v) -> io_vec4_t {
        return {v.x, v.y, v.z, v.w};
      },
      [](float x, float y, float z, float w) -> io_vec4_t {
        return {x, y, z, w};
      });

  // @type UVec4
  // @summary A vector storing four unsigned integer components.
  // @member x number
  // @member y number
  // @member z number
  // @member w number
  s.new_usertype<io_uvec4_t>("UVec4", sol::no_constructor, "x", &io_uvec4_t::x,
                             "y", &io_uvec4_t::y, "z", &io_uvec4_t::z, "w",
                             &io_uvec4_t::w);

  // @function UVec4
  // @summary Initializes a new UVec4.
  // @param x number The scalar value to set the components to.
  // @return UVec4 value The new vector.

  // @function UVec4
  // @summary Initializes a new UVec4.
  // @param vec UVec4 The vector to copy from.
  // @return UVec4 value The new vector.

  // @function UVec4
  // @summary Initializes a new UVec4.
  // @param x number First component.
  // @param y number Second component.
  // @param z number Third component.
  // @param w number Fourth component.
  // @return UVec4 value The new vector.
  s["UVec4"] = sol::overload(
      [](uint32_t s) -> io_uvec4_t {
        return {s, s, s, s};
      },
      [](const io_uvec4_t& v) -> io_uvec4_t {
        return {v.x, v.y, v.z, v.w};
      },
      [](uint32_t x, uint32_t y, uint32_t z, uint32_t w) -> io_uvec4_t {
        return {x, y, z, w};
      });

  // @type IVec4
  // @summary A vector storing four signed integer components.
  // @member x number
  // @member y number
  // @member z number
  // @member w number
  s.new_usertype<io_ivec4_t>("IVec4", sol::no_constructor, "x", &io_ivec4_t::x,
                             "y", &io_ivec4_t::y, "z", &io_ivec4_t::z, "w",
                             &io_ivec4_t::w);

  // @function IVec4
  // @summary Initializes a new IVec4.
  // @param x number The scalar value to set the components to.
  // @return IVec4 value The new vector.

  // @function IVec4
  // @summary Initializes a new IVec4.
  // @param vec IVec4 The vector to copy from.
  // @return IVec4 value The new vector.

  // @function IVec4
  // @summary Initializes a new IVec4.
  // @param x number First component.
  // @param y number Second component.
  // @param z number Third component.
  // @param w number Fourth component.
  // @return IVec4 value The new vector.
  s["IVec4"] = sol::overload(
      [](int32_t s) -> io_ivec4_t {
        return {s, s, s, s};
      },
      [](const io_ivec4_t& v) -> io_ivec4_t {
        return {v.x, v.y, v.z, v.w};
      },
      [](int32_t x, int32_t y, int32_t z, int32_t w) -> io_ivec4_t {
        return {x, y, z, w};
      });

  // @type Quat
  // @summary A quaternion with four floating point components.
  // @member w number
  // @member x number
  // @member y number
  // @member z number
  s.new_usertype<io_quat_t>("Quat", sol::no_constructor, "w", &io_quat_t::w,
                            "x", &io_quat_t::x, "y", &io_quat_t::y, "z",
                            &io_quat_t::z);

  // @function Quat
  // @summary Initializes a new Quat.
  // @param x number The scalar value to set the first component to.
  // @return Quat value The new quaternion.

  // @function Quat
  // @summary Initializes a new Quat.
  // @param quat Quat The quaternion to copy from.
  // @return Quat value The new quaternion.

  // @function Quat
  // @summary Initializes a new Quat.
  // @param w number First component.
  // @param x number Second component.
  // @param y number Third component.
  // @param z number Fourth component.
  // @return Quat value The new quaternion.
  s["Quat"] =
      sol::overload([](float s) -> io_quat_t { return {s}; },
                    [](const io_quat_t& v) -> io_quat_t {
                      return {v.w, v.x, v.y, v.z};
                    },
                    [](float w, float x, float y, float z) -> io_quat_t {
                      return {w, x, y, z};
                    });

  // @type HeightmapPixel
  // @summary A single pixel used for generating heightmaps.
  s.new_usertype<io_plugin_terrain_heightmap_pixel>("HeightmapPixel",
                                                    sol::no_constructor);

  // @type PathSettings
  // @summary Settings used when calculating paths via the Pathfinding related functions.
  // @member capsule_radius number The radius of the agent's capsule.
  // @member capsule_half_height number The half height of the agent's capsule.
  // @member step_height number The maximum step height an agent can take.
  // @member cell_size number The size of the cells used for the voxelization.
  s.new_usertype<io_pathfinding_path_settings_t>(
      "PathSettings", sol::no_constructor, "capsule_radius",
      &io_pathfinding_path_settings_t::capsule_radius, "capsule_half_height",
      &io_pathfinding_path_settings_t::capsule_half_height, "step_height",
      &io_pathfinding_path_settings_t::step_height, "cell_size",
      &io_pathfinding_path_settings_t::cell_size);

  // @type PhysicsContactEvent
  // @summary Physics event fired when contacts between two shapes are detected.
  // @member type string The type name.
  // @member data PhysicsContactEventData The data of the contact event.
  s.new_usertype<lua_physics_contact_event_t>(
      "PhysicsContactEvent", sol::no_constructor, "type",
      &lua_physics_contact_event_t::type, "data",
      &lua_physics_contact_event_t::data);
  // @type PhysicsContactEventData
  // @summary The data for a single physics contact event.
  // @member entity0 Ref The first entity in contact.
  // @member entity1 Ref The second entity in contact.
  // @member pos Vec3 The position of the contact.
  // @member impulse Vec3 The impulse of the contact.
  s.new_usertype<lua_physics_contact_event_t::event_data_t>(
      "PhysicsContactEventData", sol::no_constructor, "entity0",
      &lua_physics_contact_event_t::event_data_t::entity0, "entity1",
      &lua_physics_contact_event_t::event_data_t::entity1, "pos",
      &lua_physics_contact_event_t::event_data_t::pos, "impulse",
      &lua_physics_contact_event_t::event_data_t::impulse);

  // @type UIAnchor
  // @summary Defines an anchor used for creating (rectangle) transforms in the UI system.
  s.new_usertype<io_ui_anchor_t>("UIAnchor", sol::no_constructor);
  // @type UIAnchorOffsets
  // @summary Defines a set of anchor offsets used for creating (rectangle) transforms in the UI system.
  s.new_usertype<io_ui_anchor_offsets_t>("UIAnchorOffsets",
                                         sol::no_constructor);

  // @type UIRect
  // @summary A rectangle defined by a position and extent.
  // @member pos Vec2
  // @member extent Vec2
  s.new_usertype<io_ui_rect_t>("UIRect", sol::no_constructor, "pos",
                               &io_ui_rect_t::pos, "extent",
                               &io_ui_rect_t::extent);

  // @function UIAnchor
  // @summary Initializes a new UI anchor.
  // @param anchor number The position in [0, 1] relative to the parent transform.
  // @param offset number The absolute offset (in px).
  // @return UIAnchor value The new anchor.
  s["UIAnchor"] = [](io_float32_t anchor,
                     io_float32_t offset) -> io_ui_anchor_t {
    return {anchor, offset};
  };
  // @function UIAnchorOffsets
  // @summary Initializes a new set of UI anchor offsets.
  // @param left number Absolute offset (in px) to the left anchor.
  // @param right number Absolute offset (in px) to the right anchor.
  // @param top number Absolute offset (in px) to the top anchor.
  // @param bottom number Absolute offset (in px) to the bottom anchor.
  // @return UIAnchor value The new set of anchor offsets.
  s["UIAnchorOffsets"] = [](io_float32_t left, io_float32_t right,
                            io_float32_t top,
                            io_float32_t bottom) -> io_ui_anchor_offsets_t {
    return {left, right, top, bottom};
  };

  // @namespace Ref
  // @category Ref Functions to interact with refs.

  s["Ref"] = s.create_table();

  // @function get_type_id
  // @summary Returns the ID of the type referenced.
  // @param ref Ref The ref.
  // @return number value The ID of the type referenced.
  s["Ref"]["get_type_id"] = [](io_ref_t ref) {
    return io_base->ref_get_type_id(ref);
  };
  // @function is_valid
  // @summary Returns the ID of the type referenced.
  // @param ref Ref The ref.
  // @return boolean value True if the given ref is valid.
  s["Ref"]["is_valid"] = [](io_ref_t ref) {
    return io_base->ref_is_valid(ref);
  };
  // @function get_id
  // @summary Returns the unique ID of the referenced resource.
  // @param ref Ref The ref.
  // @return number value The ID of the referenced resource.
  s["Ref"]["get_id"] = [](io_ref_t ref) { return io_base->ref_get_id(ref); };

  // @namespace Variant
  // @category Variant Functions to interact with variants.

  s["Variant"] = s.create_table();

  // @function from_float
  // @summary Creates a new variant storing a single floating point value.
  // @param value number The floating point value to set.
  // @return Variant value The new variant.
  s["Variant"]["from_float"] = [](float value) {
    return io_base->variant_from_float(value);
  };
  // @function from_int
  // @summary Creates a new variant storing a single integer value.
  // @param value number The integer value to set.
  // @return Variant value The new variant.
  s["Variant"]["from_int"] = [](int32_t value) {
    return io_base->variant_from_int(value);
  };
  // @function from_uint
  // @summary Creates a new variant storing a single unsigned integer value.
  // @param value number The unsigned integer value to set.
  // @return Variant value The new variant.
  s["Variant"]["from_uint"] = [](uint32_t value) {
    return io_base->variant_from_uint(value);
  };
  // @function from_uint8
  // @summary Creates a new variant storing a single 8bit unsigned integer value.
  // @param value number The unsigned 8bit integer value to set.
  // @return Variant value The new variant.
  s["Variant"]["from_uint8"] = [](uint8_t value) {
    return io_base->variant_from_uint8(value);
  };
  // @function from_string
  // @summary Creates a new variant storing a string.
  // @param value string The string to set.
  // @return Variant value The new variant.
  s["Variant"]["from_string"] = [](const char* value) {
    return io_base->variant_from_string(value);
  };

  // @function from_vec2
  // @summary Creates a new variant storing a vector with two components.
  // @param value Vec2 The vector to set.
  // @return Variant value The new variant.
  s["Variant"]["from_vec2"] = [](const io_vec2_t& value) {
    return io_base->variant_from_vec2(value);
  };
  // @function from_vec3
  // @summary Creates a new variant storing a vector with three components.
  // @param value Vec3 The vector to set.
  // @return Variant value The new variant.
  s["Variant"]["from_vec3"] = [](const io_vec3_t& value) {
    return io_base->variant_from_vec3(value);
  };
  // @function from_vec4
  // @summary Creates a new variant storing a vector with four components.
  // @param value Vec4 The vector to set.
  // @return Variant value The new variant.
  s["Variant"]["from_vec4"] = [](const io_vec4_t& value) {
    return io_base->variant_from_vec4(value);
  };

  // @function from_quat
  // @summary Creates a new variant storing a quaternion with four components.
  // @param value Quat The quternion to set.
  // @return Variant value The new variant.
  s["Variant"]["from_quat"] = [](const io_quat_t& value) {
    return io_base->variant_from_quat(value);
  };

  // @function from_ivec2
  // @summary Creates a new variant storing an integer vector with two components.
  // @param value IVec2 The vector to set.
  // @return Variant value The new variant.
  s["Variant"]["from_ivec2"] = [](const io_ivec2_t& value) {
    return io_base->variant_from_ivec2(value);
  };
  // @function from_ivec3
  // @summary Creates a new variant storing an integer vector with three components.
  // @param value IVec3 The vector to set.
  // @return Variant value The new variant.
  s["Variant"]["from_ivec3"] = [](const io_ivec3_t& value) {
    return io_base->variant_from_ivec3(value);
  };
  // @function from_ivec4
  // @summary Creates a new variant storing an integer vector with fours components.
  // @param value IVec4 The vector to set.
  // @return Variant value The new variant.
  s["Variant"]["from_ivec4"] = [](const io_ivec4_t& value) {
    return io_base->variant_from_ivec4(value);
  };

  // @function from_uvec2
  // @summary Creates a new variant storing an unsigned integer vector with two components.
  // @param value UVec2 The vector to set.
  // @return Variant value The new variant.
  s["Variant"]["from_uvec2"] = [](const io_uvec2_t& value) {
    return io_base->variant_from_uvec2(value);
  };
  // @function from_uvec3
  // @summary Creates a new variant storing an unsigned integer vector with three components.
  // @param value UVec3 The vector to set.
  // @return Variant value The new variant.
  s["Variant"]["from_uvec3"] = [](const io_uvec3_t& value) {
    return io_base->variant_from_uvec3(value);
  };
  // @function from_u8vec3
  // @summary Creates a new variant storing an 8-bit unsigned integer vector with three components.
  // @param value U8Vec3 The vector to set.
  // @return Variant value The new variant.
  s["Variant"]["from_u8vec3"] = [](const io_u8vec3_t& value) {
    return io_base->variant_from_u8vec3(value);
  };
  // @function from_u16vec3
  // @summary Creates a new variant storing an 16-bit unsigned integer vector with three components.
  // @param value U16Vec3 The vector to set.
  // @return Variant value The new variant.
  s["Variant"]["from_u16vec3"] = [](const io_u16vec3_t& value) {
    return io_base->variant_from_u16vec3(value);
  };
  // @function from_uvec4
  // @summary Creates a new variant storing an unsigned integer vector with fours components.
  // @param value UVec4 The vector to set.
  // @return Variant value The new variant.
  s["Variant"]["from_uvec4"] = [](const io_uvec4_t& value) {
    return io_base->variant_from_uvec4(value);
  };

  // @function get_float
  // @summary Returns the underlying floating point value.
  // @param variant Variant The variant to retrieve from.
  // @return number value The floating point value.
  s["Variant"]["get_float"] = [](io_variant_t variant) {
    return io_base->variant_get_float(variant);
  };
  // @function get_int
  // @summary Returns the underlying integer value.
  // @param variant Variant The variant to retrieve from.
  // @return number value The integer value.
  s["Variant"]["get_int"] = [](io_variant_t variant) {
    return io_base->variant_get_int(variant);
  };
  // @function get_uint
  // @summary Returns the underlying unsigned integer value.
  // @param variant Variant The variant to retrieve from.
  // @return number value The unsigned integer value.
  s["Variant"]["get_uint"] = [](io_variant_t variant) {
    return io_base->variant_get_uint(variant);
  };
  // @function get_string
  // @summary Returns the underlying string.
  // @param variant Variant The variant to retrieve from.
  // @return number value The string.
  s["Variant"]["get_string"] = [](io_variant_t variant) {
    return io_base->variant_get_string(variant);
  };

  // @function get_vec2
  // @summary Returns the underlying two-component vector.
  // @param variant Variant The variant to retrieve from.
  // @return Vec2 value The vector.
  s["Variant"]["get_vec2"] = [](io_variant_t variant) {
    return io_base->variant_get_vec2(variant);
  };
  // @function get_vec3
  // @summary Returns the underlying three-component vector.
  // @param variant Variant The variant to retrieve from.
  // @return Vec3 value The vector.
  s["Variant"]["get_vec3"] = [](io_variant_t variant) {
    return io_base->variant_get_vec3(variant);
  };
  // @function get_vec4
  // @summary Returns the underlying four-component vector.
  // @param variant Variant The variant to retrieve from.
  // @return Vec4 value The vector.
  s["Variant"]["get_vec4"] = [](io_variant_t variant) {
    return io_base->variant_get_vec4(variant);
  };

  // @function get_quat
  // @summary Returns the underlying four-component quaternion.
  // @param variant Variant The variant to retrieve from.
  // @return Quat value The quaternion.
  s["Variant"]["get_quat"] = [](io_variant_t variant) {
    return io_base->variant_get_vec4(variant);
  };

  // @function get_ivec2
  // @summary Returns the underlying two-component integer vector.
  // @param variant Variant The variant to retrieve from.
  // @return IVec2 value The vector.
  s["Variant"]["get_ivec2"] = [](io_variant_t variant) {
    return io_base->variant_get_ivec2(variant);
  };
  // @function get_ivec3
  // @summary Returns the underlying three-component integer vector.
  // @param variant Variant The variant to retrieve from.
  // @return IVec3 value The vector.
  s["Variant"]["get_ivec3"] = [](io_variant_t variant) {
    return io_base->variant_get_ivec3(variant);
  };
  // @function get_ivec4
  // @summary Returns the underlying four-component integer vector.
  // @param variant Variant The variant to retrieve from.
  // @return IVec4 value The vector.
  s["Variant"]["get_ivec4"] = [](io_variant_t variant) {
    return io_base->variant_get_ivec4(variant);
  };

  // @function get_uvec2
  // @summary Returns the underlying two-component unsigned integer vector.
  // @param variant Variant The variant to retrieve from.
  // @return UVec2 value The vector.
  s["Variant"]["get_uvec2"] = [](io_variant_t variant) {
    return io_base->variant_get_uvec2(variant);
  };
  // @function get_uvec3
  // @summary Returns the underlying three-component unsigned integer vector.
  // @param variant Variant The variant to retrieve from.
  // @return UVec3 value The vector.
  s["Variant"]["get_uvec3"] = [](io_variant_t variant) {
    return io_base->variant_get_uvec3(variant);
  };
  // @function get_u8vec3
  // @summary Returns the underlying three-component 8-bit unsigned integer vector.
  // @param variant Variant The variant to retrieve from.
  // @return U8Vec3 value The vector.
  s["Variant"]["get_u8vec3"] = [](io_variant_t variant) {
    return io_base->variant_get_u8vec3(variant);
  };
  // @function get_u16vec3
  // @summary Returns the underlying three-component 16-bit unsigned integer vector.
  // @param variant Variant The variant to retrieve from.
  // @return U16Vec3 value The vector.
  s["Variant"]["get_u16vec3"] = [](io_variant_t variant) {
    return io_base->variant_get_u16vec3(variant);
  };
  // @function get_uvec4
  // @summary Returns the underlying four-component unsigned integer vector.
  // @param variant Variant The variant to retrieve from.
  // @return UVec4 value The vector.
  s["Variant"]["get_uvec4"] = [](io_variant_t variant) {
    return io_base->variant_get_uvec4(variant);
  };

  s["Math"] = s.create_table();
  s["Math"]["load"] = [&s]() {

    // @namespace Math

    // @category Math_Constants Provides various useful constants.

    // @type Math
    // @summary Various math related constants.
    // @member pi number Pi
    // @member half_pi number Pi / 2
    // @member two_pi number Pi * 2
    s["Math"]["pi"] = glm::pi<float>();
    s["Math"]["half_pi"] = glm::half_pi<float>();
    s["Math"]["two_pi"] = glm::two_pi<float>();

    // @category Math_General General math related functions.
    // @copy_category Interface

    // @function pow
    // @summary Calculates x raised to the power y.
    // @param x number The base.
    // @param y number The exponent.
    // @return number value The result of x raised to power y.
    s["Math"]["pow"] = powf;

    // @function sqrt
    // @summary Calculates the square root of x.
    // @param x number The input value.
    // @return number value The square root of x.
    s["Math"]["sqrt"] = sqrtf;

    // @function exp
    // @summary Calculates e (Euler's number) raised to the power x.
    // @param x number The exponent.
    // @return number value The result of e raised to the power x.
    s["Math"]["exp"] = expf;

    // @function abs
    // @summary Calculates the absolute value of x.
    // @param x number The input value.
    // @return number value The absolute value of x.
    s["Math"]["abs"] = [](io_float32_t x) {
      return glm::abs(x);
    };

    // @function clamp
    // @summary Clamps x to the provided minimum and maximum.
    // @param x number The value to clamp.
    // @param min number The minimum.
    // @param max number The maximum.
    // @return number value The result of x clamped to [min, max].
    s["Math"]["clamp"] = [](io_float32_t x, io_float32_t min, io_float32_t max)
    {
      return glm::clamp(x, min, max);
    };

    // @function min
    // @summary Calculates the minimum of x and y.
    // @param x number The first input value.
    // @param y number The second input value.
    // @return number value The minimum of x and y.
    s["Math"]["min"] = [](io_float32_t x, io_float32_t y) {
      return glm::min(x, y);
    };

    // @function max
    // @summary Calculates the maximum of x and y.
    // @param x number The first input value.
    // @param y number The second input value.
    // @return number value The maximum of x and y.
    s["Math"]["max"] = [](io_float32_t x, io_float32_t y) {
      return glm::max(x, y);
    };

    // @function floor
    // @summary Calculates the largest integer number to greater than x.
    // @param x number The input value.
    // @return number value The largest integer value not greater than x.
    s["Math"]["floor"] = [](float x) { return glm::floor(x); };

    // @function ceil
    // @summary Calculates the smallest integer number not less than x.
    // @param x number The input value.
    // @return number value The smallest integer value not less than x.
    s["Math"]["ceil"] = [](float x) { return glm::ceil(x); };

    // @function round
    // @summary Calculates the nearest integer value to x, rounding halfway cases away from zero.
    // @param x number The input value.
    // @return number value The rounded value.
    s["Math"]["round"] = [](float x) { return glm::round(x); };
    // @function fract
    // @summary Returns the fractional part of x, calculated as x - floor(x).
    // @param x number The input value.
    // @return number value The fractional part of x.
    s["Math"]["fract"] = [](float x) { return glm::fract(x); };
    // @function trunc
    // @summary Calculates a value equal to the nearest integer to x whose absolute value is not larger than the absolute value of x. 
    // @param x number The input value.
    // @return number value The truncated value.
    s["Math"]["trunc"] = [](float x) { return glm::trunc(x); };

    // @category Math_Trigonometry Trigonometric math functions.

    // @function radians
    // @summary Convers the provided value in degrees to radians.
    // @param x number Input value in degrees.
    // @return number value Input value converted to radians.
    s["Math"]["radians"] = glm::radians<float>;
    // @function degrees
    // @summary Convers the provided value in radians to degrees.
    // @param angle number Input value in radians.
    // @return number value Input value converted to degrees.
    s["Math"]["degrees"] = glm::degrees<float>;

    // @function sin
    // @summary Calculates the sine of angle.
    // @param angle number The input angle (in radians).
    // @return number value The sine of angle.
    s["Math"]["sin"] = [](io_float32_t x) { return glm::sin(x); };
    // @function asin
    // @summary Calculates the arc sine of x.
    // @param x number The input value.
    // @return number value The arc sine of x (in radians).
    s["Math"]["asin"] = [](io_float32_t x) { return glm::asin(x); };
    // @function cos
    // @summary Calculates the cosine of angle.
    // @param angle number The input angle.
    // @return number value The cosine of angle.
    s["Math"]["cos"] = [](io_float32_t x) { return glm::cos(x); };
    // @function acos
    // @summary Calculates the arc cosine of x.
    // @param x number The input value.
    // @return number value The arc cosine of x (in radians).
    s["Math"]["acos"] = [](io_float32_t x) { return glm::acos(x); };
    // @function tan
    // @summary Calculates the tangent of angle.
    // @param angle number The input angle.
    // @return number value The tangent of angle.
    s["Math"]["tan"] = [](io_float32_t x) { return glm::tan(x); };
    // @function atan
    // @summary Calculates the arc tangent of x.
    // @param x number The input value.
    // @return number value The arc tangent of x.
    s["Math"]["atan"] = [](io_float32_t x) { return glm::atan(x); };
    // @function atan
    // @summary Calculates the arc tangent of y over x.
    // @param y number The first input value.
    // @param x number The second input value.
    // @return number value The arc tangent of y over x.
    s["Math"]["atan2"] = [](io_float32_t y, io_float32_t x) { return glm::atan(y, x); };

    // @category Math_Interpolation Collection of functions to interpolate values.

    // @function lerp
    // @summary Linearly interpolates between x and y.
    // @param x number|Vec2|Vec3|Vec4 The first input value.
    // @param y number|Vec2|Vec3|Vec4 The second input value.
    // @param a number The value to use to interpolate between x and y.
    // @return number|Vec2|Vec3|Vec4 value The interpolated value.
    s["Math"]["lerp"] = sol::overload(math_helper::lerp<float>, math_helper::lerp<io_vec2_t>,
                                      math_helper::lerp<io_vec3_t>, math_helper::lerp<io_vec4_t>);
    // @function slerp
    // @summary Spherically linearly interpolates between x and y.
    // @param x Quat The first input value.
    // @param y Quat The second input value.
    // @param a number The value to use to interpolate between x and y.
    // @return Quat value The interpolated value.
    s["Math"]["slerp"] = math_helper::slerp<io_quat_t>;


    // @category Math_Vectors Collection of functions to work with vectors.

    // @function vec_get_component
    // @summary Selects the component of the vector using the provided index.
    // @param x Vec2|Vec3|Vec4 The input value.
    // @param idx number The index of the component to retrieve.
    // @return number value The select4ed component of x.
    s["Math"]["vec_get_component"] =
        sol::overload(math_helper::vec_get_component<io_vec2_t>, math_helper::vec_get_component<io_vec3_t>,
                      math_helper::vec_get_component<io_vec4_t>);
    // @function vec_mul
    // @summary Multiplies the provided vectors.
    // @param x Vec2|Vec3|Vec4 The first input value.
    // @param y Vec2|Vec3|Vec4 The second input value.
    // @return Vec2|Vec3|Vec4 value The result of x * y.
    s["Math"]["vec_mul"] =
        sol::overload(math_helper::vec_mul<io_vec2_t>, math_helper::vec_mul<io_vec3_t>, math_helper::vec_mul<io_vec4_t>);
    // @function vec_scale
    // @summary Scales the vector by the given scalar
    // @param s number The scalar value.
    // @param x Vec2|Vec3|Vec4 The vector to scale.
    // @return Vec2|Vec3|Vec4 value The result of s*x.
    s["Math"]["vec_scale"] = sol::overload(
        math_helper::vec_scale<io_vec2_t>, math_helper::vec_scale<io_vec3_t>, math_helper::vec_scale<io_vec4_t>);
    // @function vec_div
    // @summary Divides the first vector by the second.
    // @param x Vec2|Vec3|Vec4 The first input value.
    // @param y Vec2|Vec3|Vec4 The second input value.
    // @return Vec2|Vec3|Vec4 value The result of x/y.
    s["Math"]["vec_div"] =
        sol::overload(math_helper::vec_div<io_vec2_t>, math_helper::vec_div<io_vec3_t>, math_helper::vec_div<io_vec4_t>);
    // @function vec_add
    // @summary Adds the provided vectors.
    // @param x Vec2|Vec3|Vec4 The first input value.
    // @param y Vec2|Vec3|Vec4 The second input value.
    // @return Vec2|Vec3|Vec4 value The result of x+y.
    s["Math"]["vec_add"] =
        sol::overload(math_helper::vec_add<io_vec2_t>, math_helper::vec_add<io_vec3_t>, math_helper::vec_add<io_vec4_t>);
    // @function vec_sub
    // @summary Subtracts the provided vectors.
    // @param x Vec2|Vec3|Vec4 The first input value.
    // @param y Vec2|Vec3|Vec4 The second input value.
    // @return Vec2|Vec3|Vec4 value The result of x-y.
    s["Math"]["vec_sub"] =
        sol::overload(math_helper::vec_sub<io_vec2_t>, math_helper::vec_sub<io_vec3_t>, math_helper::vec_sub<io_vec4_t>);
    // @function vec_length
    // @summary Calculates the length of the provided vector
    // @param x Vec2|Vec3|Vec4 The input value.
    // @return number value The length of the vector, calculated as sqrt(x.x*x.x + x.y*x.y + x.z*x.z).
    s["Math"]["vec_length"] = sol::overload(
        math_helper::vec_length<io_vec2_t>, math_helper::vec_length<io_vec3_t>, math_helper::vec_length<io_vec4_t>);
    // @function vec_length2
    // @summary Calculates the squared length of the provided vector
    // @param x Vec2|Vec3|Vec4 The input value.
    // @return number value The length of the vector, calculated as x.x*x.x + x.y*x.y + x.z*x.z.
    s["Math"]["vec_length2"] = sol::overload(
        math_helper::vec_length2<io_vec2_t>, math_helper::vec_length2<io_vec3_t>, math_helper::vec_length2<io_vec4_t>);
    // @function vec_distance
    // @summary Calculates the distance between the provided vectors.
    // @param x Vec2|Vec3|Vec4 The first input value.
    // @param y Vec2|Vec3|Vec4 The second input value.
    // @return number value The distance between the vectors, calculated as vec_length(vec_sub(y, x)).
    s["Math"]["vec_distance"] =
        sol::overload(math_helper::vec_distance<io_vec2_t>, math_helper::vec_distance<io_vec3_t>,
                      math_helper::vec_distance<io_vec4_t>);
    // @function vec_distance2
    // @summary Calculates the squared distance between the provided vectors.
    // @param x Vec2|Vec3|Vec4 The first input value.
    // @param y Vec2|Vec3|Vec4 The second input value.
    // @return number value The distance between the vectors, calculated as vec_length2(vec_sub(y, x)).
    s["Math"]["vec_distance2"] =
        sol::overload(math_helper::vec_distance2<io_vec2_t>, math_helper::vec_distance2<io_vec3_t>,
                      math_helper::vec_distance2<io_vec4_t>);
    // @function vec_normalize
    // @summary Normalizes the provided vector.
    // @param x Vec2|Vec3|Vec4 The input value.
    // @return Vec2|Vec3|Vec4 value The normalized vector of x, calculate as vec_scale(1.0 / vec_length(x), x).
    s["Math"]["vec_normalize"] =
        sol::overload(math_helper::vec_normalize<io_vec2_t>, math_helper::vec_normalize<io_vec3_t>,
                      math_helper::vec_normalize<io_vec4_t>);
    // @function vec_dot
    // @summary Calculates the dot product of the provided vectors.
    // @param x Vec2|Vec3|Vec4 The first input value.
    // @param y Vec2|Vec3|Vec4 The second input value.
    // @return number value The dot product of x and y.
    s["Math"]["vec_dot"] =
        sol::overload(math_helper::vec_dot<io_vec2_t>, math_helper::vec_dot<io_vec3_t>, math_helper::vec_dot<io_vec4_t>);
    // @function vec_cross
    // @summary Calculates the cross product of the provided vectors.
    // @param x Vec3 The first input value.
    // @param y Vec3 The second input value.
    // @return Vec3 value The cross product of x and y.
    s["Math"]["vec_cross"] = math_helper::vec_cross<io_vec3_t>;

    // @category Math_Quaternions Collection of functions to work with quaternions.

    // @function quat_mul
    // @summary Multiplies the provided quaternions
    // @param x Quat The first input value.
    // @param y Quat The second input value.
    // @return Quat value The result of x*y.
    s["Math"]["quat_mul"] = math_helper::vec_mul<io_quat_t>;
    // @function quat_rotate
    // @summary Rotates the provided vector by the given quaternion.
    // @param x Quat The quaternion to rotate by.
    // @param y Vec3 The vector to rotate.
    // @return Vec3 value The rotated vector.
    s["Math"]["quat_rotate"] = math_helper::quat_rotate;
    // @function quat_inverse
    // @summary Calculate the inverse of the provided quaternion.
    // @param x Quat The quaternion to invert.
    // @return Quat value The inverse of the quaternion.
    s["Math"]["quat_inverse"] = math_helper::quat_inverse;
    // @function quat_normalize
    // @summary Normalizes the provided quaternion.
    // @param x Quat The quaternion to normalize.
    // @return Quat value The normalized quaternion.
    s["Math"]["quat_normalize"] = math_helper::vec_normalize<io_quat_t>;
    // @function quat_look_at
    // @summary Calculates a quaternion "looking" into a specific direction.
    // @param dir Vec3 The direction to look into.
    // @param up Vec3 The up vector to use.
    // @return Quat value Quaternion "looking" into the provided direction.
    s["Math"]["quat_look_at"] = math_helper::quat_look_at;
    // @function quat_from_angle_axis
    // @summary Calculates a quaternion from the angle and axis.
    // @param angle number The angle.
    // @param axis Vec3 The axis of the rotation.
    // @return Quat value Quaternion rotating angle around axis.
    s["Math"]["quat_from_angle_axis"] = math_helper::quat_from_angle_axis;
    // @function quat_to_euler_angles
    // @summary Converts the provided quaternion to Euler angles.
    // @param x Quat The quaternion to convert.
    // @return Vec3 value The Euler angles, pitch, yaw, and roll derived from the given Quaternion in radians.
    s["Math"]["quat_to_euler_angles"] = math_helper::quat_to_euler_angles;
    // @function quat_from_euler_angles
    // @summary Converts the provided Euler angles to a quaternion.
    // @param angles Vec3 The Euler angles (in radians) to convert.
    // @return Quat value The quaternion constructed from the provided Euler angles.
    s["Math"]["quat_from_euler_angles"] = math_helper::quat_from_euler_angles;
    // @function quat_rotation
    // @summary Calculates a quaternion that transforms vector x to vector y.
    // @param x Vec3 The first vector.
    // @param y Vec3 The second vector.
    // @return Quat value Quaternion transforming vector to vector.y
    s["Math"]["quat_rotation"] = math_helper::quat_rotation;

  };

  s["Settings"] = s.create_table();
  s["Settings"]["load"] = [&s]() {
    // @namespace Settings
    // @category Settings Functions to interact with the global settings.
    // @copy_category Interface

    // @function set_bool
    // @summary Sets the given setting to the provided boolean value.
    // @param name string The name of the setting.
    // @param value boolean The value to set.
    s["Settings"]["set_bool"] = io_settings->set_bool;
    // @function set_uint
    // @summary Sets the given setting to the provided unsigned integer value.
    // @param name string The name of the setting.
    // @param value number The value to set.
    s["Settings"]["set_uint"] = io_settings->set_uint;
    // @function set_float
    // @summary Sets the given setting to the provided floating point value.
    // @param name string The name of the setting.
    // @param value number The value to set.
    s["Settings"]["set_float"] = io_settings->set_float;

    // @function get_bool
    // @summary Retrieves the given boolean setting.
    // @param name string The name of the setting to retrieve.
    // @return boolean value The value of the setting.
    s["Settings"]["get_bool"] = io_settings->get_bool;
    // @function get_uint
    // @summary Retrieves the given (unsigned) integer setting.
    // @param name string The name of the setting to retrieve.
    // @return number value The value of the setting.
    s["Settings"]["get_uint"] = io_settings->get_uint;
    // @function get_float
    // @summary Retrieves the given floating point setting.
    // @param name string The name of the setting to retrieve.
    // @return number value The value of the setting.
    s["Settings"]["get_float"] = io_settings->get_float;
  };

  s["Log"] = s.create_table();
  s["Log"]["load"] = [&s]() {
    // @namespace Log
    // @category Logging Functions to log to the console and log file.
    // @copy_category Interface

    // @function log_info
    // @summary Logs the given string as info.
    // @param s string The string to log.
    s["Log"]["log_info"] = io_logging->log_info;
    // @function log_warning
    // @summary Logs the given string as a warning.
    // @param s string The string to log.
    s["Log"]["log_warning"] = io_logging->log_warning;
    // @function log_error
    // @summary Logs the given string as an error.
    // @param s string The string to log.
    s["Log"]["log_error"] = io_logging->log_error;
  };

  // TODO:
  s["UI"] = s.create_table();
  s["UI"]["load"] = [&s]() {
    // @namespace UI
    // @category UI Functions to interact with the UI system.
    // @copy_category Interface

    // @function draw_rect
    // @summary Draws a rectangle.
    // @param color Vec4 The color of the rectangle.
    s["UI"]["draw_rect"] = [](io_vec4_t color) { io_ui->draw_rect(color); };
    // @function draw_direct
    // @summary Draws a circle.
    // @param color Vec4 The color of the circle.
    s["UI"]["draw_circle"] = [](io_vec4_t color) { io_ui->draw_circle(color); };
    // @function draw_ngon
    // @summary Draws a n-sided polygon.
    // @param color Vec4 The color of the n-sided polygon.
    // @param num_sides number The number of sided.
    s["UI"]["draw_ngon"] = [](io_vec4_t color, io_uint32_t num_sides) {
      io_ui->draw_ngon(color, num_sides);
    };

    // @function draw_image
    // @summary Draws the image with the given name.
    // @param name string The name of the image to draw.
    // @param tint Vec4 The tint of the image.
    s["UI"]["draw_image"] = [](const char* name, io_vec4_t tint) {
      io_ui->draw_image(name, tint);
    };
    // @function get_image_size
    // @summary Gets the size of the image (in px).
    // @param name string The name of the image.
    // @return Vec2 value The size of the image (in px).
    s["UI"]["get_image_size"] = [](const char* name) {
      return io_ui->get_image_size(name);
    };

    // @function draw_text
    // @summary Draws the text with the given options. 
    // @param text string The text to draw.
    // @param align_horizontal number The horizontal alignment.
    // @param align_vertical number The vertical alignment.
    // @param flags number The text flags.
    s["UI"]["draw_text"] =
        [](const char* text, io_ui_text_align_horizontal align_horizontal,
           io_ui_text_align_vertical align_vertical, io_ui_text_flag flags) {
          io_ui->draw_text(text, align_horizontal, align_vertical, flags);
        };
    // @function calc_text_bounds
    // @summary Calculates the bounds for the given text and options.
    // @param text string The text to compute the bounds for.
    // @param align_horizontal number The horizontal alignment.
    // @param align_vertical number The vertical alignment.
    // @param flags number The text flags.
    // @return Vec2 value The bounds for the text.
    s["UI"]["calc_text_bounds"] =
        [](const char* text, io_ui_text_align_horizontal align_horizontal,
           io_ui_text_align_vertical align_vertical, io_ui_text_flag flags) {
          return io_ui->calc_text_bounds(text, align_horizontal, align_vertical,
                                         flags);
        };
    // @function get_last_text_bounds
    // @summary Returns the bounds of the text that was drawn last.
    // @return Vec2 value The bounds for the text.
    s["UI"]["get_last_text_bounds"] = []() {
      return io_ui->get_last_text_bounds();
    };

    // @function push_transform
    // @summary Pushes the previous transform to the stack and activates the given one.
    // @param left UIAnchor Left anchor.
    // @param right UIAnchor Right anchor.
    // @param top UIAnchor Top anchor.
    // @param bottom UIAnchor Bottom anchor.
    // @param rotation number The rotation to apply.
    s["UI"]["push_transform"] = [](io_ui_anchor_t left, io_ui_anchor_t right,
                                   io_ui_anchor_t top, io_ui_anchor_t bottom,
                                   io_float32_t rotation) {
      io_ui->push_transform(left, right, top, bottom, rotation);
    };

    // @function push_transform_preset
    // @summary Pushes the previous transform to the stack and activates the given one.
    // @param preset number The transform preset to use.
    // @param offsets UIAnchorOffsets The offsets for each of the anchors.
    // @param rotation number The rotation to apply.
    s["UI"]["push_transform_preset"] = [](io_ui_anchor_preset preset,
                                          io_ui_anchor_offsets_t offsets,
                                          io_float32_t rotation) {
      io_ui->push_transform_preset(preset, offsets, rotation);
    };
    // @function pop_transform
    // @summary Pops the last transform off the stack and activates it.
    s["UI"]["pop_transform"] = []() { return io_ui->pop_transform(); };

    // @function push_scale_offset_for_base_size
    // @summary Calculates the scale and offset for the given base size and according to the aspect mode.
    // @param base_size Vec2 The base size.
    // @param aspect_mode number The aspect mode to use.
    s["UI"]["push_scale_offset_for_base_size"] =
        [](io_vec2_t base_size, io_ui_aspect_mode aspect_mode) {
          io_ui->push_scale_offset_for_base_size(base_size, aspect_mode);
        };

    // @function push_scale_offset
    // @summary Pushes the scale and offset to the stack and activates the given parameters.
    // @param scale number The uniform scaling factor.
    // @param offset Vec2 The offset.
    s["UI"]["push_scale_offset"] = [](io_float32_t scale, io_vec2_t offset) {
      io_ui->push_scale_offset(scale, offset);
    };

    // @function pop_scale_offset
    // @summary Pops the last scale and offset from the stack and activates it.
    s["UI"]["pop_scale_offset"] = []() { io_ui->pop_scale_offset(); };

    // @function push_style_var_float
    // @summary Pushes the current style variation float value to the stack and sets the given one.
    // @param var number The style variation.
    // @param value number The value to set.
    s["UI"]["push_style_var_float"] = [](io_ui_style_var var,
                                         io_float32_t value) {
      io_ui->push_style_var_float(var, value);
    };
    // @function push_style_var_vec4
    // @summary Pushes the current style variation Vec4 value to the stack and sets the given one.
    // @param var number The style variation.
    // @param value Vec4 The value to set.
    s["UI"]["push_style_var_vec4"] = [](io_ui_style_var var, io_vec4_t value) {
      io_ui->push_style_var_vec4(var, value);
    };
    // @function pop_style_var
    // @summary Pops the last style variation from the stack and activates it.
    s["UI"]["pop_style_var"] = []() { io_ui->pop_style_var(); };

    // @function clip_children
    // @summary Clips the children of the current transform
    s["UI"]["clip_children"] = []() { io_ui->clip_children(); };

    // @function push_font_size
    // @summary Pushes the current font size to the stack and sets the given one.
    // @param size number The font size (in px).
    s["UI"]["push_font_size"] = [](io_float32_t size) {
      io_ui->push_font_size(size);
    };

    // @function pop_font_size
    // @summary Pops the last font size from the stack and activates it.
    s["UI"]["pop_font_size"] = []() { io_ui->pop_font_size(); };

    // @function intersects
    // @summary Returns true if the given position (in px) intersects the current transform.
    s["UI"]["intersects"] = [](io_vec2_t position) {
      return io_ui->intersects(position);
    };
  };

  s["Random"] = s.create_table();
  s["Random"]["load"] = [&s]() {
    // @namespace Random
    // @category Random Collection of functions to calculate random values.
    // @copy_category Interface

    static io_uint64_t seed = 42ull;

    // @function set_seed
    // @summary Sets the seed used for the underlying random number generator (RNG).
    // @param seed number The seed to set.
    s["Random"]["set_seed"] = [](io_uint32_t s) { seed = s; };
    // @function rand_uint
    // @summary Calculates a random (unsigned) integer value.
    // @return number value The random value.
    s["Random"]["rand_uint"] = []() {
      return math_helper::calc_random_number_fast(seed);
    };
    // @function rand_uint_min_max
    // @summary Calculates a random (unsigned) integer value in the given interval.
    // @param min number The minimum random value to generate.
    // @param max number The maximum random value to generate.
    // @return number value The random value.
    s["Random"]["rand_uint_min_max"] = [](io_uint32_t min, io_uint32_t max) {
      return math_helper::calc_random_number_fast(seed) % (max - min) + min;
    };

    // @function rand_float
    // @summary Calculates a random floating point value in [0.0, 1.0].
    // @return number value The random value.
    s["Random"]["rand_float"] = []() {
      return math_helper::calc_random_float_min_max_fast(0.0f, 1.0f, seed);
    };

    // @function rand_float_min_max
    // @summary Calculates a random floating point value in the given interval.
    // @param min number The minimum random value to generate.
    // @param max number The maximum random value to generate.
    // @return number value The random value.
    s["Random"]["rand_float_min_max"] = [](io_float32_t min, io_float32_t max) {
      return math_helper::calc_random_float_min_max_fast(min, max, seed);
    };
  };

  s["Entity"] = s.create_table();
  s["Entity"]["load"] = [&s]() {

    // @namespace Entity
    // @category Entity Functions to interact with entities.
    // @copy_category Interface

    // @function get_type_id
    // @summary Gets the type id for all entities.
    // @return number value The type id for all entities.
    s["Entity"]["get_type_id"] = io_entity->get_type_id;

    // @function is_alive
    // @summary Returns whether the given entity is alive.
    // @param entity Ref The entity to check.
    // @return boolean value True if the given entity is alive.
    s["Entity"]["is_alive"] = io_entity->is_alive;

    // @function get_name
    // @summary Returns the name of the given entity.
    // @param entity Ref The entity to check.
    // @return string value The name of the given entity.
    s["Entity"]["get_name"] = io_entity->get_name;

    // @function find_first_entity_with_name
    // @summary Finds the first entity with the given name.
    // @param name string The name of the entity to search for.
    // @return Ref value The entity with the given name. Ref is invalid if none was found.
    s["Entity"]["find_first_entity_with_name"] = io_entity->find_first_entity_with_name;

    // @function find_entities_with_name
    // @summary Finds all entities with the given name and returns as table containg refs as result.
    // @param name string The name of the entities to search for.
    // @return table value Table containing the entities with the given name.
    s["Entity"]["find_entities_with_name"] = [](const char* name) {
      uint32_t num_entities;
      io_entity->find_entities_with_name(name, nullptr, &num_entities);
      std::vector<io_ref_t> entities(num_entities); 
      io_entity->find_entities_with_name(name, entities.data(), &num_entities);

      return entities;
    };

  };

  s["SaveData"] = s.create_table();
  s["SaveData"]["load"] = [&s]() {

    // @namespace SaveData
    // @category Save_Data Functions to interact with the save data system.
    // @copy_category Interface

    // @function save_to_user_data
    // @summary Stores the given node hierarchy using the given filename in the user data directory.
    // @param filename string The filename to use (without the file extension).
    // @param node Ref The node to save.
    s["SaveData"]["save_to_user_data"] = io_save_data->save_to_user_data;
    // @function load_from_user_data
    // @summary Loads the node hierarchy from the given file in the user data directory.
    // @param filename string The filename to load from (without the file extension).
    // @return Ref value The root node of the loaded hierarchy.
    s["SaveData"]["load_from_user_data"] = io_save_data->load_from_user_data;

  };

  s["World"] = s.create_table();
  s["World"]["load"] = [&s]() {

    // @namespace World
    // @category World Collection of functions that operate on a global level.
    // @copy_category Interface

    // @function get_root_node
    // @summary Returns the world's root node.
    // @return Ref value The root node of the world.
    s["World"]["get_root_node"] = io_world->get_root_node;
    // @function get_world_name
    // @summary Returns the name of the currently loaded world.
    // @return string value The name of the currently loaded world.
    s["World"]["get_world_name"] = io_world->get_world_name;

    // @function load_world
    // @summary Loads the world with the given name.
    // @param name string The name of the world to load.
    s["World"]["load_world"] = [](const char* world_name) { internal::queue_load_world(world_name); };
    // @function save_world
    // @summary Saves the current world under the given name.
    // @param name string The name of the world to save.
    s["World"]["save_world"] = io_world->save_world;

    // @function spawn_prefab
    // @summary Loads the prefab with the given name.
    // @param name string The name of the prefab to load.
    // @return Ref value The root node of the loaded prefab.
    s["World"]["spawn_prefab"] = io_world->spawn_prefab;

    // @function get_current_time_factor
    // @summary Returns the current time factor.
    // @return Ref value The current time factor.
    s["World"]["get_current_time_factor"] = io_world->get_current_time_factor;
    // @function push_time_factor
    // @summary Pushes the current time factor to the stack and applies the given one. Use this to speed up or slow down the game time.
    // @param factor number The time factor to apply.
    s["World"]["push_time_factor"] = io_world->push_time_factor;
    // @function pop_time_factor
    // @summary Pops the last time factor from the stack and activates it.
    s["World"]["pop_time_factor"] = io_world->pop_time_factor;

    // @function radius_damage
    // @summary Applies damage to all voxel shapes in the given radius in the world.
    // @param position Vec3 The position.
    // @param radius number The radius.
    // @param flags number The flags to use. Bit 1: Shade voxels around the impact area, Bit 2: Apply as fracturing.
    // @param max_hardness number Limits which voxels this damage operation can affect based on the hardness material parameter.
    s["World"]["radius_damage"] = io_world->radius_damage;

    // @function calc_mouse_ray
    // @summary Calculates a ray that points at the current mouse position in the world.
    // @return Vec3 origin The origin of the ray (in world coordinates).
    // @return Vec3 direction The direction of the ray.
    s["World"]["calc_mouse_ray"] = []() { io_vec3_t o, d; io_world->calc_mouse_ray(&o, &d); return std::make_tuple(o, d); };

    // @function highlight_node
    // @summary Highlights the given node using an outline shader.
    // @param node Ref The node to highlight.
    // @param color Vec4 The color of the highlight.
    // @param outline boolean Set to true to only highlight using an outline.
    s["World"]["highlight_node"] = io_world->highlight_node;

  };

  s["ParticleSystem"] = s.create_table();
  s["ParticleSystem"]["load"] = [&s]() {

    // @namespace ParticleSystem
    // @category Particle_System Functions to interact with the particle system.
    // @copy_category Interface

    // @function spawn_particle_emitter
    // @summary Spawns a new particle emitter.
    // @param effect string The name of the effect.
    // @param position Vec3 The spawn position of the emitter (in world coordinates).
    // @param lifetime number The lifetime of the emitter in seconds.
    // @param adjust_spawn_rate boolean Set to true to adjust the spawn rate based on the lifetime.
    // @return Handle value The handle for the emitter.
    s["ParticleSystem"]["spawn_particle_emitter"] = io_particle_system->spawn_particle_emitter;

    // @function despawn_particle_emitter
    // @summary Despawns the given particle emitter.
    // @param emitter Handle The emitter to despawn.
    s["ParticleSystem"]["despawn_particle_emitter"] = io_particle_system->despawn_particle_emitter;

    // @function attach_to_node
    // @summary Attaches the given particle emitter to the provided node.
    // @param emitter Handle The emitter to attach.
    // @param node Ref The node to attach the emitter to.
    s["ParticleSystem"]["attach_to_node"] = io_particle_system->attach_to_node;

    // @function set_spawn_rate
    // @summary Sets the spawn rate for the given emitter.
    // @param emitter Handle The emitter to adjust.
    // @param spawn_rate number The spawn rate to set.
    s["ParticleSystem"]["set_spawn_rate"] = io_particle_system->set_spawn_rate;

    // @function set_position
    // @summary Sets the position for the given emitter.
    // @param emitter Handle The emitter to adjust.
    // @param position Vec3 The position to set.
    s["ParticleSystem"]["set_position"] = io_particle_system->set_position;

    // @function set_scale
    // @summary Sets the scale for the given emitter.
    // @param emitter Handle The emitter to adjust.
    // @param position number The scale to set.
    s["ParticleSystem"]["set_scale"] = io_particle_system->set_scale;

    // @function set_emission_direction
    // @summary Sets the emission direction for the given emitter.
    // @param emitter Handle The emitter to adjust.
    // @param direction Vec3 The direction to set.
    s["ParticleSystem"]["set_emission_direction"] = io_particle_system->set_emission_direction;

  };

  s["Input"] = s.create_table();
  s["Input"]["load"] = [&s]() {

    // @namespace Input
    // @category Input Functions to interact with the input system.
    // @copy_category Interface

    // @function get_key_state
    // @summary Gets the current state of the given key.
    // @param key number The key to retrieve the state from (see Key table).
    // @param player number The index of the player to retrieve the state from.
    // @return number value The current state of the key (see KeyState table).
    s["Input"]["get_key_state"] = io_input_system->get_key_state;

    // @function get_axis_state
    // @summary Gets the current state of the given axis.
    // @param axis number The axis to retrieve the state from (see Axis table).
    // @param player number The index of the player to retrieve the state from.
    // @return number value The current state of the axis in [0.0, 1.0].
    s["Input"]["get_axis_state"] = io_input_system->get_axis_state;

    // @function get_mouse_pos
    // @summary Gets the current position of the mouse as pixel coordinates.
    // @return Vec2 value The position of the mouse.
    s["Input"]["get_mouse_pos"] = io_input_system->get_mouse_pos;

    // @function get_mouse_pos_viewport
    // @summary Gets the current position of the mouse in the viewport.
    // @return Vec2 value The position of the mouse in [0.0, 1.0].
    s["Input"]["get_mouse_pos_viewport"] = io_input_system->get_mouse_pos_viewport;

    // @function get_mouse_pos_relative
    // @summary Gets the delta movement of the mouse in pixels.
    // @return Vec2 value The delta movement of the mouse.
    s["Input"]["get_mouse_pos_relative"] = io_input_system->get_mouse_pos_relative;

    // @function request_mouse_cursor
    // @summary Request the mouse cursor to show. Has to be called every frame to keep the mouse cursor visible.
    s["Input"]["request_mouse_cursor"] = io_input_system->request_mouse_cursor;

    // @namespace Global
    // @category Globals Provides various global functions and types.

    // @table KeyState
    // @summary The different states a key can have.
    // @member kReleased number The key is released.
    // @member kPressed number The key is pressed.
    // @member kClicked number The key has been clicked (pressed and released).

    s["KeyState"] = s.create_table();
    s["KeyState"]["kReleased"] = io_input_key_state_released;
    s["KeyState"]["kPressed"] = io_input_key_state_pressed;
    s["KeyState"]["kClicked"] = io_input_key_state_clicked;

    // @table Key
    // @summary The different keys available.
    // @member kUp number
    // @member kDown number
    // @member kLeft number
    // @member kRight number
    // @member kA number
    // @member kB number
    // @member kC number
    // @member kD number
    // @member kE number
    // @member kF number
    // @member kG number
    // @member kH number
    // @member kI number
    // @member kJ number
    // @member kK number
    // @member kL number
    // @member kM number
    // @member kN number
    // @member kO number
    // @member kP number
    // @member kQ number
    // @member kR number
    // @member kS number
    // @member kT number
    // @member kU number
    // @member kV number
    // @member kW number
    // @member kX number
    // @member kY number
    // @member kZ number
    // @member k0 number
    // @member k1 number
    // @member k2 number
    // @member k3 number
    // @member k4 number
    // @member k5 number
    // @member k6 number
    // @member k7 number
    // @member k8 number
    // @member k9 number
    // @member kF1 number
    // @member kF2 number
    // @member kF3 number
    // @member kF4 number
    // @member kF5 number
    // @member kF6 number
    // @member kF7 number
    // @member kF8 number
    // @member kF9 number
    // @member kF10 number
    // @member kF11 number
    // @member kF12 number
    // @member kDel number
    // @member kBackspace number
    // @member kTab number
    // @member kMouseLeft number
    // @member kMouseRight number
    // @member kMouseMiddle number
    // @member kShift number
    // @member kAlt number
    // @member kCtrl number
    // @member kSpace number
    // @member kEscape number
    // @member kReturn number
    // @member kNumPlus number
    // @member kNumMinus number
    // @member kNum0 number
    // @member kNum1 number
    // @member kNum2 number
    // @member kNum3 number
    // @member kNum4 number
    // @member kNum5 number
    // @member kNum6 number
    // @member kNum7 number
    // @member kNum8 number
    // @member kNum9 number
    // @member kControllerButtonA number
    // @member kControllerButtonY number
    // @member kControllerButtonB number
    // @member kControllerButtonX number
    // @member kAny number

    s["Key"] = s.create_table();
    s["Key"]["kUp"] = io_input_key_up;
    s["Key"]["kDown"] = io_input_key_down;
    s["Key"]["kLeft"] = io_input_key_left;
    s["Key"]["kRight"] = io_input_key_right;

    s["Key"]["kDel"] = io_input_key_del;
    s["Key"]["kBackspace"] = io_input_key_backspace;
    s["Key"]["kTab"] = io_input_key_tab;

    s["Key"]["kMouseLeft"] = io_input_key_mouse_left;
    s["Key"]["kMouseRight"] = io_input_key_mouse_right;
    s["Key"]["kMouseMiddle"] = io_input_key_mouse_middle;

    s["Key"]["kShift"] = io_input_key_mouse_left;
    s["Key"]["kAlt"] = io_input_key_mouse_right;
    s["Key"]["kCtrl"] = io_input_key_ctrl;
    s["Key"]["kSpace"] = io_input_key_space;
    s["Key"]["kEscape"] = io_input_key_escape;
    s["Key"]["kReturn"] = io_input_key_return;
    s["Key"]["kNumPlus"] = io_input_key_num_plus;
    s["Key"]["kNumMinus"] = io_input_key_num_minus;

    s["Key"]["kControllerButtonA"] = io_input_key_controller_button_a;
    s["Key"]["kControllerButtonY"] = io_input_key_controller_button_y;
    s["Key"]["kControllerButtonB"] = io_input_key_controller_button_b;
    s["Key"]["kControllerButtonX"] = io_input_key_controller_button_x;

    s["Key"]["kAny"] = io_input_key_any;

    // Letters
    for (uint32_t i = 0u; i < 26u; ++i)
    {
      stbsp_snprintf(string_buffer, string_buffer_length, "k%c", 'A' + i);
      s["Key"][string_buffer] = io_input_key_a + i;
    }

    // Number and num keys
    for (uint32_t i = 0u; i < 10u; ++i)
    {
      stbsp_snprintf(string_buffer, string_buffer_length, "k%u", i);
      s["Key"][string_buffer] = io_input_key_0 + i;
      stbsp_snprintf(string_buffer, string_buffer_length, "kNum%u", i);
      s["Key"][string_buffer] = io_input_key_num_0 + i;
    }

    // Function keys
    for (uint32_t i = 0u; i < 12u; ++i)
    {
      stbsp_snprintf(string_buffer, string_buffer_length, "kF%u", i);
      s["Key"][string_buffer] = io_input_key_f1 + i;
    }

    // @table Axis
    // @summary The different axes for controller input handling.
    // @member kLeftX number Left stick x-axis.
    // @member kLeftY number Left stick y-axis.
    // @member kRightX number Right stick x-axis.
    // @member kRightY number Right stick y-axis.
    // @member kTriggerLeft number Left trigger.
    // @member kTriggerRight number Right trigger.

    s["Axis"] = s.create_table();
    s["Axis"]["kLeftX"] = io_input_axis_left_x;
    s["Axis"]["kLeftY"] = io_input_axis_left_y;
    s["Axis"]["kRightX"] = io_input_axis_right_x;
    s["Axis"]["kRightY"] = io_input_axis_right_y;
    s["Axis"]["kTriggerLeft"] =io_input_axis_trigger_left;
    s["Axis"]["kTriggerRight"] = io_input_axis_trigger_right;

  };

  s["Terrain"] = s.create_table();
  s["Terrain"]["load"] = [&s]() {

    if (!io_plugin_terrain)
    {
      io_logging->log_warning("Terrain plugin is not available.");
      return;
    }

    // @namespace Terrain
    // @category Terrain Functions to interact with the terrain generator.
    // @copy_category Interface

    // @function generate_from_image
    // @summary Generates a new heightmap-based terrain from the given image.
    // @param filename string The filename of the image.
    // @param palette string The name of the palette to use.
    // @param max_height number The maximum height of the terrain in world coordinates.
    // @param voxel_size number The size of a single voxel.
    // @return Ref value The root node of the terrain.
    s["Terrain"]["generate_from_image"] = io_plugin_terrain->generate_from_image;
    // @function generate_from_data
    // @summary Generates a new heightmap-based terrain from the table.
    // @param data table The heightmap data as a table made up of pixels (see Terrain.Pixel)
    // @param size number The width/height of the terrain in pixels.
    // @param palette string The name of the palette to use.
    // @param max_height number The maximum height of the terrain in world coordinates.
    // @param voxel_size number The size of a single voxel.
    // @return Ref value The root node of the terrain.
    s["Terrain"]["generate_from_data"] = [](const sol::table& heightmap, io_uint32_t size, const char* palette_name, io_float32_t max_height, io_float32_t voxel_size)
    {
      std::vector<io_plugin_terrain_heightmap_pixel> data(heightmap.size());
      for (uint32_t i=0u; i<heightmap.size(); ++i)
        data[i] = heightmap[1u + i];

      return io_plugin_terrain->generate_from_data(data.data(), size, palette_name, max_height, voxel_size);
    };

    // @function HeightmapPixel
    // @summary Initializes a new pixel for the terrain generator.
    // @param height number The height in [0.0, 1.0].
    // @param grass_height number The grass height in [0.0, 1.0].
    // @param palette_index number The palette index.
    // @return HeightmapPixel value The new heightmap pixel.
    s["Terrain"]["HeightmapPixel"] = io_plugin_terrain_create_heightmap_pixel;

  };

  s["Noise"] = s.create_table();
  s["Noise"]["load"] = [&s]() {

    // @namespace Noise
    // @category Noise Various procedural noise functions. Useful for, e.g., generating terrain, animations, etc.
    // @copy_category Interface

    // @function simplex
    // @summary Calculates perlin noise.
    // @param x Vec4 The coordinate to calculate the noise value at.
    // @return number value The noise value at the given coordinate.
    s["Noise"]["perlin"] = [](const io_vec4_t& x) { return glm::perlin(io_cvt(x)); };

    // @function simplex
    // @summary Calculates simplex noise.
    // @param x Vec4 The coordinate to calculate the noise value at.
    // @return number value The noise value at the given coordinate.
    s["Noise"]["simplex"] = [](const io_vec4_t& x) { return glm::simplex(io_cvt(x)); };

  };

  s["Physics"] = s.create_table();
  s["Physics"]["load"] = [&s]() {

    // @namespace Physics
    // @category Physics Functions to interact with the various physics subsystems.
    // @copy_category Interface

    // @function set_gravity
    // @summary Sets the global gravity (in meters per second).
    // @param gravity Vec3 The gravity to set.
    s["Physics"]["set_gravity"] = io_physics->set_gravity;
    // @function get_gravity
    // @summary Gets the global gravity (in meters per second).
    // @return Vec3 value The current global gravity.
    s["Physics"]["get_gravity"] = io_physics->get_gravity;

    // @function overlap_sphere
    // @summary Performs a sphere overlap test against the physics geometry.
    // @param position Vec3 The position of the sphere in world coordinates.
    // @param radius number The radius of the sphere.
    // @return boolean hit True if a blocking hit is detected.
    // @return Ref hit_entity The blocking entity. Invalid ref if no hit was detected.
    s["Physics"]["overlap_sphere"] = [](io_vec3_t pos, io_float32_t radius) {
      auto result = io_physics->overlap_sphere(pos, radius);
      return std::make_tuple(result.hit, result.entity);
    };
    // @function sweep_sphere
    // @summary Performs a sweeped sphere test against the physics geometry.
    // @param position Vec3 The position of the sphere in world coordinates.
    // @param radius number The radius of the sphere.
    // @param direction Vec3 The direction to perform the sweep test in.
    // @param distance number The distance to sweep.
    // @return boolean hit True if a blocking hit is detected.
    // @return number hit_distance The distance to the hit.
    // @return Vec3 hit_position The position of the hit.
    // @return Vec3 hit_normal The normal of the hit.
    // @return Ref hit_entity The blocking entity. Invalid ref if no hit was detected.
    s["Physics"]["sweep_sphere"] = [](io_vec3_t pos, io_float32_t radius, io_vec3_t dir, io_float32_t dist) {
      auto result = io_physics->sweep_sphere(pos, radius, dir, dist);
      return std::make_tuple(result.hit, result.distance, result.position, result.normal, result.entity);
    };
    // @function raycast
    // @summary Performs a raycast again the physics geometry.
    // @param ray_origin Vec3 The origin of the ray in world coordinates.
    // @param ray_direction Vec3 The direction of ray.
    // @param ray_distance number The maximum distance the ray should travel.
    // @return boolean hit True if a blocking hit is detected.
    // @return number hit_distance The distance to the hit.
    // @return Vec3 hit_position The position of the hit.
    // @return Vec3 hit_normal The normal of the hit.
    // @return Ref hit_entity The blocking entity. Invalid ref if no hit was detected.
    s["Physics"]["raycast"] = [](io_vec3_t pos, io_vec3_t dir, io_float32_t dist) {
      auto result = io_physics->raycast(pos, dir, dist);
      return std::make_tuple(result.hit, result.distance, result.position, result.normal, result.entity);
    };

  };

  s["DebugGeometry"] = s.create_table();
  s["DebugGeometry"]["load"] = [&s]() {

    // @namespace DebugGeometry
    // @category Debug_Geometry Functions to quickly draw geometry for debugging purposes.
    // @copy_category Interface

    // @function draw_line
    // @summary Draws a line.
    // @param start_pos Vec3 Line start position.
    // @param end_pos Vec3 Line end position.
    // @param color Vec4 The color of the line.
    // @param always_in_front boolean Set to true to render the line in front of all the other geometry.
    s["DebugGeometry"]["draw_line"] = io_debug_geometry->draw_line;
    // @function draw_sphere
    // @summary Draws a sphere.
    // @param position Vec3 The center position of the sphere.
    // @param radius number The radius of the sphere.
    // @param color Vec4 The color of the sphere.
    // @param always_in_front boolean Set to true to render the sphere in front of all the other geometry.
    s["DebugGeometry"]["draw_sphere"] = io_debug_geometry->draw_sphere;

  };

  s["Sound"] = s.create_table();
  s["Sound"]["load"] = [&s]() {

    // @namespace Sound
    // @category Sound Functions to interact with the sound system.
    // @copy_category Interface

    // @function play_sound_effect
    // @summary Starts playing the sound effect with the given name.
    // @param name string The name of the sound effect to play.
    // @return Handle handle The handle of the created sound effect instance.
    s["Sound"]["play_sound_effect"] = io_sound->play_sound_effect;
    // @function stop_sound_effect
    // @summary Stops playing the given sound effect instance.
    // @param handle Handle The handle of the sound effect instance.
    s["Sound"]["stop_sound_effect"] = io_sound->stop_sound_effect;
    // @function set_position
    // @summary Sets the position of the given sound effect instance in the world. Used for spatial audio effects.
    // @param handle Handle The handle of the sound effect instance.
    // @param pos Vec3 The position to set.
    s["Sound"]["set_position"] = io_sound->set_position;

    // @function get_spectrum
    // @summary Retrieves the current snapshot of the sound spectrum.
    // @return table spectrum Table containing the bins of the sound spectrum.
    s["Sound"]["get_spectrum"] = []() {
      const io_float32_t* d;
      io_uint32_t d_length;
      io_sound->get_spectrum(&d, &d_length);

      std::vector<io_float32_t> spectrum;
      spectrum.resize(d_length);
      memcpy(spectrum.data(), d, sizeof(io_float32_t) * d_length);

      return spectrum;
    };

  };

  s["Pathfinding"] = s.create_table();
  s["Pathfinding"]["load"] = [&s]() {

    // @namespace Pathfinding
    // @category Pathfinding Functions to interact with the pathfinding system.
    // @copy_category Interface

    // @function find_path
    // @summary Starts finding a path.
    // @param start_pos Vec3 The start position in the world.
    // @param end_pos Vec3 The end position in the world.
    // @param settings PathSettings The path settings to use for finding the path.
    // @return Handle handle The handle of the path.
    s["Pathfinding"]["find_path"] = io_pathfinding->find_path;
    // @function is_valid
    // @summary Checks if the given path is valid.
    // @param handle Handle The handle of the path.
    // @return boolean value True if the path is valid.
    s["Pathfinding"]["is_valid"] = io_pathfinding->is_valid;
    // @function reset_path
    // @summary Resets the given path.
    // @param handle Handle The handle of the path.
    s["Pathfinding"]["reset_path"] = io_pathfinding->reset_path;
    // @function destroy_path
    // @summary Destroys the given path.
    // @param handle Handle The handle of the path.
    s["Pathfinding"]["destroy_path"] = io_pathfinding->destroy_path;
    // @function is_path_found
    // @summary Check if a valid path has been found.
    // @param handle Handle The handle of the path.
    // @return boolean value True if a path has been found.
    s["Pathfinding"]["is_path_found"] = io_pathfinding->is_path_found;
    // @function get_next_position_on_path
    // @summary Returns the next position on the path.
    // @param handle Handle The handle of the path.
    // @return Vec3 position The next position on the path.
    // @return boolean success True if a new position was retrieved.
    s["Pathfinding"]["get_next_position_on_path"] = [](io_handle16_t handle) {
      io_vec3_t p;
      const bool result = io_pathfinding->get_next_position_on_path(handle, &p);
      return std::make_tuple(p, result);
    };
    // @function draw_debug_geometry
    // @summary Draws the internal debug geometry for the given path.
    // @param handle Handle The handle of the path.
    // @param in_front boolean Set to true to draw the debug geometry in front of all other geometry.
    s["Pathfinding"]["draw_debug_geometry"] = io_pathfinding->draw_debug_geometry;
    // @function draw_path
    // @summary Draws the path using lines. 
    // @param handle Handle The handle of the path.
    // @param color Vec4 The color used for drawing.
    // @param in_front boolean Set to true to draw the path in front of all other geometry.
    s["Pathfinding"]["draw_path"] = io_pathfinding->draw_path;

    // @function PathSettings
    // @summary Creates and initializes the settings for calculating a path to the default values.
    // @return PathSettings value The path settings.
    s["Pathfinding"]["PathSettings"] = []() { 
      io_pathfinding_path_settings_t settings; 
      io_pathfinding_init_path_settings(&settings);
      return settings;
    };

  };

#define SHARED_COMPONENT_INTERFACE_IMPL(t_, i_)                                \
  t_["get_type_id"] = i_->base.get_type_id;                                    \
  t_["create"] = i_->base.create;                                              \
  t_["destroy"] = i_->base.destroy;                                            \
  t_["commit_changes"] = i_->base.commit_changes;                              \
  t_["get_num_active_components"] = i_->base.get_num_active_components;        \
  t_["get_component_for_entity"] = i_->base.get_component_for_entity;          \
  t_["is_alive"] = i_->base.is_alive;                                          \
  t_["get_entity"] = i_->base.get_entity;                                      \
  t_["set_property"] = i_->base.set_property;                                  \
  t_["get_property"] = i_->base.get_property;                                  \
  t_["list_properties"] = []() {                                               \
    uint32_t numProperties;                                                    \
    i_->base.list_properties(nullptr, &numProperties);                         \
    std::vector<io_property_desc_t> properties(numProperties);                 \
    i_->base.list_properties(properties.data(), &numProperties);               \
    return properties;                                                         \
  }

  s["CustomData"] = s.create_table();
  s["CustomData"]["load"] = [&s]() {

    // @namespace CustomData
    // @category Custom_Data_Component Functions to interact with custom datas.
    // @copy_category Interface
    // @copy_category Components

    SHARED_COMPONENT_INTERFACE_IMPL(s["CustomData"], io_component_custom_data);

    // @function get
    // @summary Gets the value for the given index.
    // @param component Ref The custom data component.
    // @param index number The index of the value to retrieve.
    // @return Variant value The value for the given index.
    s["CustomData"]["get"] = io_component_custom_data->get;
    // @function set
    // @summary Sets the value for the given index.
    // @param component Ref The custom data component.
    // @param index number The index of the value to retrieve.
    // @param value Variant The value to set.
    s["CustomData"]["set"] = io_component_custom_data->set;
    // @function add
    // @summary Adds a new value.
    // @param component Ref The custom data component.
    // @param value Variant The value to add.
    s["CustomData"]["add"] = io_component_custom_data->add;
    // @function remove
    // @summary Adds a new value.
    // @param component Ref The custom data component.
    // @param index number The index of the value to remove.
    s["CustomData"]["remove"] = io_component_custom_data->remove;

  };

  s["Tag"] = s.create_table();
  s["Tag"]["load"] = [&s]() {

    SHARED_COMPONENT_INTERFACE_IMPL(s["Tag"], io_component_tag);

    // @namespace Tag
    // @category Tag_Component Functions to interact with tags.
    // @copy_category Interface
    // @copy_category Components

    // @function find_entities_with_tag
    // @summary Finds and returns all entities with the given tag.
    // @param tag string The tag to search for.
    // @return table value Table containing all entities with the given tag.
    s["Tag"]["find_entities_with_tag"] = [](const char* tag) {
      uint32_t num_entities;
      io_component_tag->find_entities_with_tag(tag, nullptr, &num_entities);
      std::vector<io_ref_t> entities(num_entities);
      io_component_tag->find_entities_with_tag(tag, entities.data(),
                                               &num_entities);

      return entities;
    };

  };

  s["FlipbookAnimation"] = s.create_table();
  s["FlipbookAnimation"]["load"] = [&s]() {

    SHARED_COMPONENT_INTERFACE_IMPL(s["FlipbookAnimation"], io_component_flipbook_animation);

    // @namespace FlipbookAnimation
    // @category Flipbook_Animation_Component Functions to interact with flipbook animations.
    // @copy_category Interface
    // @copy_category Components

    // @function play
    // @summary Starts playing the flipbook animation.
    // @param component Ref The component in question.
    s["FlipbookAnimation"]["play"] = io_component_flipbook_animation->play;
    // @function stop
    // @summary Stops playing the flipbook animation.
    // @param component Ref The component in question.
    s["FlipbookAnimation"]["stop"] = io_component_flipbook_animation->stop;

  };

  s["PostEffectVolume"] = s.create_table();
  s["PostEffectVolume"]["load"] = [&s]() {

    SHARED_COMPONENT_INTERFACE_IMPL(s["PostEffectVolume"], io_component_post_effect_volume);

    // @namespace PostEffectVolume
    // @category Post_Effect_Volume_Component Functions to interact with post effect volumes.
    // @copy_category Interface
    // @copy_category Components

  };

  s["Camera"] = s.create_table();
  s["Camera"]["load"] = [&s]() {

    SHARED_COMPONENT_INTERFACE_IMPL(s["Camera"], io_component_camera);

    // @namespace Camera
    // @category Camera_Component Functions to interact with cameras.
    // @copy_category Interface
    // @copy_category Components

  };

  s["Script"] = s.create_table();
  s["Script"]["load"] = [&s]() {

    SHARED_COMPONENT_INTERFACE_IMPL(s["Script"], io_component_script);

    // @namespace Script
    // @category Script_Component Functions to interact with scripts.
    // @copy_category Interface
    // @copy_category Components

  };

  s["VoxelShape"] = s.create_table();
  s["VoxelShape"]["load"] = [&s]() {

    SHARED_COMPONENT_INTERFACE_IMPL(s["VoxelShape"], io_component_voxel_shape);

    // @namespace VoxelShape
    // @category Voxel_Shape_Component Functions to interact with voxel shapes.
    // @copy_category Interface
    // @copy_category Components

    // @function set
    // @summary Sets the voxel to the given palette index.
    // @param component Ref The voxel shape component.
    // @param coord U8Vec3 The coordinate of the voxel.
    // @param palette_index number The palette index to set.
    s["VoxelShape"]["set"] = io_component_voxel_shape->set;
    // @function set_unsafe
    // @summary Sets the voxel to the given palette index (without boundary checks).
    // @param component Ref The voxel shape component.
    // @param coord U8Vec3 The coordinate of the voxel.
    // @param palette_index number The palette index to set.
    s["VoxelShape"]["set_unsafe"] = io_component_voxel_shape->set_unsafe;
    // @function get
    // @summary Gets the palette index for a voxel.
    // @param component Ref The voxel shape component.
    // @param coord U8Vec3 The coordinate of the voxel.
    // @return number value The palette index of the given voxel.
    s["VoxelShape"]["get"] = io_component_voxel_shape->get;
    // @function fill
    // @summary Fills all voxels in the range defined by min and max.
    // @param component Ref The voxel shape component.
    // @param min U8Vec3 The min voxel coorindate of the area.
    // @param max U8Vec3 The max voxel coordinate of the area.
    // @param palette_index number The palette index to set.
    s["VoxelShape"]["fill"] = io_component_voxel_shape->fill;
    // @function copy
    // @summary Copies the given source shape into the target shape.
    // @param target Ref The target voxel shape component.
    // @param source Ref The source voxel shape component.
    s["VoxelShape"]["copy"] = [](io_ref_t target, io_ref_t source) {
      const auto target_dim = io_cvt(io_component_voxel_shape->get_dim(target));
      const auto source_dim = io_cvt(io_component_voxel_shape->get_dim(source));

      const auto target_data = io_component_voxel_shape->get_voxel_data(target);
      const auto source_data = io_component_voxel_shape->get_voxel_data(source);

      const auto copy_dim = glm::min(target_dim, source_dim);

      for (uint32_t z = 0u; z < copy_dim.z; ++z)
      {
        const uint32_t szo = z * source_dim.x * source_dim.y;
        const uint32_t tzo = z * target_dim.x * target_dim.y;

        for (uint32_t y = 0u; y < copy_dim.y; ++y)
        {
          const uint32_t syo = y * source_dim.x;
          const uint32_t tyo = y * target_dim.x;
          const uint32_t syzo = syo + szo;
          const uint32_t tyzo = tyo + tzo;

          for (uint32_t x = 0u; x < copy_dim.x; ++x)
            target_data[x + tyzo] = source_data[x + syzo];
        }
      }
    };

    // @function get_dim
    // @summary Gets the dimensions of the shape in voxels.
    // @param component Ref The voxel shape component.
    // @return U16Vec3 value The dimensions of the shape in voxels.
    s["VoxelShape"]["get_dim"] = io_component_voxel_shape->get_dim;

    // @function voxelize
    // @summary Queues this shape for voxelization
    // @param component Ref The voxel shape component.
    s["VoxelShape"]["voxelize"] = io_component_voxel_shape->voxelize;

    // @function apply_force_at_world_position
    // @summary Applies a force at the given world position.
    // @param component Ref The voxel shape component.
    // @param force Vec3 The force vector to apply.
    // @param position Vec3 The position to apply the force at (world coordinates).
    s["VoxelShape"]["apply_force_at_world_position"] =
        io_component_voxel_shape->apply_force_at_world_position;
    // @function apply_force_at_local_position
    // @summary Applies a force at the given local position.
    // @param component Ref The voxel shape component.
    // @param force Vec3 The force vector to apply.
    // @param position Vec3 The position to apply the force at (local coordinates).
    s["VoxelShape"]["apply_force_at_local_position"] =
        io_component_voxel_shape->apply_force_at_local_position;
    // @function apply_force
    // @summary Applies a force at the center of mass.
    // @param component Ref The voxel shape component.
    // @param force Vec3 The force vector to apply.
    s["VoxelShape"]["apply_force"] = io_component_voxel_shape->apply_force;
    // @function apply_torque
    // @summary Applies a torque at the center of mass.
    // @param component Ref The voxel shape component.
    // @param torque Vec3 The torque vector to apply.
    s["VoxelShape"]["apply_torque"] = io_component_voxel_shape->apply_torque;

    // @function get_linear_velocity
    // @summary Gets the linear velocity of the shape.
    // @param component Ref The voxel shape component.
    // @return Vec3 value The linear velocity of the shape.
    s["VoxelShape"]["get_linear_velocity"] = io_component_voxel_shape->get_linear_velocity;
    // @function set_linear_velocity
    // @summary Sets the linear velocity of the shape.
    // @param component Ref The voxel shape component.
    // @param velocity Vec3 The linear velocity to set.
    s["VoxelShape"]["set_linear_velocity"] = io_component_voxel_shape->set_linear_velocity;
    // @function get_angular_velocity
    // @summary Gets the angular velocity of the shape.
    // @param component Ref The voxel shape component.
    // @return Vec3 value The angular velocity of the shape.
    s["VoxelShape"]["get_angular_velocity"] = io_component_voxel_shape->get_angular_velocity;
    // @function set_angular_velocity
    // @summary Sets the angular velocity of the shape.
    // @param component Ref The voxel shape component.
    // @param velocity Vec3 The angular velocity to set.
    s["VoxelShape"]["set_angular_velocity"] = io_component_voxel_shape->set_angular_velocity;

  };

  s["Light"] = s.create_table();
  s["Light"]["load"] = [&s]() {

    SHARED_COMPONENT_INTERFACE_IMPL(s["Light"], io_component_light);

    // @namespace Light
    // @category Light_Component Functions to interact with lights.
    // @copy_category Interface
    // @copy_category Components

  };

  s["Node"] = s.create_table();
  s["Node"]["load"] = [&s]() {

    // @namespace Node
    // @category Node_Component Functions to interact with nodes.

    // @function get_type_id
    // @summary Returns the type ID of the component.
    // @param component Ref The component.
    // @return number value The type ID of the component.
    s["Node"]["get_type_id"] = io_component_node->base.get_type_id;

    // @function get_num_active_components
    // @summary Returns the total number of active components of this type.
    // @return number value Total total number of active components of this type.
    s["Node"]["get_num_active_components"] = io_component_node->base.get_num_active_components;

    // @function get_component_for_entity
    // @summary Returns the component for the given entity.
    // @param entity Ref The entity.
    // @return Ref value The component for the given entity.
    s["Node"]["get_component_for_entity"] = io_component_node->base.get_component_for_entity;

    // @function is_alive
    // @summary Returns true if the referenced component is alive.
    // @param component Ref The component.
    // @return boolean value True if the component is alive.
    s["Node"]["is_alive"] = io_component_node->base.is_alive;

    // @function get_entity
    // @summary Returns the entity the component is assigned to.
    // @param component Ref The component.
    // @return Ref value The entity the component is assigned to.
    s["Node"]["get_entity"] = io_component_node->base.get_entity;

    // @function create
    // @summary Creates a new node component with the given name and attaches it to the root node of the world.
    // @param name string The name of the new node component.
    // @return Ref value The new node component.

    // @function create
    // @summary Creates a new node component with the given name and attaches it to the provided node.
    // @param parent_node Ref The node to attach the new node to.
    // @param name string The name of the new node component.
    // @return Ref value The new node component.

    s["Node"]["create"] =
        sol::overload(io_component_node->create, io_component_node->create_with_parent);

    // @function destroy
    // @summary Destroys the given node component.
    // @param node Ref The node to destroy.
    s["Node"]["destroy"] = [](io_ref_t node) { internal::queue_destroy_node(node); };

    // @function get_parent
    // @summary Gets the parent node (if any).
    // @param node Ref The node in question.
    // @return Ref value The parent of the node (if any).
    s["Node"]["get_parent"] = io_component_node->get_parent;
    // @function get_next_sibling
    // @summary Gets the next sibling of the node (if any).
    // @param node Ref The node in question.
    // @return Ref value The next sibling of the node (if any).
    s["Node"]["get_next_sibling"] = io_component_node->get_next_sibling;
    // @function get_prev_sibling
    // @summary Gets the previous sibling of the node (if any).
    // @param node Ref The node in question.
    // @return Ref value The previous sibling of the node (if any).
    s["Node"]["get_prev_sibling"] = io_component_node->get_prev_sibling;

    // @function set_hidden
    // @summary Sets the hidden state of the node.
    // @param node Ref The node in question.
    // @param hidden boolean Pass true if the node should be hidden.
    s["Node"]["set_hidden"] = io_component_node->set_hidden;
    // @function is_hidden
    // @summary Retrieves the hidden state of the node.
    // @param node Ref The node in question.
    // @return boolean value True if the node is hidden.
    s["Node"]["is_hidden"] = io_component_node->is_hidden;

    // @function get_position
    // @summary Gets the (local) position of the node.
    // @param node Ref The node in question.
    // @return Vec3 value The (local) position of the node.
    s["Node"]["get_position"] = io_component_node->get_position;
    // @function get_world_position
    // @summary Gets the (world) position of the node.
    // @param node Ref The node in question.
    // @return Vec3 value The (world) position of the node.
    s["Node"]["get_world_position"] = io_component_node->get_world_position;
    // @function get_orientation
    // @summary Gets the (local) orientation of the node.
    // @param node Ref The node in question.
    // @return Quat value The (local) orientation of the node.
    s["Node"]["get_orientation"] = io_component_node->get_orientation;
    // @function get_world_orientation
    // @summary Gets the (world) orientation of the node.
    // @param node Ref The node in question.
    // @return Quat value The (world) orientation of the node.
    s["Node"]["get_world_orientation"] = io_component_node->get_world_orientation;
    // @function get_size
    // @summary Gets the (local) size of the node.
    // @param node Ref The node in question.
    // @return Vec3 value The (local) size of the node.
    s["Node"]["get_size"] = io_component_node->get_size;
    // @function get_world_size
    // @summary Gets the (world) size of the node.
    // @param node Ref The node in question.
    // @return Vec3 value The (world) size of the node.
    s["Node"]["get_world_size"] = io_component_node->get_world_size;
    // @function set_position
    // @summary Sets the (local) position of the node.
    // @param node Ref The node in question.
    // @param position Vec3 The (local) position of the node.
    s["Node"]["set_position"] = io_component_node->set_position;
    // @function set_world_position
    // @summary Sets the (world) position of the node.
    // @param node Ref The node in question.
    // @param position Vec3 The (world) position of the node.
    s["Node"]["set_world_position"] = io_component_node->set_world_position;
    // @function set_orientation
    // @summary Sets the (local) orientation of the node.
    // @param node Ref The node in question.
    // @param orientation Quat The (local) orientation of the node.
    s["Node"]["set_orientation"] = io_component_node->set_orientation;
    // @function set_world_orientation
    // @summary Sets the (world) orientation of the node.
    // @param node Ref The node in question.
    // @param orientation Quat The (world) orientation of the node.
    s["Node"]["set_world_orientation"] = io_component_node->set_world_orientation;
    // @function set_size
    // @summary Sets the (local) size of the node.
    // @param node Ref The node in question.
    // @param size Vec3 The (local) size of the node.
    s["Node"]["set_size"] = io_component_node->set_size;
    // @function set_world_size
    // @summary Sets the (world) size of the node.
    // @param node Ref The node in question.
    // @param size Vec3 The (world) size of the node.
    s["Node"]["set_world_size"] = io_component_node->set_world_size;

    // @function to_local_space
    // @summary Transforms the provided position into the local space of the given node.
    // @param node Ref The node used for the transformation.
    // @param position Vec3 The position to transform.
    // @return Vec3 value The position transformed into the local node space.
    s["Node"]["to_local_space"] = io_component_node->to_local_space;
    // @function to_local_space_direction
    // @summary Transforms the provided direction into the local space of the given node.
    // @param node Ref The node used for the transformation.
    // @param position Vec3 The direction to transform.
    // @return Vec3 value The direction transformed into the local node space.
    s["Node"]["to_local_space_direction"] = io_component_node->to_local_space_direction;
    // @function to_world_space
    // @summary Transforms the provided position in the local space of the given node to world space.
    // @param node Ref The node used for the transformation.
    // @param position Vec3 The position to transform.
    // @return Vec3 value The position transformed into world space.
    s["Node"]["to_world_space"] = io_component_node->to_world_space;
    // @function to_world_space_direction
    // @summary Transforms the provided direction in the local space of the given node to world space.
    // @param node Ref The node used for the transformation.
    // @param direction Vec3 The direction to transform.
    // @return Vec3 value The direction transformed into world space.
    s["Node"]["to_world_space_direction"] = io_component_node->to_world_space_direction;

    // @function collect_nodes_depth_first
    // @summary Collects all nodes in the hierarchy in depth first ordering starting at the provided node (including the root).
    // @param node Ref The root node to start collecting at.
    // @return table value Table containig all nodes in the hierarchy.
    s["Node"]["collect_nodes_depth_first"] = [](io_ref_t root_node) {
      uint32_t num_nodes;
      io_component_node->collect_nodes_depth_first(root_node, nullptr, &num_nodes);
      std::vector<io_ref_t> nodes(num_nodes);
      io_component_node->collect_nodes_depth_first(root_node, nodes.data(),
                                              &num_nodes);
      return nodes;
    };
    // @function collect_nodes_breadth_first
    // @summary Collects all nodes in the hierarchy in breadth first ordering starting at the provided node (including the root).
    // @param node Ref The root node to start at.
    // @return table value Table containig all nodes in the hierarchy.
    s["Node"]["collect_nodes_breadth_first"] = [](io_ref_t root_node) {
      uint32_t num_nodes;
      io_component_node->collect_nodes_breadth_first(root_node, nullptr, &num_nodes);
      std::vector<io_ref_t> nodes(num_nodes);
      io_component_node->collect_nodes_breadth_first(root_node, nodes.data(),
                                              &num_nodes);
      return nodes;
    };

    // @function update_transforms
    // @summary Updates the transformations of the given node hierarchy.
    // @param node Ref The root node of the hierarchy.
    s["Node"]["update_transforms"] = io_component_node->update_transforms;

  };

  s["CharacterController"] = s.create_table();
  s["CharacterController"]["load"] = [&s]() {

    SHARED_COMPONENT_INTERFACE_IMPL(s["CharacterController"], io_component_character_controller);

    // @namespace CharacterController
    // @category Character_Controller_Component Functions to interact with character controllers.
    // @copy_category Interface
    // @copy_category Components

    // @function move
    // @summary Moves the character controller.
    // @param component Ref The controller in question.
    // @param move Vec3 The movement to apply to the character controller in world space.
    s["CharacterController"]["move"] = io_component_character_controller->move;
    // @function is_grounded
    // @summary Returns true if the character controller is grounded.
    // @param component Ref The controller in question.
    // @return boolean value True if the controller is grounded.
    s["CharacterController"]["is_grounded"] = io_component_character_controller->is_grounded;
    // @function is_colliding_sides
    // @summary Returns true if the controller is colliding with its sides.
    // @param component Ref The controller in question.
    // @return boolean value True if the controller is colliding with its sides.
    s["CharacterController"]["is_colliding_sides"] = io_component_character_controller->is_colliding_sides;
    // @function is_colliding_up
    // @summary Returns true if the upper part of the controller is colliding.
    // @param component Ref The controller in question.
    // @return boolean value True if the top of the controller is colliding.
    s["CharacterController"]["is_colliding_up"] = io_component_character_controller->is_colliding_up;

  };

  s["CameraController"] = s.create_table();
  s["CameraController"]["load"] = [&s]() {

    SHARED_COMPONENT_INTERFACE_IMPL(s["CameraController"], io_component_camera_controller);

    // @namespace CameraController
    // @category Camera_Controller_Component Functions to interact with camera controllers.
    // @copy_category Interface
    // @copy_category Components

    // @function set_target_node
    // @summary Sets the node the controller should track.
    // @param component Ref The controller in question.
    // @param node Ref The node the controller should track.
    s["CameraController"]["set_target_node"] = io_component_camera_controller->set_target_node;
    // @function set_target_euler_angles
    // @summary Sets the target Euler angles.
    // @param component Ref The controller in question.
    // @param angles Vec3 The target euler angles in radians.
    s["CameraController"]["set_target_euler_angles"] = io_component_camera_controller->set_target_euler_angles;
    // @function get_target_euler_angles
    // @summary Gets the target Euler angles.
    // @param component Ref The controller in question.
    // @return Vec3 value The current target euler angles in radians.
    s["CameraController"]["get_target_euler_angles"] = io_component_camera_controller->get_target_euler_angles;

  };

  s["Particle"] = s.create_table();
  s["Particle"]["load"] = [&s]() {

    SHARED_COMPONENT_INTERFACE_IMPL(s["Particle"], io_component_particle);

    // @namespace Particle
    // @category Particle_Component Functions to interact with particles.
    // @copy_category Interface
    // @copy_category Components

    // @function get_emitter_handle
    // @summary Gets the emitter handle.
    // @param component Ref The particle in question.
    // @return Handle value The emitter handle of this particle.
    s["Particle"]["get_emitter_handle"] = io_component_particle->get_emitter_handle;

  };

  // Call into plugins and let them init. the state
  auto lua_callback_interface =
      (const io_plugin_lua_user_callack_i*)io_api_manager->find_first(
          IO_PLUGIN_LUA_USER_CALLBACK_API_NAME);
  while (lua_callback_interface)
  {
    if (lua_callback_interface->on_init_lua_state)
      lua_callback_interface->on_init_lua_state(&s);

    lua_callback_interface =
        (const io_plugin_lua_user_callack_i*)io_api_manager->get_next(
            lua_callback_interface);
  }
}
} // namespace internal
