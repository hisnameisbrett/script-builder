#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct Blue : public Component
    {
        Blue();
        Blue(const Blue& rhs) = default;
        ~Blue();
        Blue* Clone() const override { return new Blue(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<Blue> reg;	// self-register
    };
}
