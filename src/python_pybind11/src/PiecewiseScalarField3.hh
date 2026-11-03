/*
 * Copyright (C) 2026 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef GZ_MATH_PYTHON__PIECEWISE_SCALAR_FIELD3_HH_
#define GZ_MATH_PYTHON__PIECEWISE_SCALAR_FIELD3_HH_

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <gz/math/AdditivelySeparableScalarField3.hh>
#include <gz/math/PiecewiseScalarField3.hh>
#include <gz/math/Polynomial3.hh>
#include <gz/math/Region3.hh>
#include <gz/math/Vector3.hh>

namespace py = pybind11;
using namespace pybind11::literals;

namespace gz
{
namespace math
{
namespace python
{
/// Help define a pybind11 wrapper for a polynomial-backed
/// gz::math::PiecewiseScalarField3.
/**
 * \param[in] module a pybind11 module to add the definition to
 * \param[in] typestr name of the type used by Python
 */
template<typename T>
void helpDefineMathPiecewiseScalarField3(
    py::module &m, const std::string &typestr)
{
  using Field = gz::math::AdditivelySeparableScalarField3<
      gz::math::Polynomial3<T>, T>;
  using Class = gz::math::PiecewiseScalarField3<Field, T>;
  using InputPiece = std::pair<Region3<T>, Field>;
  auto toString = [](const Class &field)
  {
    std::stringstream stream;
    stream << field;
    return stream.str();
  };

  py::class_<Class>(m, typestr.c_str(), py::dynamic_attr())
    .def(py::init<>())
    .def(py::init([](const std::vector<InputPiece> &_pieces)
         {
           std::vector<typename Class::Piece> pieces;
           pieces.reserve(_pieces.size());
           std::transform(_pieces.begin(), _pieces.end(),
               std::back_inserter(pieces), [](const InputPiece &_piece)
               {
                 return typename Class::Piece{_piece.first, _piece.second};
               });
           return Class(pieces);
         }))
    .def_static("throughout", &Class::Throughout,
                "Define the scalar field throughout R^3")
    .def("evaluate", &Class::Evaluate,
         "Evaluate the piecewise scalar field at `_point`")
    .def("minimum",
         [](const Class &self, Vector3<T> &_pMin)
         {
           T result = self.Minimum(_pMin);
           return std::make_tuple(result, _pMin);
         },
         "Compute the piecewise scalar field minimum")
    .def("minimum",
         py::overload_cast<>(&Class::Minimum, py::const_),
         "Compute the piecewise scalar field minimum")
    .def("__call__", &Class::operator())
    .def("__copy__", [](const Class &self)
    {
      return Class(self);
    })
    .def("__deepcopy__", [](const Class &self, py::dict)
    {
      return Class(self);
    }, "memo"_a)
    .def("__str__", toString)
    .def("__repr__", toString);
}

/// Define pybind11 wrappers for polynomial-backed
/// gz::math::PiecewiseScalarField3 types.
/**
 * \param[in] module a pybind11 module to add the definitions to
 * \param[in] typestr base name of the types used by Python
 */
void defineMathPiecewiseScalarField3(
    py::module &m, const std::string &typestr);

}  // namespace python
}  // namespace math
}  // namespace gz

#endif  // GZ_MATH_PYTHON__PIECEWISE_SCALAR_FIELD3_HH_
