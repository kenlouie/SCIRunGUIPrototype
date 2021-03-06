  /*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2012 Scientific Computing and Imaging Institute,
   University of Utah.

   License for the specific language governing rights and limitations under
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/
 
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <fstream>
#include <Core/Algorithms/Math/ParallelAlgebra/ParallelLinearAlgebra.h>
#include <Core/Datatypes/DenseMatrix.h>
#include <Core/Datatypes/DenseColumnMatrix.h>
#include <Core/Datatypes/SparseRowMatrix.h>
#include <Core/Datatypes/MatrixComparison.h>
#include <Core/Datatypes/MatrixTypeConversions.h>
#include <Core/Datatypes/MatrixIO.h>
#include <Testing/Utils/MatrixTestUtilities.h>
#include <boost/thread/thread.hpp>

using namespace SCIRun::Core::Datatypes;
using namespace SCIRun::Core::Algorithms::Math;
using namespace SCIRun::Core::Algorithms;
using namespace SCIRun::TestUtils;
using namespace SCIRun;
using namespace ::testing;

namespace
{
  const int size = 1000;
  SparseRowMatrixHandle matrix1() 
  {
    SparseRowMatrixHandle m(boost::make_shared<SparseRowMatrix>(size,size));
    m->insert(0,0) = 1;
    m->insert(1,2) = -1;
    m->insert(size-1,size-1) = 2;
    return m;
  }

  SparseRowMatrix matrix2() 
  {
    SparseRowMatrix m(size,size);
    m.insert(0,1) = -2;
    m.insert(1,1) = 0.5;
    m.insert(size-1,size-1) = 4;
    return m;
  }

  SparseRowMatrix matrix3() 
  {
    SparseRowMatrix m(size,size);
    m.insert(0,0) = -1;
    m.insert(1,2) = 1;
    m.insert(size-1,size-1) = -2;
    return m;
  }

  DenseColumnMatrixHandle vector1()
  {
    DenseColumnMatrixHandle v(boost::make_shared<DenseColumnMatrix>(size));
    v->setZero();
    *v << 1, 2, 4;
    (*v)[size-1] = -1;
    return v;
  }

  DenseColumnMatrixHandle vector2()
  {
    DenseColumnMatrixHandle v(boost::make_shared<DenseColumnMatrix>(size));
    v->setZero();
    *v << -1, -2, -4;
    (*v)[size-1] = 1;
    return v;
  }

  DenseColumnMatrixHandle vector3()
  {
    DenseColumnMatrixHandle v(boost::make_shared<DenseColumnMatrix>(size));
    v->setZero();
    *v << 0, 1, 0;
    (*v)[size-1] = -7;
    return v;
  }

  unsigned int numProcs()
  {
    return boost::thread::hardware_concurrency();
  }

  SolverInputs getDummySystem()
  {
    SolverInputs system;
    system.A = matrix1();
    system.b = vector1();
    system.x = vector2();
    system.x0 = vector3();
    return system;
  }
}

TEST(ParallelLinearAlgebraTests, CanCreateEmptyParallelVector)
{
  ParallelLinearAlgebraSharedData data(getDummySystem(), 1);
  ParallelLinearAlgebra pla(data, 0);

  ParallelLinearAlgebra::ParallelVector v;
  EXPECT_TRUE(pla.new_vector(v));

  EXPECT_EQ(size, v.size_);
}

TEST(ParallelLinearAlgebraTests, CanCreateParallelVectorFromVectorAsShallowReference)
{
  ParallelLinearAlgebraSharedData data(getDummySystem(), 1);
  ParallelLinearAlgebra pla(data, 0);

  ParallelLinearAlgebra::ParallelVector v;
  auto v1 = vector1();
  EXPECT_TRUE(pla.add_vector(v1, v));

  EXPECT_EQ(v1->nrows(), v.size_);
  for (size_t i = 0; i < size; ++i)
    EXPECT_EQ((*v1)[i], v.data_[i]);

  EXPECT_EQ(0, (*v1)[100]);
  v.data_[100]++;
  EXPECT_EQ(1, (*v1)[100]);
}

TEST(ParallelLinearAlgebraTests, CanCopyParallelSparseMatrixAsShallowReference)
{
  ParallelLinearAlgebraSharedData data(getDummySystem(), 1);
  ParallelLinearAlgebra pla(data, 0);

  ParallelLinearAlgebra::ParallelMatrix m;
  auto m1 = matrix1();
  EXPECT_TRUE(pla.add_matrix(m1, m));

  EXPECT_EQ(m1->nrows(), m.m_);
  EXPECT_EQ(m1->ncols(), m.n_);
  EXPECT_EQ(m1->nonZeros(), m.nnz_);
  EXPECT_EQ(m1->coeff(1,2), m.data_[1]);

  EXPECT_EQ(1, m1->coeff(0,0));
  m.data_[0]++;
  EXPECT_EQ(2, m1->coeff(0,0));
}

TEST(ParallelLinearAlgebraTests, CanCopyContentsOfVector)
{
  ParallelLinearAlgebraSharedData data(getDummySystem(), 1);
  ParallelLinearAlgebra pla(data, 0);

  ParallelLinearAlgebra::ParallelVector v1;
  pla.new_vector(v1);
  
  ParallelLinearAlgebra::ParallelVector v2;
  auto vec2 = vector2();
  pla.add_vector(vec2, v2);
  
  pla.copy(v2, v1);

  EXPECT_EQ(v1.size_, v2.size_);
  for (size_t i = 0; i < size; ++i)
  {
    EXPECT_EQ(v2.data_[i], v1.data_[i]);
  }
  v1.data_[7]++;
  EXPECT_NE(v1.data_[7], v2.data_[7]);
}

struct Copy
{
  Copy(ParallelLinearAlgebraSharedData& data, ParallelLinearAlgebra::ParallelVector& v1, ParallelLinearAlgebra::ParallelVector& v2, int proc, DenseColumnMatrixHandle vec2copy) : 
    data_(data), v1_(v1), v2_(v2), proc_(proc), vec2copy_(vec2copy) {}

  ParallelLinearAlgebraSharedData& data_;
  int proc_;
  ParallelLinearAlgebra::ParallelVector& v1_;
  ParallelLinearAlgebra::ParallelVector& v2_;
  DenseColumnMatrixHandle vec2copy_;

  void operator()()
  {
    ParallelLinearAlgebra pla(data_, proc_);
    pla.new_vector(v1_);
    pla.add_vector(vec2copy_, v2_);
    pla.copy(v2_, v1_);
  }
};

TEST(ParallelLinearAlgebraTests, CanCopyContentsOfVectorMulti)
{
  ParallelLinearAlgebraSharedData data(getDummySystem(), 2);
  
  ParallelLinearAlgebra::ParallelVector v1, v2;

  auto vec2copy = vector2();
  Copy c0(data, v1, v2, 0, vec2copy);
  Copy c1(data, v1, v2, 1, vec2copy);
  
  boost::thread t1 = boost::thread(boost::ref(c0));
  boost::thread t2 = boost::thread(boost::ref(c1));
  t1.join();
  t2.join();

  EXPECT_EQ(v1.size_, v2.size_);
  for (size_t i = 0; i < size; ++i)
  {
    EXPECT_EQ(v2.data_[i], v1.data_[i]);
  }
  v1.data_[7]++;
  EXPECT_NE(v1.data_[7], v2.data_[7]);
}



TEST(ParallelArithmeticTests, CanComputeMaxOfVector)
{
  ParallelLinearAlgebraSharedData data(getDummySystem(), 1);
  ParallelLinearAlgebra pla(data, 0);

  ParallelLinearAlgebra::ParallelVector v1;
  auto vec1 = vector1();
  pla.add_vector(vec1, v1);
  double max1 = pla.max(v1);
  ParallelLinearAlgebra::ParallelVector v2;
  auto vec2 = vector2();
  pla.add_vector(vec2, v2);
  double max2 = pla.max(v2);
  
  EXPECT_EQ(4, max1);
  EXPECT_EQ(1, max2);
}

//TODO FIX_UNIT_TESTS--Intern.
#if 0
//TODO: by intern
TEST(ParallelArithmeticTests, CanTakeAbsoluteValueOfDiagonal)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanTakeAbsoluteValueOfDiagonalMulti)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanComputeMaxOfVectorMulti)
{
  ParallelLinearAlgebraSharedData data(getDummySystem(), 4);
  ParallelLinearAlgebra pla(data, 1);

  //pla.max()


  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanInvertElementsOfVectorWithAbsoluteValueThreshold)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanInvertElementsOfVectorWithAbsoluteValueThresholdMulti)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelLinearAlgebraTests, CanFillVectorWithOnes)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanMultiplyMatrixByVector)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanMultiplyMatrixByVectorMulti)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanSubtractVectors)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanCompute2Norm)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanMultiplyVectorsComponentWise)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanComputeDotProduct)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanSubtractVectorsMulti)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanCompute2NormMulti)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanMultiplyVectorsComponentWiseMulti)
{
  EXPECT_TRUE(false);
}

//TODO: by intern
TEST(ParallelArithmeticTests, CanComputeDotProductMulti)
{
  EXPECT_TRUE(false);
}
#endif
