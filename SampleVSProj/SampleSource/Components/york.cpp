#include "york.h"

namespace EE
{
    REGISTER_TYPE_DEFINITION(york); // self-registry

    york::york()
    {

    }

    // york::york(const york & rhs) : Component(rhs) {} [for custom copy constructors]

    void york::Initialize()
    {

    }

    york::~york()
    {

    }

    void york::SerializeSelf(Json::Value & comp_root) const
    {
        //Serialize(mem0_, "mem0_", comp_root); 
        //Serialize(mem1_, "mem1_", comp_root);
    }

    ComponentPtr york::DeserializeSelf(Json::Value& comp_root)
    {
        //Deserialize(mem0_, "mem0_", comp_root); 
        //Deserialize(mem1_, "mem1_", comp_root);

        return this;
    }

    void york::EditMode()
    {
        //float center[2] = { center_.x, center_.y };
        //if (ImGui::DragFloat2("Center", center))
        //	SetCenter(glm::vec2(center[0], center[1]));
    }
}