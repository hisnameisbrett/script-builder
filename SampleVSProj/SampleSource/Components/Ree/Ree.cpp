#include "Ree.h"

namespace EE
{
    REGISTER_TYPE_DEFINITION(Ree); // self-registry

    Ree::Ree()
    {

    }

    // Ree::Ree(const Ree & rhs) : Component(rhs) {} [for custom copy constructors]

    void Ree::Initialize()
    {

    }

    Ree::~Ree()
    {

    }

    void Ree::SerializeSelf(Json::Value & comp_root) const
    {
        //Serialize(mem0_, "mem0_", comp_root); 
        //Serialize(mem1_, "mem1_", comp_root);
    }

    ComponentPtr Ree::DeserializeSelf(Json::Value& comp_root)
    {
        //Deserialize(mem0_, "mem0_", comp_root); 
        //Deserialize(mem1_, "mem1_", comp_root);

        return this;
    }

    void Ree::EditMode()
    {
        //float center[2] = { center_.x, center_.y };
        //if (ImGui::DragFloat2("Center", center))
        //	SetCenter(glm::vec2(center[0], center[1]));
    }
}
