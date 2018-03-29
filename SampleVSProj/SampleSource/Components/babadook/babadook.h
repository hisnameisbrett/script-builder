#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct babadook : public Component
    {
        babadook();
        babadook(const babadook& rhs) = default;
        ~babadook();
        babadook* Clone() const override { return new babadook(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<babadook> reg;	// self-register
    };
}
