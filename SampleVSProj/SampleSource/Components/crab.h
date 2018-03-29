#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct crab : public Component
    {
        crab();
        crab(const crab& rhs) = default;
        ~crab();
        crab* Clone() const override { return new crab(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<crab> reg;	// self-register
    };
}
