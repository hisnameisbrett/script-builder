#include "crab.h"

namespace EE
{
    REGISTER_TYPE_DEFINITION(crab); // self-registry

    crab::crab()
    {

    }

    // crab::crab(const crab & rhs) : Component(rhs) {} [for custom copy constructors]

    void crab::Initialize()
    {

    }

    crab::~crab()
    {

    }

    void crab::SerializeSelf(Json::Value & comp_root) const
    {
        //Serialize(mem0_, "mem0_", comp_root); 
        //Serialize(mem1_, "mem1_", comp_root);
    }

    ComponentPtr crab::DeserializeSelf(Json::Value& comp_root)
    {
        //Deserialize(mem0_, "mem0_", comp_root); 
        //Deserialize(mem1_, "mem1_", comp_root);

        return this;
    }

    void crab::EditMode()
    {
        //float center[2] = { center_.x, center_.y };
        //if (ImGui::DragFloat2("Center", center))
        //	SetCenter(glm::vec2(center[0], center[1]));
    }
}
