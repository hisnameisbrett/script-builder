#pragma once
#include "core\System\System.h"

namespace EE
{
    class SystemPattern : public System
    {
    public:
		SystemPattern();

        virtual void Init();
        virtual void Update(float dt);
        virtual void Destruct();

        virtual void SpaceInit(const SpacePtr space);
        virtual void SpaceDestruct(const SpacePtr space);
    };

}
