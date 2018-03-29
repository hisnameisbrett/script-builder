#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct barg : public Component
    {
        barg();
        barg(const barg& rhs) = default;
        ~barg();
        barg* Clone() const override { return new barg(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<barg> reg;	// self-register
    };
}
