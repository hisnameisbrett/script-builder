#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct Salmon : public Component
    {
        Salmon();
        Salmon(const Salmon& rhs) = default;
        ~Salmon();
        Salmon* Clone() const override { return new Salmon(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<Salmon> reg;	// self-register
    };
}
