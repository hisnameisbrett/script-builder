#include "Clam.h"

namespace EE
{
    REGISTER_TYPE_DEFINITION(Clam); // self-registry

    Clam::Clam()
    {

    }

    // Clam::Clam(const Clam & rhs) : Component(rhs) {} [for custom copy constructors]

    void Clam::Initialize()
    {

    }

    Clam::~Clam()
    {

    }

    void Clam::SerializeSelf(Json::Value & comp_root) const
    {
        //Serialize(mem0_, "mem0_", comp_root); 
        //Serialize(mem1_, "mem1_", comp_root);
    }

    ComponentPtr Clam::DeserializeSelf(Json::Value& comp_root)
    {
        //Deserialize(mem0_, "mem0_", comp_root); 
        //Deserialize(mem1_, "mem1_", comp_root);

        return this;
    }

    void Clam::EditMode()
    {
        //float center[2] = { center_.x, center_.y };
        //if (ImGui::DragFloat2("Center", center))
        //	SetCenter(glm::vec2(center[0], center[1]));
    }
}
