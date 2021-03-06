#include "Kit/Submesh.hpp"

std::map<std::string, kit::Submesh::Ptr> kit::Submesh::m_cache = std::map<std::string, kit::Submesh::Ptr>();

kit::Submesh::Submesh(std::string filename)
{
  this->allocateBuffers();
  this->m_indexCount = 0;
  
  std::string fullpath = std::string("./data/geometry/") + filename;
  this->loadGeometry(fullpath);
}

kit::Submesh::~Submesh()
{
  this->releaseBuffers();
}

void kit::Submesh::renderGeometry()
{
  kit::GL::bindVertexArray(this->m_glVertexArray);
  KIT_GL(glDrawElements( GL_TRIANGLES, this->m_indexCount, GL_UNSIGNED_INT, (void*)0));
}

kit::Submesh::Ptr kit::Submesh::load(std::string name)
{
  auto finder = kit::Submesh::m_cache.find(name);
  if(finder != kit::Submesh::m_cache.end())
  {
    return finder->second;
  }
  
  kit::Submesh::m_cache[name] = std::make_shared<kit::Submesh>(name);
  
  return kit::Submesh::m_cache[name];
}

void kit::Submesh::flushCache()
{
  kit::Submesh::m_cache.clear();
}

void kit::Submesh::allocateBuffers()
{
  
  KIT_GL(glGenVertexArrays(1, &this->m_glVertexArray));
  KIT_GL(glGenBuffers(1, &this->m_glVertexIndices));
  KIT_GL(glGenBuffers(1, &this->m_glVertexBuffer));
}

void kit::Submesh::releaseBuffers()
{
  
  KIT_GL(glDeleteBuffers(1, &this->m_glVertexIndices));
  KIT_GL(glDeleteBuffers(1, &this->m_glVertexBuffer));
  KIT_GL(glDeleteVertexArrays(1, &this->m_glVertexArray));
}

void kit::Submesh::loadGeometry(std::string filename)
{
  
  kit::Geometry data;
  if(!data.load(filename))
  {
    KIT_THROW("Failed to load submesh data from file");
  }
  
  this->m_indexCount = (uint32_t)data.m_indices.size();
  
  kit::GL::bindVertexArray(this->m_glVertexArray);
  
  // Upload indices
  kit::GL::bindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_glVertexIndices);
  KIT_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.m_indices.size() * sizeof(uint32_t), &data.m_indices[0], GL_STATIC_DRAW));
  
  // Upload vertices 
  kit::GL::bindBuffer(GL_ARRAY_BUFFER, this->m_glVertexBuffer);
  KIT_GL(glBufferData(GL_ARRAY_BUFFER, data.m_vertices.size() * 19 * sizeof(float) , &data.m_vertices[0], GL_STATIC_DRAW));
  
  // Total size
  uint32_t attributeSize = (sizeof(GLfloat) * 15) + (sizeof(GLint) * 4);
  
  // Positions
  KIT_GL(glEnableVertexAttribArray(0));
  KIT_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, attributeSize, (void*)0));
  
  // Texture coordinates
  KIT_GL(glEnableVertexAttribArray(1));
  KIT_GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, attributeSize, (void*) (sizeof(GLfloat) * 3) ));

  // Normals
  KIT_GL(glEnableVertexAttribArray(2));
  KIT_GL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, attributeSize, (void*) (sizeof(GLfloat) * 5) ));
  
  // Tangents
  KIT_GL(glEnableVertexAttribArray(3));
  KIT_GL(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, attributeSize, (void*) (sizeof(GLfloat) * 8) ));
  
  // Bone ID's
  KIT_GL(glEnableVertexAttribArray(4));
  KIT_GL(glVertexAttribIPointer(4, 4, GL_INT, attributeSize, (void*) (sizeof(GLfloat) * 11)  ));
  
  // Bone weights
  KIT_GL(glEnableVertexAttribArray(5));
  KIT_GL(glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, attributeSize, (void*)((sizeof(GLfloat) * 11) + (sizeof(GLint) * 4)) ));
}
