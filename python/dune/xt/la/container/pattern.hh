// This file is part of the dune-xt-la project:
//   https://github.com/dune-community/dune-xt-la
// Copyright 2009-2018 dune-xt-la developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2016 - 2017)
//   Rene Milk       (2018)

#ifndef DUNE_XT_LA_CONTAINER_PATTERN_PBH
#define DUNE_XT_LA_CONTAINER_PATTERN_PBH

#include <boost/numeric/conversion/cast.hpp>

#include <dune/pybindxi/pybind11.h>
#include <dune/pybindxi/operators.h>

#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/type_traits.hh>

#include <dune/xt/la/container/pattern.hh>

namespace Dune {
namespace XT {
namespace LA {


pybind11::class_<SparsityPatternDefault> bind_SparsityPatternDefault(pybind11::module& m)
{
  typedef SparsityPatternDefault C;

  namespace py = pybind11;
  using namespace pybind11::literals;

  py::class_<SparsityPatternDefault> c(m, "SparsityPatternDefault", "SparsityPatternDefault");

  c.def(py::init([](const ssize_t size) {
          try {
            return new C(boost::numeric_cast<size_t>(size));
          } catch (boost::bad_numeric_cast& ee) {
            DUNE_THROW(Common::Exceptions::wrong_input_given,
                       "Given size has to be positive!\n\n The error in boost while converting '"
                           << size << "' to '" << Common::Typename<size_t>::value() << "' was: " << ee.what());
          }
        }),
        "size"_a = 0);

  c.def("insert", &C::insert, "outer_index"_a, "inner_index"_a);
  c.def("sort", [](C& self, const size_t outer_index) { self.sort(outer_index); }, "outer_index"_a);
  c.def("sort", [](C& self) { self.sort(); });

  c.def(py::self == py::self);
  c.def(py::self != py::self);

  return c;
} // ... bind_SparsityPatternDefault(...)


} // namespace LA
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_LA_CONTAINER_PATTERN_PBH
