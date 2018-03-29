#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct Borkerino : public Component
    {
        Borkerino();
        Borkerino(const Borkerino& rhs) = default;
        ~Borkerino();
        Borkerino* Clone() const override { return new Borkerino(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<Borkerino> reg;	// self-register
    };
}
