#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct Ree : public Component
    {
        Ree();
        Ree(const Ree& rhs) = default;
        ~Ree();
        Ree* Clone() const override { return new Ree(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<Ree> reg;	// self-register
    };
}
