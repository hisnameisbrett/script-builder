#include "ree.h"

namespace EE
{
    REGISTER_TYPE_DEFINITION(ree); // self-registry

    ree::ree()
    {

    }

    // ree::ree(const ree & rhs) : Component(rhs) {} [for custom copy constructors]

    void ree::Initialize()
    {

    }

    ree::~ree()
    {

    }

    void ree::SerializeSelf(Json::Value & comp_root) const
    {
        //Serialize(mem0_, "mem0_", comp_root); 
        //Serialize(mem1_, "mem1_", comp_root);
    }

    ComponentPtr ree::DeserializeSelf(Json::Value& comp_root)
    {
        //Deserialize(mem0_, "mem0_", comp_root); 
        //Deserialize(mem1_, "mem1_", comp_root);

        return this;
    }

    void ree::EditMode()
    {
        //float center[2] = { center_.x, center_.y };
        //if (ImGui::DragFloat2("Center", center))
        //	SetCenter(glm::vec2(center[0], center[1]));
    }
}
