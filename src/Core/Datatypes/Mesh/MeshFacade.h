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

#ifndef CORE_DATATYPES_MESH_FACADE_H
#define CORE_DATATYPES_MESH_FACADE_H 

#include <Core/Datatypes/Mesh/MeshFacadeIterators.h>
#include <Core/Datatypes/Mesh/Share.h>

namespace SCIRun {
namespace Core {
namespace Datatypes {

  class SCISHARE MeshFacade
  {
  public:
    virtual ~MeshFacade() {}

    // prototype this for use with BOOST_FOREACH, so use pairs of iterators. When upgrading to C++11, will need to support a range concept.
    typedef std::pair<SmartNodeIterator, SmartNodeIterator> Nodes;
    typedef std::pair<SmartEdgeIterator, SmartEdgeIterator> Edges;
    typedef std::pair<SmartFaceIterator, SmartFaceIterator> Faces;

    virtual Nodes nodes() const = 0;
    virtual Edges edges() const = 0;
    virtual Faces faces() const = 0;

    virtual size_t numNodes() const = 0;
    virtual size_t numEdges() const = 0;
    virtual size_t numFaces() const = 0;
    virtual size_t numElements() const = 0;
  };

}}}

#endif
