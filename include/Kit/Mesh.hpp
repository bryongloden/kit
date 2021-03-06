#ifndef KIT_MESH_HPP
#define KIT_MESH_HPP

#include "Kit/Export.hpp"
#include "Kit/GL.hpp"

#include <memory>
#include <map>

namespace kit
{
  class Submesh;
  typedef std::shared_ptr<Submesh> SubmeshPtr;

  class Material;
  typedef std::shared_ptr<Material> MaterialPtr;

  class ConvexHull;
  typedef std::shared_ptr<ConvexHull> ConvexHullPtr;
  
  class Camera;
  typedef std::shared_ptr<Camera> CameraPtr;

  class KITAPI Mesh 
  {
    public:
      
      struct SubmeshEntry 
      {
        kit::SubmeshPtr  m_submesh;
        kit::MaterialPtr m_material;
      };
      
      typedef std::shared_ptr<Mesh> Ptr;
      
      ~Mesh();

      static kit::Mesh::Ptr create();
      static kit::Mesh::Ptr load(std::string filename);
      
      void render(kit::CameraPtr cam, const glm::mat4 & modelMatrix, bool forward, const std::vector<glm::mat4> & skintransform = std::vector<glm::mat4>());
      void renderGeometry();

      void addSubmeshEntry(std::string name, kit::SubmeshPtr geometry, kit::MaterialPtr material);
      
      void setSubmeshEnabled(std::string name, bool s);

      std::vector<kit::ConvexHullPtr> & getHull();
      
      kit::Mesh::SubmeshEntry * getSubmeshEntry(std::string name);
      std::map<std::string, kit::Mesh::SubmeshEntry> & getSubmeshEntries();
      Mesh();

    private:
      kit::GL m_glSingleton;
      std::map<std::string, kit::Mesh::SubmeshEntry> m_submeshEntries;
      std::map<std::string, bool> m_submeshesEnabled;
      
      std::vector<kit::ConvexHullPtr> m_hull;

  };

}

#endif