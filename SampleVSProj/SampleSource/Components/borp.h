#pragma once
#include "core\Space\Component\Component.h"

namespace EE
{
    struct borp : public Component
    {
        borp();
        borp(const borp& rhs) = default;
        ~borp();
        borp* Clone() const override { return new borp(*this); }
        void Initialize() override;

        void SerializeSelf(Json::Value& comp_root) const override;
        ComponentPtr DeserializeSelf(Json::Value& comp_root) override;
        void EditMode() override;

    private:
        static DerivedRegister<borp> reg;	// self-register
    };
}
