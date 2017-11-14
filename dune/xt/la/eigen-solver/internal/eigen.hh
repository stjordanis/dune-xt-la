// This file is part of the dune-xt-la project:
//   https://github.com/dune-community/dune-xt-la
// Copyright 2009-2017 dune-xt-la developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler  (2017)

#ifndef DUNE_XT_LA_EIGEN_SOLVER_INTERNAL_EIGEN_HH
#define DUNE_XT_LA_EIGEN_SOLVER_INTERNAL_EIGEN_HH

#if HAVE_EIGEN
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#endif

#include <dune/common/typetraits.hh>

#include <dune/xt/common/type_traits.hh>
#include <dune/xt/la/exceptions.hh>


namespace Dune {
namespace XT {
namespace LA {
namespace internal {


#if HAVE_EIGEN


template <class S>
void compute_all_eigenvalues_and_vectors_using_eigen(
    const ::Eigen::Matrix<S, ::Eigen::Dynamic, ::Eigen::Dynamic>& matrix,
    std::vector<XT::Common::complex_t<S>>& eigenvalues,
    ::Eigen::Matrix<XT::Common::complex_t<S>, ::Eigen::Dynamic, ::Eigen::Dynamic>& eigenvectors)
{
  ::Eigen::EigenSolver<::Eigen::Matrix<S, ::Eigen::Dynamic, ::Eigen::Dynamic>> eigen_solver(
      matrix, /*computeEigenvectors=*/true);
  if (eigen_solver.info() != ::Eigen::Success)
    DUNE_THROW(Exceptions::eigen_solver_failed, "The eigen backend reported '" << eigen_solver.info() << "'!");
  const auto& evs = eigen_solver.eigenvalues(); // this should be an Eigen vector of std::complex<S>
  if (eigenvalues.size() != evs.size())
    eigenvalues.resize(evs.size());
  for (size_t ii = 0; ii < size_t(evs.size()); ++ii)
    eigenvalues[ii] = evs[ii];
  eigenvectors = eigen_solver.eigenvectors();
} // ... compute_all_eigenvalues_and_vectors_using_eigen(...)


template <class S>
std::vector<XT::Common::complex_t<S>>
compute_all_eigenvalues_using_eigen(const ::Eigen::Matrix<S, ::Eigen::Dynamic, ::Eigen::Dynamic>& matrix)
{
  ::Eigen::EigenSolver<::Eigen::Matrix<S, ::Eigen::Dynamic, ::Eigen::Dynamic>> eigen_solver(
      matrix, /*computeEigenvectors=*/false);
  if (eigen_solver.info() != ::Eigen::Success)
    DUNE_THROW(Exceptions::eigen_solver_failed, "The eigen backend reported '" << eigen_solver.info() << "'!");
  const auto& evs = eigen_solver.eigenvalues(); // this should be an Eigen vector of std::complex<S>
  std::vector<XT::Common::complex_t<S>> ret(evs.size());
  for (size_t ii = 0; ii < size_t(evs.size()); ++ii)
    ret[ii] = evs[ii];
  return ret;
} // ... compute_all_eigenvalues_using_eigen(...)


template <class S>
::Eigen::Matrix<XT::Common::complex_t<S>, ::Eigen::Dynamic, ::Eigen::Dynamic>
compute_all_eigenvectors_using_eigen(const ::Eigen::Matrix<S, ::Eigen::Dynamic, ::Eigen::Dynamic>& matrix)
{
  ::Eigen::EigenSolver<::Eigen::Matrix<S, ::Eigen::Dynamic, ::Eigen::Dynamic>> eigen_solver(
      matrix, /*computeEigenvectors=*/true);
  if (eigen_solver.info() != ::Eigen::Success)
    DUNE_THROW(Exceptions::eigen_solver_failed, "The eigen backend reported '" << eigen_solver.info() << "'!");
  return eigen_solver.eigenvectors();
} // ... compute_all_eigenvectors_using_eigen(...)


#else // HAVE_EIGEN


template <class S>
void compute_all_eigenvalues_and_vectors_using_eigen(
    const ::Eigen::Matrix<S, ::Eigen::Dynamic, ::Eigen::Dynamic>& /*matrix*/,
    std::vector<XT::Common::complex_t<S>>& /*eigenvalues*/,
    ::Eigen::Matrix<XT::Common::complex_t<S>, ::Eigen::Dynamic, ::Eigen::Dynamic>& /*eigenvectors*/)
{
  static_assert(AlwaysFalse<S>::value, "You are missing eigen!");
}


template <class S>
std::vector<XT::Common::complex_t<S>>
compute_all_eigenvalues_using_eigen(const ::Eigen::Matrix<S, ::Eigen::Dynamic, ::Eigen::Dynamic>& /*matrix*/)
{
  static_assert(AlwaysFalse<S>::value, "You are missing eigen!");
}


template <class S>
::Eigen::Matrix<XT::Common::complex_t<S>, ::Eigen::Dynamic, ::Eigen::Dynamic>
compute_all_eigenvectors_using_eigen(const ::Eigen::Matrix<S, ::Eigen::Dynamic, ::Eigen::Dynamic>& /*matrix*/)
{
  static_assert(AlwaysFalse<S>::value, "You are missing eigen!");
}


#endif // HAVE_EIGEN

} // namespace internal
} // namespace LA
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_LA_EIGEN_SOLVER_INTERNAL_EIGEN_HH
