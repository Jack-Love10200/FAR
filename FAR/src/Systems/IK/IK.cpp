
#include "PCH/PCH.hpp"
#include "IK.hpp"

#include "Engine/Engine.hpp"
#include "Components/Model.hpp"

namespace FAR
{
  void IK::Init()
  {

  }

  void IK::PreUpdate()
  {
    
  }

  void IK::Update()
  {
    std::vector<Entity> ents = Engine::GetInstance()->GetEntities<Model>();
    
    for (Entity e : ents)
    {
      Model& model = Engine::GetInstance()->GetComponent<Model>(e);

      //get euler angles of the bones
      for (Model::Node n : model.nodes)
      {
        
        //std::cout << n.transform
        //continue;
      }
      
    }
  }

  void IK::PostUpdate()
  {

  }

  void IK::Exit()
  {

  }
}