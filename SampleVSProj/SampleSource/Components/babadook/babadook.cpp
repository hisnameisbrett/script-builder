#include "babadook.h"

namespace EE
{
    REGISTER_TYPE_DEFINITION(babadook); // self-registry

    babadook::babadook()
    {

    }

    // babadook::babadook(const babadook & rhs) : Component(rhs) {} [for custom copy constructors]

    void babadook::Initialize()
    {

    }

    babadook::~babadook()
    {

    }

    void babadook::SerializeSelf(Json::Value & comp_root) const
    {
        //Serialize(mem0_, "mem0_", comp_root); 
        //Serialize(mem1_, "mem1_", comp_root);
    }

    ComponentPtr babadook::DeserializeSelf(Json::Value& comp_root)
    {
        //Deserialize(mem0_, "mem0_", comp_root); 
        //Deserialize(mem1_, "mem1_", comp_root);

        return this;
    }

    void babadook::EditMode()
    {
        //float center[2] = { center_.x, center_.y };
        //if (ImGui::DragFloat2("Center", center))
        //	SetCenter(glm::vec2(center[0], center[1]));
    }
}
