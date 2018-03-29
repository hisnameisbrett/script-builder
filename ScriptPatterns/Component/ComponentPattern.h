#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct $(CLASS_NAME) : public Component
    {
        $(CLASS_NAME)();
        $(CLASS_NAME)(const $(CLASS_NAME)& rhs) = default;
        ~$(CLASS_NAME)();
        $(CLASS_NAME)* Clone() const override { return new $(CLASS_NAME)(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<$(CLASS_NAME)> reg;	// self-register
    };
}
