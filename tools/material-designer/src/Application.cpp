#include "../include/Application.hpp"
#include <chaiscript/utility/utility.hpp>

#include "Kit/Window.hpp"
#include "Kit/Quad.hpp"
#include "Kit/Renderer.hpp"
#include "Kit/Text.hpp"

kmd::Application::Application() : kit::Application()
{

}

kmd::Application::~Application()
{

}

kit::Renderer::Ptr kmd::Application::getRenderer()
{
  return this->m_renderer;
}

void kmd::Application::onInitialize()
{
  kit::Application::onInitialize();
  this->getWindow()->maximize();
  this->getWindow()->setVSync(false);
  this->setRenderRate(1000.0 / 240.0);
  this->setUpdateRate(1000.0 / 240.0);
  this->m_renderer = kit::Renderer::create( glm::uvec2( this->getWindow()->getFramebufferSize().x, this->getWindow()->getFramebufferSize().y)); 
  this->m_fullscreenQuad = kit::Quad::create();
  this->m_scriptEngine.add_global(chaiscript::var(this->m_renderer), "renderer");
}

void kmd::Application::onRender()
{
  kit::Application::onRender();

  this->m_renderer->renderFrame();
  
  this->getWindow()->bind();
  this->m_fullscreenQuad->setTexture(this->m_renderer->getBuffer());
  this->m_fullscreenQuad->render();
  this->getRenderer()->getMetricsText()->setPosition(glm::vec2(32.0f, 32.0f));
  this->getRenderer()->getMetricsText()->renderShadowed(this->getWindow()->getFramebufferSize(), glm::vec2(1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
}

void kmd::Application::onResize(glm::uvec2 newsize)
{
  kit::Application::onResize(newsize);
  if(newsize.x > 0 && newsize.y > 0)
  {
    std::cout << "App onresize " << newsize.x << "x" << newsize.y << std::endl;
    this->m_renderer->setResolution(newsize);
  }
}
