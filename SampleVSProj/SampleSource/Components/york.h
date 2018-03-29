#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct york : public Component
    {
        york();
        york(const york& rhs) = default;
        ~york();
        york* Clone() const override { return new york(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<york> reg;	// self-register
    };
}
