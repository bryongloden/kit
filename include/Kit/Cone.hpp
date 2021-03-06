#ifndef KIT_CONE_HPP
#define KIT_CONE_HPP

#include "Kit/Export.hpp"
#include "Kit/Types.hpp"
#include "Kit/GL.hpp"

#include <memory>

namespace kit
{
  class KITAPI Cone
  {
  public:
    typedef std::shared_ptr<kit::Cone> Ptr;

    Cone(float radius, float depth, uint32_t sectors);
    ~Cone();

    static kit::Cone::Ptr create(float radius, float depth, uint32_t sectors);

    void renderGeometry();

  private:
    kit::GL m_glSingleton;

    // Individual GPU data
    void allocateBuffers();
    void releaseBuffers();

    GLuint m_glVertexArray;
    GLuint m_glVertexIndices;
    GLuint m_glVertexBuffer;

    uint32_t m_indexCount;

  };
}

#endif // KIT_CONE_HPP