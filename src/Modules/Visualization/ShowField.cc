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

#include <Modules/Visualization/ShowField.h>
#include <Core/Datatypes/Geometry.h>
#include <Core/Datatypes/Legacy/Field/VMesh.h>
#include <Core/Datatypes/Legacy/Field/Field.h>
#include <Core/Datatypes/Legacy/Field/VField.h>
#include <Core/Datatypes/Mesh/MeshFacade.h>
#include <Core/Datatypes/Color.h>

#include <boost/foreach.hpp>

using namespace SCIRun::Modules::Visualization;
using namespace SCIRun::Core::Datatypes;
using namespace SCIRun::Dataflow::Networks;
using namespace SCIRun::Core::Algorithms;
using namespace SCIRun;


ShowFieldModule::ShowFieldModule() : 
    Module(ModuleLookupInfo("ShowField", "Visualization", "SCIRun"))
{
  setDefaults();
}

void ShowFieldModule::setDefaults()
{
  auto state = get_state();
  state->setValue(ShowNodes, false);
  state->setValue(ShowEdges, true);
  state->setValue(ShowFaces, true);
  state->setValue(NodeTransparency, false);
  state->setValue(EdgeTransparency, false);
  state->setValue(FaceTransparency, false);
  //state->setValue(DefaultMeshColor, std::string("white"));
}

void ShowFieldModule::execute()
{
  boost::shared_ptr<SCIRun::Field> field = getRequiredInput(Field);
  GeometryHandle geom = buildGeometryObject(field, get_state(), get_id());
  sendOutput(SceneGraph, geom);
}


GeometryHandle ShowFieldModule::buildGeometryObject(
    boost::shared_ptr<SCIRun::Field> field,
    ModuleStateHandle state, 
    const std::string& id)
{
  // Function for reporting progress.
  SCIRun::Core::Algorithms::AlgorithmStatusReporter::UpdaterFunc progressFunc =
      getUpdaterFunc();

  // VMesh facade. A simpler interface to the vmesh type.
  SCIRun::Core::Datatypes::MeshTraits<VMesh>::MeshFacadeHandle facade =
      field->mesh()->getFacade();
  ENSURE_NOT_NULL(facade, "Mesh facade");

  // VField required only for extracting field data from the field datatype.
  VField* vfield = field->vfield();

  // Grab the vmesh object so that we can synchronize and extract normals.
  VMesh* vmesh = field->vmesh();

  // Ensure any changes made to the mesh are reflected in the normals by
  // by synchronizing the mesh.
  if (vmesh->has_normals())
    vmesh->synchronize(Mesh::NORMALS_E);
  
  /// \todo Determine a better way of handling all of the various object state.
  bool showNodes = state->getValue(ShowFieldModule::ShowNodes).getBool();
  bool showEdges = state->getValue(ShowFieldModule::ShowEdges).getBool();
  bool showFaces = state->getValue(ShowFieldModule::ShowFaces).getBool();
  bool nodeTransparency = state->getValue(ShowFieldModule::NodeTransparency).getBool();

  // Resultant geometry type (representing a spire object and a number of passes).
  GeometryHandle geom(new GeometryObject(field));
  geom->objectName = id;

  /// \todo Split the mesh into chunks of about ~32,000 vertices. May be able to
  ///       eek out better coherency and use a 16 bit index buffer instead of
  ///       a 32 bit index buffer.

  // Crude method of counting the attributes we are placing in the VBO.
  int numFloats = 3 + 1;  // Position + field data.
  if (vmesh->has_normals())
    numFloats += 3;       // Position + field data + normals;

  // Allocate memory for vertex buffer.
  // Edges and faces should use the same vbo!
  std::shared_ptr<std::vector<uint8_t>> rawVBO(new std::vector<uint8_t>());
  size_t vboSize = sizeof(float) * numFloats * facade->numNodes();
  rawVBO->resize(vboSize); // linear complexity.
  // The C++ standard guarantees that vectors are contiguous in memory, so we
  // grab a pointer to the first element and use that as the starting point
  // for building our VBO.
  float* vbo = reinterpret_cast<float*>(&(*rawVBO)[0]);

  // Add shared VBO to the geometry object.
  /// \note This 'primaryVBO' is dependent on the types present in the data.
  ///       If normals are specified, then this will NOT be the primary VBO.
  ///       Another VBO will be constructed containing normal information.
  ///       All of this is NOT necessary if we are on OpenGL 3.2+ where we
  ///       can compute all normals in the geometry shader (smooth and face).
  std::string primVBOName = id + "primaryVBO";
  std::vector<std::string> attribs;   ///< \todo Switch to initializer lists when msvc supports it.
  attribs.push_back("aPos");          ///< \todo Find a good place to pull these names from.
  if (vmesh->has_normals())
    attribs.push_back("aNormal");
  attribs.push_back("aFieldData");
  geom->mVBOs.emplace_back(GeometryObject::SpireVBO(primVBOName, attribs, rawVBO));

  if (progressFunc) progressFunc(0.1);

  // Build vertex buffer.
  size_t i = 0;
  BOOST_FOREACH(const NodeInfo<VMesh>& node, facade->nodes())
  {
    // Add position (aPos)
    size_t nodeOffset = 0;
    vbo[i+nodeOffset+0] = node.point().x();
    vbo[i+nodeOffset+1] = node.point().y();
    vbo[i+nodeOffset+2] = node.point().z();
    nodeOffset += 3;

    // Add optional normals (aNormal)
    if (vmesh->has_normals())
    {
      Core::Geometry::Vector normal;
      vmesh->get_normal(normal, node.index());

      vbo[i+nodeOffset+0] = normal.x();
      vbo[i+nodeOffset+1] = normal.y();
      vbo[i+nodeOffset+2] = normal.z();
      nodeOffset += 3;
    }

    // Add field data (aFieldData)
    if (node.index() < vfield->num_values())
    {
      double val = 0.0;
      vfield->get_value(val, node.index());
      vbo[i+nodeOffset] = static_cast<float>(val);
    }
    else
    {
      vbo[i+nodeOffset] = 0.0f;
    }
    nodeOffset += 1;

    i += nodeOffset;
  }

  if (progressFunc) progressFunc(0.25);

  // Build the edges
  if (showEdges)
  {
    std::string iboName = id + "edgesIBO";
    buildEdgesIBO(facade, geom, iboName);

    // Build pass for the edges.
    /// \todo Find an appropriate place to put program names like UniformColor.
    GeometryObject::SpireSubPass pass = 
        GeometryObject::SpireSubPass("edgesPass", primVBOName, iboName,
                                     "UniformColor", Spire::Interface::LINES);

    Spire::GPUState gpuState;
    gpuState.mLineWidth = 2.5f;
    pass.addGPUState(gpuState);

    bool edgeTransparency = state->getValue(ShowFieldModule::EdgeTransparency).getBool();
    // Add appropriate uniforms to the pass (in this case, uColor).
    if (edgeTransparency)
      pass.addUniform("uColor", Spire::V4(0.6f, 0.6f, 0.6f, 0.5f));
    else
      pass.addUniform("uColor", Spire::V4(0.6f, 0.6f, 0.6f, 1.0f));

    geom->mPasses.emplace_back(pass);
  }

  if (progressFunc) progressFunc(0.5);

  // Build the faces
  if (showFaces)
  {
    //double dataMin = 0.0;
    //double dataMax = 0.0;
    //vfield->min(dataMin);
    //vfield->max(dataMax);
    const std::string iboName = id + "facesIBO";
    buildFacesIBO(facade, geom, iboName);

    // Construct construct a uniform color pass.
    /// \todo Allow the user to select from a few different lighting routines
    ///       and bind them here.
    if (vmesh->has_normals())
    {
      GeometryObject::SpireSubPass pass = 
          GeometryObject::SpireSubPass("facesPass", primVBOName, iboName, 
                                       "DirPhong", Spire::Interface::TRIANGLES);

      // Add common uniforms.
      pass.addUniform("uAmbientColor", Spire::V4(0.01f, 0.01f, 0.01f, 1.0f));
      pass.addUniform("uDiffuseColor", Spire::V4(0.8f, 0.8f, 0.8f, 1.0f));
      pass.addUniform("uSpecularColor", Spire::V4(1.0f, 1.0f, 1.0f, 1.0f));
      pass.addUniform("uSpecularPower", 32.0f);
      geom->mPasses.emplace_back(pass);
    }
    else
    {
      // No normals present in the model, construct a uniform pass
      GeometryObject::SpireSubPass pass = 
          GeometryObject::SpireSubPass("facesPass", primVBOName, iboName,
                                       "UniformColor", Spire::Interface::TRIANGLES);

      // Apply misc user settings.
      bool faceTransparency = state->getValue(ShowFieldModule::FaceTransparency).getBool();
      float transparency    = 1.0f;
      if (faceTransparency) transparency = 0.2f;
      pass.addUniform("uColor", Spire::V4(0.6f, 0.6f, 0.6f, transparency));
      geom->mPasses.emplace_back(pass);
    }
  }

  if (progressFunc) progressFunc(0.75);

  // Build the nodes
  if (showNodes)
  {
    const std::string iboName = id + "nodesIBO";
    buildNodesIBO(facade, geom, iboName);

    // Build pass for the nodes.
    /// \todo Find an appropriate place to put program names like UniformColor.
    GeometryObject::SpireSubPass pass = 
        GeometryObject::SpireSubPass("nodesPass", primVBOName, iboName,
                                     "UniformColor", Spire::Interface::POINTS);

    // Add appropriate uniforms to the pass (in this case, uColor).
    bool nodeTransparency = state->getValue(ShowFieldModule::NodeTransparency).getBool();
    if (nodeTransparency)
      pass.addUniform("uColor", Spire::V4(0.6f, 0.6f, 0.6f, 0.5f));
    else
      pass.addUniform("uColor", Spire::V4(0.6f, 0.6f, 0.6f, 1.0f));

    geom->mPasses.emplace_back(pass);
  }

  if (progressFunc) progressFunc(1);

  return geom;
}


void ShowFieldModule::buildFacesIBO(
    SCIRun::Core::Datatypes::MeshTraits<VMesh>::MeshFacadeHandle facade, 
    GeometryHandle geom, const std::string& desiredIBOName)
{
  uint32_t* iboFaces = nullptr;

  // Determine the size of the face structure (taking into account the varying
  // types of faces -- only quads and triangles are currently supported).
  size_t iboFacesSize = 0;
  BOOST_FOREACH(const FaceInfo<VMesh>& face, facade->faces())
  {
    VMesh::Node::array_type nodes = face.nodeIndices();
    if (nodes.size() == 4)
    {
      iboFacesSize += sizeof(uint32_t) * 6;
    }
    else if (nodes.size() == 3)
    {
      iboFacesSize += sizeof(uint32_t) * 3;
    }
    else
    {
      BOOST_THROW_EXCEPTION(SCIRun::Core::DimensionMismatch() 
        << SCIRun::Core::ErrorMessage("Only Quads and Triangles are supported."));
    }
  }
  std::shared_ptr<std::vector<uint8_t>> rawIBO(new std::vector<uint8_t>());
  rawIBO->resize(iboFacesSize);   // Linear in complexity... If we need more performance,
  // use malloc to generate buffer and then vector::assign.
  iboFaces = reinterpret_cast<uint32_t*>(&(*rawIBO)[0]);
  size_t i = 0;
  BOOST_FOREACH(const FaceInfo<VMesh>& face, facade->faces())
  {
    VMesh::Node::array_type nodes = face.nodeIndices();
    if (nodes.size() == 4)
    {
      // Winding order looks good from tests.
      iboFaces[i  ] = static_cast<uint32_t>(nodes[0]); iboFaces[i+1] = static_cast<uint32_t>(nodes[1]); iboFaces[i+2] = static_cast<uint32_t>(nodes[2]);
      iboFaces[i+3] = static_cast<uint32_t>(nodes[0]); iboFaces[i+4] = static_cast<uint32_t>(nodes[2]); iboFaces[i+5] = static_cast<uint32_t>(nodes[3]);
      i += 6;
    }
    else if (nodes.size() == 3)
    {
      iboFaces[i  ] = static_cast<uint32_t>(nodes[0]); iboFaces[i+1] = static_cast<uint32_t>(nodes[1]); iboFaces[i+2] = static_cast<uint32_t>(nodes[2]);
      i += 3;
    }
    // All other cases have been checked in the loop above which determines
    // the size of the face IBO.
  }

  // Add IBO for the faces.
  geom->mIBOs.emplace_back(GeometryObject::SpireIBO(desiredIBOName, sizeof(uint32_t), rawIBO));
}


void ShowFieldModule::buildEdgesIBO(
    SCIRun::Core::Datatypes::MeshTraits<VMesh>::MeshFacadeHandle facade,
    GeometryHandle geom, const std::string& desiredIBOName)
{

  size_t iboEdgesSize = sizeof(uint32_t) * facade->numEdges() * 2;
  uint32_t* iboEdges = nullptr;

  std::shared_ptr<std::vector<uint8_t>> rawIBO(new std::vector<uint8_t>());
  rawIBO->resize(iboEdgesSize);   // Linear in complexity... If we need more performance,
  // use malloc to generate buffer and then vector::assign.
  iboEdges = reinterpret_cast<uint32_t*>(&(*rawIBO)[0]);
  size_t i = 0;
  BOOST_FOREACH(const EdgeInfo<VMesh>& edge, facade->edges())
  {
    // There should *only* be two indicies (linestrip would be better...)
    VMesh::Node::array_type nodes = edge.nodeIndices();
    ENSURE_DIMENSIONS_MATCH(nodes.size(), 2, "Edges require exactly 2 indices.");
    iboEdges[i] = static_cast<uint32_t>(nodes[0]); iboEdges[i+1] = static_cast<uint32_t>(nodes[1]);
    i += 2;
  }

  // Add IBO for the edges.
  geom->mIBOs.emplace_back(GeometryObject::SpireIBO(desiredIBOName, sizeof(uint32_t), rawIBO));

}

void ShowFieldModule::buildNodesIBO(
    SCIRun::Core::Datatypes::MeshTraits<VMesh>::MeshFacadeHandle facade,
    GeometryHandle geom, const std::string& desiredIBOName)
{
  size_t iboNodesSize = sizeof(uint32_t) * facade->numNodes();
  uint32_t* iboNodes = nullptr;

  std::shared_ptr<std::vector<uint8_t>> rawIBO(new std::vector<uint8_t>());
  rawIBO->resize(iboNodesSize);   // Linear in complexity... If we need more performance,
  // use malloc to generate buffer and then vector::assign.
  iboNodes = reinterpret_cast<uint32_t*>(&(*rawIBO)[0]);
  size_t i = 0;
  BOOST_FOREACH(const NodeInfo<VMesh>& node, facade->nodes())
  {
    // There should *only* be two indicies (linestrip would be better...)
    //node.index()
    //VirtualMesh::Node::array_type nodes = node.nodeIndices();
    //ENSURE_DIMENSIONS_MATCH(nodes.size(), 2, "Edges require exactly 2 indices.");
    iboNodes[i] = static_cast<uint32_t>(node.index());
    i++;
  }

  // Add IBO for the nodes.
  geom->mIBOs.emplace_back(GeometryObject::SpireIBO(desiredIBOName, sizeof(uint32_t), rawIBO));
}

AlgorithmParameterName ShowFieldModule::ShowNodes("ShowNodes");
AlgorithmParameterName ShowFieldModule::ShowEdges("ShowEdges");
AlgorithmParameterName ShowFieldModule::ShowFaces("ShowFaces");
AlgorithmParameterName ShowFieldModule::NodeTransparency("NodeTransparency");
AlgorithmParameterName ShowFieldModule::EdgeTransparency("EdgeTransparency");
AlgorithmParameterName ShowFieldModule::FaceTransparency("FaceTransparency");
AlgorithmParameterName ShowFieldModule::DefaultMeshColor("DefaultMeshColor");


