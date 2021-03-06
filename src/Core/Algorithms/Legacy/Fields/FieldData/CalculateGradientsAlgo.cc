/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2009 Scientific Computing and Imaging Institute,
   University of Utah.

   
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


#include <Core/Algorithms/Legacy/Fields/FieldData/CalculateGradientsAlgo.h>
#include <Core/Datatypes/Legacy/Field/FieldInformation.h>
#include <Core/Datatypes/Legacy/Field/VField.h>
#include <Core/Algorithms/Base/AlgorithmPreconditions.h>

using namespace SCIRun;
using namespace SCIRun::Core::Algorithms::Fields;
using namespace SCIRun::Core::Geometry;
using namespace SCIRun::Core::Utility;
using namespace SCIRun::Core::Algorithms;

bool
CalculateGradientsAlgo::run(FieldHandle input, FieldHandle& output) const
{
  ScopedAlgorithmStatusReporter asr(this, "CalculateGradients");
  if (!input)
  {
    error("No input field");
    return (false);
  }
  
  FieldInformation fi(input);

  if (fi.is_pointcloudmesh())
  {
    error("Cannot calculate gradients for a point cloud");
    return (false);    
  }

  if (fi.is_nodata())
  {
    error("Input field does not have data associated with it");
    return (false);    
  }

  if (!(fi.is_scalar()))
  {
    error("The data needs to be of scalar type to calculate gradients");
    return (false);    
  }

  fi.make_vector();
  fi.make_constantdata();
  output = CreateField(fi,input->mesh());

  if (!output)
  {
    error("Could not allocate output field");
    return (false);      
  }

  VField* ifield = input->vfield();
  VField* ofield = output->vfield();
  VMesh*  imesh  = input->vmesh();
  
  ofield->resize_values();
  
  VMesh::coords_type coords;
  imesh->get_element_center(coords);
  
  VField::size_type num_elems = imesh->num_elems();

  StackVector<double,3> grad;
  for (VMesh::Elem::index_type idx = 0; idx < num_elems; idx++)
  {
    ifield->gradient(grad,coords,idx);    
    Vector v(grad[0],grad[1],grad[2]);
    ofield->set_value(v,idx);  
  }

  return (true);
}

AlgorithmInputName CalculateGradientsAlgo::ScalarField("ScalarField");
AlgorithmOutputName CalculateGradientsAlgo::VectorField("VectorField");

AlgorithmOutput CalculateGradientsAlgo::run_generic(const AlgorithmInput& input) const
{
  auto field = input.get<Field>(ScalarField);

  FieldHandle gradient;
  if (!run(field, gradient))
    THROW_ALGORITHM_PROCESSING_ERROR("False returned on legacy run call.");

  AlgorithmOutput output;
  output[VectorField] = gradient;
  return output;
}