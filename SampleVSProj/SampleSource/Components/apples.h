#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct apples : public Component
    {
        apples();
        apples(const apples& rhs) = default;
        ~apples();
        apples* Clone() const override { return new apples(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<apples> reg;	// self-register
    };
}
