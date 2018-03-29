#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct shleem : public Component
    {
        shleem();
        shleem(const shleem& rhs) = default;
        ~shleem();
        shleem* Clone() const override { return new shleem(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<shleem> reg;	// self-register
    };
}
