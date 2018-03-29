#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct ree : public Component
    {
        ree();
        ree(const ree& rhs) = default;
        ~ree();
        ree* Clone() const override { return new ree(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<ree> reg;	// self-register
    };
}
