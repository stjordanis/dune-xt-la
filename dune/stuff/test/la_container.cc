// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht, Rene Milk
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include <dune/stuff/test/test_common.hh>

#include <type_traits>
#include <memory>

#include <dune/common/float_cmp.hh>

#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/la/container/interfaces.hh>
#include <dune/stuff/la/container/dunedynamic.hh>
#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/la/container/istl.hh>
#include <dune/stuff/la/container.hh>


using namespace Dune;

static const size_t dim = 4;

typedef testing::Types<Dune::Stuff::LA::DuneDynamicVector<double>
#if HAVE_EIGEN
                       ,
                       Dune::Stuff::LA::EigenDenseVector<double>
#endif
#if HAVE_DUNE_ISTL
                       ,
                       Dune::Stuff::LA::IstlDenseVector<double>
#endif
                       > VectorTypes;

typedef testing::Types<Dune::Stuff::LA::DuneDynamicMatrix<double>
#if HAVE_EIGEN
                       ,
                       Dune::Stuff::LA::EigenRowMajorSparseMatrix<double>
#endif
#if HAVE_DUNE_ISTL
                       ,
                       Dune::Stuff::LA::IstlRowMajorSparseMatrix<double>
#endif
                       > MatrixTypes;

typedef testing::Types<Dune::Stuff::LA::DuneDynamicVector<double>, Dune::Stuff::LA::DuneDynamicMatrix<double>
#if HAVE_EIGEN
                       ,
                       Dune::Stuff::LA::EigenDenseVector<double>, Dune::Stuff::LA::EigenRowMajorSparseMatrix<double>
#endif
#if HAVE_DUNE_ISTL
                       ,
                       Dune::Stuff::LA::IstlDenseVector<double>, Dune::Stuff::LA::IstlRowMajorSparseMatrix<double>
#endif
                       > ContainerTypes;


template <class ContainerImp>
struct ContainerTest : public ::testing::Test
{
  void check() const
  {
    // static tests
    typedef typename ContainerImp::Traits Traits;
    // * of the traits
    typedef typename Traits::derived_type T_derived_type;
    static_assert(std::is_same<ContainerImp, T_derived_type>::value, "derived_type has to be the correct Type!");
    typedef typename Traits::ScalarType T_ScalarType;
    // * of the container as itself (aka the derived type)
    typedef typename ContainerImp::ScalarType D_ScalarType;
    // * of the container as the interface
    typedef typename Stuff::LA::ContainerInterface<Traits> InterfaceType;
    typedef typename InterfaceType::derived_type I_derived_type;
    typedef typename InterfaceType::ScalarType I_ScalarType;
    // dynamic tests
    // * of the container as itself (aka the derived type)
    ContainerImp DUNE_UNUSED(d_empty);
    ContainerImp d_by_size = Stuff::LA::Container<ContainerImp>::create(dim);
    ContainerImp d_copy_constructor(d_by_size);
    ContainerImp DUNE_UNUSED(d_copy_assignment) = d_by_size;
    ContainerImp d_deep_copy = d_by_size.copy();
    d_by_size.scal(D_ScalarType(1));
    d_by_size.axpy(D_ScalarType(1), d_deep_copy);
    if (!d_by_size.has_equal_shape(d_deep_copy))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    // * of the container as the interface
    InterfaceType& i_by_size = static_cast<InterfaceType&>(d_by_size);
    ContainerImp i_deep_copy = i_by_size.copy();
    i_by_size.scal(I_ScalarType(1));
    i_by_size.axpy(I_ScalarType(1), i_deep_copy);
  }
}; // struct ContainerTest


TYPED_TEST_CASE(ContainerTest, ContainerTypes);
TYPED_TEST(ContainerTest, LA_CONTAINER)
{
  this->check();
}


template <class VectorImp>
struct VectorTest : public ::testing::Test
{
  void check() const
  {
    // static tests
    typedef typename VectorImp::Traits Traits;
    // * of the traits
    typedef typename Traits::derived_type T_derived_type;
    static_assert(std::is_same<VectorImp, T_derived_type>::value, "derived_type has to be the correct Type!");
    typedef typename Traits::ScalarType T_ScalarType;
    // * of the vector as itself (aka the derived type)
    typedef typename VectorImp::ScalarType D_ScalarType;
    // * of the vector as the interface
    typedef typename Stuff::LA::VectorInterface<Traits> InterfaceType;
    typedef typename InterfaceType::derived_type I_derived_type;
    typedef typename InterfaceType::ScalarType I_ScalarType;
    // dynamic tests
    // * of the vector as itself (aka the derived type)
    VectorImp d_by_size(dim);
    VectorImp d_by_size_and_value(dim, D_ScalarType(0));
    size_t d_size = d_by_size.size();
    if (d_size != dim)
      DUNE_THROW_COLORFULLY(Dune::Exception, d_size << " vs. " << dim);
    for (size_t ii = 0; ii < d_size; ++ii) {
      d_by_size_and_value.set_entry(ii, D_ScalarType(0.5));
      d_by_size_and_value.add_to_entry(ii, D_ScalarType(0.5));
      if (FloatCmp::ne(d_by_size_and_value.get_entry(ii), D_ScalarType(1)))
        DUNE_THROW_COLORFULLY(Dune::Exception, d_by_size_and_value.get_entry(ii));
    }
    size_t DUNE_UNUSED(d_dim) = d_by_size.dim();
    bool d_almost_equal = d_by_size.almost_equal(d_by_size);
    if (!d_almost_equal)
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    d_by_size_and_value.scal(D_ScalarType(0));
    if (!d_by_size_and_value.almost_equal(d_by_size))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    D_ScalarType d_dot = d_by_size.dot(d_by_size_and_value);
    if (!Dune::FloatCmp::eq(d_dot, D_ScalarType(0)))
      DUNE_THROW_COLORFULLY(Dune::Exception, d_dot);
    D_ScalarType d_l1_norm = d_by_size.l1_norm();
    if (!Dune::FloatCmp::eq(d_l1_norm, D_ScalarType(0)))
      DUNE_THROW_COLORFULLY(Dune::Exception, d_l1_norm);
    D_ScalarType d_l2_norm = d_by_size.l2_norm();
    if (!Dune::FloatCmp::eq(d_l2_norm, D_ScalarType(0)))
      DUNE_THROW_COLORFULLY(Dune::Exception, d_l2_norm);
    D_ScalarType d_sup_norm = d_by_size.sup_norm();
    if (!Dune::FloatCmp::eq(d_sup_norm, D_ScalarType(0)))
      DUNE_THROW_COLORFULLY(Dune::Exception, d_sup_norm);
    VectorImp d_ones(dim, D_ScalarType(1));
    std::pair<size_t, D_ScalarType> d_amax = d_ones.amax();
    if (d_amax.first != 0 || !Dune::FloatCmp::eq(d_amax.second, D_ScalarType(1)))
      DUNE_THROW_COLORFULLY(Dune::Exception, d_amax.first << ",  " << d_amax.second);
    d_ones.add(d_by_size, d_by_size_and_value);
    if (!d_by_size_and_value.almost_equal(d_ones))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    VectorImp d_added = d_ones.add(d_by_size);
    if (!d_added.almost_equal(d_by_size_and_value))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    d_added.iadd(d_by_size);
    if (!d_added.almost_equal(d_ones))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    d_ones.sub(d_by_size, d_by_size_and_value);
    if (!d_by_size_and_value.almost_equal(d_ones))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    VectorImp d_subtracted = d_ones.sub(d_by_size);
    if (!d_subtracted.almost_equal(d_by_size_and_value))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    d_subtracted.isub(d_by_size);
    if (!d_subtracted.almost_equal(d_ones))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    // * of the vector as the interface
    VectorImp d_by_size_2(dim);
    VectorImp d_by_size_and_value_2(dim, D_ScalarType(1));
    InterfaceType& i_by_size           = static_cast<InterfaceType&>(d_by_size_2);
    InterfaceType& i_by_size_and_value = static_cast<InterfaceType&>(d_by_size_and_value_2);
    DUNE_STUFF_SSIZE_T DUNE_UNUSED(i_dim) = i_by_size.dim();
    bool i_almost_equal = i_by_size.almost_equal(d_by_size_2);
    if (!i_almost_equal)
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    i_by_size_and_value.scal(I_ScalarType(0));
    if (!i_by_size_and_value.almost_equal(d_by_size_2))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    I_ScalarType i_dot = i_by_size.dot(d_by_size_and_value_2);
    if (!Dune::FloatCmp::eq(i_dot, I_ScalarType(0)))
      DUNE_THROW_COLORFULLY(Dune::Exception, i_dot);
    I_ScalarType i_l1_norm = i_by_size.l1_norm();
    if (!Dune::FloatCmp::eq(i_l1_norm, I_ScalarType(0)))
      DUNE_THROW_COLORFULLY(Dune::Exception, i_l1_norm);
    I_ScalarType i_l2_norm = i_by_size.l2_norm();
    if (!Dune::FloatCmp::eq(i_l2_norm, I_ScalarType(0)))
      DUNE_THROW_COLORFULLY(Dune::Exception, i_l2_norm);
    I_ScalarType i_sup_norm = i_by_size.sup_norm();
    if (!Dune::FloatCmp::eq(i_sup_norm, I_ScalarType(0)))
      DUNE_THROW_COLORFULLY(Dune::Exception, i_sup_norm);
    VectorImp i_ones(dim, I_ScalarType(1));
    std::pair<size_t, I_ScalarType> i_amax = i_ones.amax();
    if (i_amax.first != 0 || !Dune::FloatCmp::eq(i_amax.second, I_ScalarType(1)))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    i_ones.add(d_by_size_2, d_by_size_and_value_2);
    if (!i_by_size_and_value.almost_equal(i_ones))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    VectorImp i_added = i_ones.add(d_by_size_2);
    if (!i_added.almost_equal(d_by_size_and_value_2))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    i_added.iadd(d_by_size_2);
    if (!i_added.almost_equal(i_ones))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    i_ones.sub(d_by_size_2, d_by_size_and_value_2);
    if (!i_by_size_and_value.almost_equal(i_ones))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    VectorImp i_subtracted = i_ones.sub(d_by_size_2);
    if (!i_subtracted.almost_equal(d_by_size_and_value_2))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
    i_subtracted.isub(d_by_size_2);
    if (!i_subtracted.almost_equal(i_ones))
      DUNE_THROW_COLORFULLY(Dune::Exception, "");
  }
}; // struct VectorTest


TYPED_TEST_CASE(VectorTest, VectorTypes);
TYPED_TEST(VectorTest, LA_CONTAINER)
{
  this->check();
}


int main(int argc, char** argv)
{
  try {
    test_init(argc, argv);
    return RUN_ALL_TESTS();
  } catch (Dune::Exception& e) {
    std::cerr << Dune::Stuff::Common::colorStringRed("Dune reported error: ") << e << std::endl;
  } catch (...) {
    std::cerr << Dune::Stuff::Common::colorStringRed("Unknown exception thrown!") << std::endl;
  }
}