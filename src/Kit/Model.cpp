#include "Kit/Model.hpp"
#include "Kit/Mesh.hpp"
#include "Kit/Camera.hpp"
#include "Kit/Material.hpp"
#include "Kit/Submesh.hpp"
#include "Kit/Program.hpp"
#include "Kit/Texture.hpp"
#include "Kit/Renderer.hpp"

#include <glm/gtx/transform.hpp>

uint32_t kit::Model::m_instanceCount = 0;
kit::Program::Ptr kit::Model::m_programShadow = nullptr;
kit::Program::Ptr kit::Model::m_programShadowO = nullptr;
kit::Program::Ptr kit::Model::m_programShadowS = nullptr;
kit::Program::Ptr kit::Model::m_programShadowSO = nullptr;

kit::Model::Model() : kit::Renderable::Renderable()
{
  kit::Model::m_instanceCount++;
  if (kit::Model::m_instanceCount == 1)
  {
    kit::Model::allocateShared();
  }
}

kit::Model::~Model()
{
  kit::Model::m_instanceCount--;
  if (kit::Model::m_instanceCount == 0)
  {
    kit::Model::releaseShared();
  }
}

void kit::Model::allocateShared()
{
  kit::Model::m_programShadow = kit::Program::load({ "lighting/shadow.vert" }, {"lighting/shadow.frag"});
  kit::Model::m_programShadowO = kit::Program::load({ "lighting/shadow.vert" }, { "lighting/shadow-opacitymasked.frag" });
  kit::Model::m_programShadowS = kit::Program::load({ "lighting/shadow-skinned.vert" }, { "lighting/shadow.frag" });
  kit::Model::m_programShadowSO = kit::Program::load({ "lighting/shadow-skinned.vert" }, { "lighting/shadow-opacitymasked.frag" });
}

void kit::Model::releaseShared()
{
  kit::Model::m_programShadow.reset();
  kit::Model::m_programShadowO.reset();
  kit::Model::m_programShadowS.reset();
  kit::Model::m_programShadowSO.reset();
}

kit::Model::Ptr kit::Model::create(std::string mesh)
{
  kit::Model::Ptr returner = std::make_shared<kit::Model>();
  returner->m_mesh = kit::Mesh::load(mesh);
  returner->m_skeleton = nullptr;
  return returner;
}

kit::Model::Ptr kit::Model::create(kit::Mesh::Ptr mesh)
{
  kit::Model::Ptr returner = std::make_shared<kit::Model>();
  returner->m_mesh = mesh;
  returner->m_skeleton = nullptr;
  return returner;
}

kit::Model::Ptr kit::Model::create(std::string mesh, std::string skeleton)
{
  kit::Model::Ptr returner = std::make_shared<kit::Model>();
  returner->m_mesh = kit::Mesh::load(mesh);
  returner->m_skeleton = kit::Skeleton::load(skeleton);
  return returner;
}

kit::Mesh::Ptr kit::Model::getMesh()
{
  return this->m_mesh;
}

kit::Skeleton::Ptr kit::Model::getSkeleton()
{
  return this->m_skeleton;
}

void kit::Model::update(const double & ms)
{
  if(this->m_skeleton != nullptr)
  {
    this->m_skeleton->update(ms);
  }
}

void kit::Model::renderDeferred(kit::Renderer::Ptr renderer)
{
  if(this->m_skeleton  != nullptr)
  {
    this->m_mesh->render(renderer->getActiveCamera(), this->getTransformMatrix(), false, this->m_skeleton->getSkin());
  }
  else
  {
    this->m_mesh->render(renderer->getActiveCamera(), this->getTransformMatrix(), false);
  }
}

void kit::Model::renderForward(kit::Renderer::Ptr renderer)
{
  if (this->m_skeleton != nullptr)
  {
    this->m_mesh->render(renderer->getActiveCamera(), this->getTransformMatrix(), true, this->m_skeleton->getSkin());
  }
  else
  {
    this->m_mesh->render(renderer->getActiveCamera(), this->getTransformMatrix(), true);
  }
}

void kit::Model::renderShadows(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
  for (auto &currSubmeshIndex : this->m_mesh->getSubmeshEntries())
  {
    auto & currMaterial = currSubmeshIndex.second.m_material;
    auto & currSubmesh = currSubmeshIndex.second.m_submesh;

    if (!currMaterial->getCastShadows())
    {
      continue;
    }

    if (currMaterial->getDoubleSided())
    {
      kit::GL::disable(GL_CULL_FACE);
    }
    else
    {
      kit::GL::enable(GL_CULL_FACE);
      kit::GL::cullFace(GL_BACK);
    }

    bool opacityMask = (currMaterial->getOpacityMask() != nullptr);
    bool skinned = (this->m_skeleton != nullptr);

    if (opacityMask)
    {
      if (skinned)
      {
        // SO
        this->m_programShadowSO->use();
        this->m_programShadowSO->setUniformTexture("uniform_opacityMask", currMaterial->getOpacityMask());
        this->m_programShadowSO->setUniformMat4v("uniform_bones", this->m_skeleton->getSkin());
        this->m_programShadowSO->setUniformMat4("uniform_mvpMatrix", projectionMatrix * viewMatrix * this->getTransformMatrix());
      }
      else
      {
        //O
        this->m_programShadowO->use();
        this->m_programShadowO->setUniformTexture("uniform_opacityMask", currMaterial->getOpacityMask());
        this->m_programShadowO->setUniformMat4("uniform_mvpMatrix", projectionMatrix * viewMatrix * this->getTransformMatrix());
      }
    }
    else
    {
      if (skinned)
      {
        //S
        this->m_programShadowS->use();
        this->m_programShadowS->setUniformMat4v("uniform_bones", this->m_skeleton->getSkin());
        this->m_programShadowS->setUniformMat4("uniform_mvpMatrix", projectionMatrix * viewMatrix * this->getTransformMatrix());
      }
      else
      {
        //None
        this->m_programShadow->use();
        this->m_programShadow->setUniformMat4("uniform_mvpMatrix", projectionMatrix * viewMatrix * this->getTransformMatrix());
      }
    }

    currSubmesh->renderGeometry();
  }
}

void kit::Model::renderGeometry()
{
  this->m_mesh->renderGeometry();
}

bool kit::Model::isSkinned()
{
  return (this->m_skeleton != nullptr);
}

std::vector<glm::mat4> kit::Model::getSkin()
{
  if (this->m_skeleton == nullptr)
  {
    KIT_ERR("Warning: tried to get skin from non-skinned model");
    return std::vector<glm::mat4>();
  }

  return this->m_skeleton->getSkin();
}

glm::vec3 kit::Model::getBoneWorldPosition(std::string bone)
{
  if (!this->m_skeleton)
  {
    KIT_ERR("Warning: tried to get bone position from non-skinned model");
    return glm::vec3();
  }

  kit::Skeleton::Bone::Ptr currBone = this->m_skeleton->getBone(bone);
  if (!currBone)
  {
    KIT_ERR("Warning: tried to get bone position from non-existent bone");
    return glm::vec3();
  }

  return glm::vec3( this->getTransformMatrix() * currBone->m_globalTransform * glm::vec4(0.0, 0.0, 0.0, 1.0));
}

glm::quat kit::Model::getBoneWorldRotation(std::string bone)
{
  if (!this->m_skeleton)
  {
    KIT_ERR("Warning: tried to get bone rotation from non-skinned model");
    return glm::quat();
  }

  kit::Skeleton::Bone::Ptr currBone = this->m_skeleton->getBone(bone);
  if (!currBone)
  {
    KIT_ERR("Warning: tried to get bone rotation from non-existent bone");
    return glm::quat();
  }

  // TODO: This (fodderFix) is needed for badly exported/imported models. Fix our importer/blenders exporter/whatever and then remove this.
  glm::quat fodderFix;
  fodderFix = glm::rotate(fodderFix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  fodderFix = glm::rotate(fodderFix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  return glm::quat_cast(this->getTransformMatrix() * currBone->m_globalTransform) * fodderFix;
}

kit::ProgramPtr kit::Model::getShadowProgram(bool s, bool o)
{
  if(s && o)
  {
    return kit::Model::m_programShadowSO;
  }
  
  if(s && !o)
  {
    return kit::Model::m_programShadowS;
  }
  
  if(!s && o)
  {
    return kit::Model::m_programShadowO;
  }
  
  if(!s && !o)
  {
    return kit::Model::m_programShadow;
  }
  
    return kit::Model::m_programShadow;
}
