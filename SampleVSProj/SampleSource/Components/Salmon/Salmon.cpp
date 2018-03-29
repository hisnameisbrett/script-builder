#include "Salmon.h"

namespace EE
{
    REGISTER_TYPE_DEFINITION(Salmon); // self-registry

    Salmon::Salmon()
    {

    }

    // Salmon::Salmon(const Salmon & rhs) : Component(rhs) {} [for custom copy constructors]

    void Salmon::Initialize()
    {

    }

    Salmon::~Salmon()
    {

    }

    void Salmon::SerializeSelf(Json::Value & comp_root) const
    {
        //Serialize(mem0_, "mem0_", comp_root); 
        //Serialize(mem1_, "mem1_", comp_root);
    }

    ComponentPtr Salmon::DeserializeSelf(Json::Value& comp_root)
    {
        //Deserialize(mem0_, "mem0_", comp_root); 
        //Deserialize(mem1_, "mem1_", comp_root);

        return this;
    }

    void Salmon::EditMode()
    {
        //float center[2] = { center_.x, center_.y };
        //if (ImGui::DragFloat2("Center", center))
        //	SetCenter(glm::vec2(center[0], center[1]));
    }
}
