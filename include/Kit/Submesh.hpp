#ifndef KIT_SUBMESH_HPP
#define KIT_SUBMESH_HPP

#include "Kit/Export.hpp"
#include "Kit/Types.hpp"
#include "Kit/GL.hpp"

#include <memory>

namespace kit 
{
  class KITAPI Submesh 
  {
    public:
      typedef std::shared_ptr<kit::Submesh> Ptr;
      
      ~Submesh();
      
      static kit::Submesh::Ptr load(std::string geometry);
      static void flushCache();
      
      void renderGeometry();
      Submesh(std::string filename);

    private:
      void loadGeometry(std::string filename);
      
      kit::GL m_glSingleton;
      
      // Cache
      static std::map<std::string, kit::Submesh::Ptr> m_cache;
      
      // Individual GPU data
      void allocateBuffers();
      void releaseBuffers();
      
      GLuint m_glVertexArray;   
      GLuint m_glVertexIndices; 
      GLuint m_glVertexBuffer;
      
      uint32_t m_indexCount;

  };
}

#endif // KIT_SUBMESH_HPP