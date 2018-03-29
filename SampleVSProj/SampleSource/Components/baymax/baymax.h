#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct baymax : public Component
    {
        baymax();
        baymax(const baymax& rhs) = default;
        ~baymax();
        baymax* Clone() const override { return new baymax(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<baymax> reg;	// self-register
    };
}
