#include "$(CLASS_NAME).h"

namespace Example
{
    REGISTER_TYPE_DEFINITION($(CLASS_NAME)); // self-registry

    $(CLASS_NAME)::$(CLASS_NAME)()
    {

    }

    // $(CLASS_NAME)::$(CLASS_NAME)(const $(CLASS_NAME) & rhs) : Component(rhs) {} [for custom copy constructors]

    void $(CLASS_NAME)::Initialize()
    {

    }

    $(CLASS_NAME)::~$(CLASS_NAME)()
    {

    }

    void $(CLASS_NAME)::SerializeSelf(Json::Value & comp_root) const
    {
        //Serialize(mem0_, "mem0_", comp_root); 
        //Serialize(mem1_, "mem1_", comp_root);
    }

    ComponentPtr $(CLASS_NAME)::DeserializeSelf(Json::Value& comp_root)
    {
        //Deserialize(mem0_, "mem0_", comp_root); 
        //Deserialize(mem1_, "mem1_", comp_root);

        return this;
    }

    void $(CLASS_NAME)::EditMode()
    {
        //float center[2] = { center_.x, center_.y };
        //if (ImGui::DragFloat2("Center", center))
        //	SetCenter(glm::vec2(center[0], center[1]));
    }
}
