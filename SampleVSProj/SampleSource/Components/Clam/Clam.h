#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct Clam : public Component
    {
        Clam();
        Clam(const Clam& rhs) = default;
        ~Clam();
        Clam* Clone() const override { return new Clam(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<Clam> reg;	// self-register
    };
}
