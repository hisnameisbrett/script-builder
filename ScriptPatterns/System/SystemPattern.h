#pragma once
#include "core\System\System.h"

namespace EE
{
    class $(CLASS_NAME) : public System
    {
    public:
		$(CLASS_NAME)();

        virtual void Init();
        virtual void Update(float dt);
        virtual void Destruct();

        virtual void SpaceInit(const SpacePtr space);
        virtual void SpaceDestruct(const SpacePtr space);
    };

}
