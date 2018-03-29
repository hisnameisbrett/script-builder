#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct borker : public Component
    {
        borker();
        borker(const borker& rhs) = default;
        ~borker();
        borker* Clone() const override { return new borker(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<borker> reg;	// self-register
    };
}
