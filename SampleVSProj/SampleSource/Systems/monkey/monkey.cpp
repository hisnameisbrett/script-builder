#include "monkey.h"
#include "core\Space\Entity\Entity.h"

namespace EE
{
	//enum { GROUP0, GROUP1};

	monkey::monkey() :
		System(typeid(monkey))
	{}

	void monkey::Init()
	{
		//RegisterType<ComponentTypeA>(GROUP0);
		//RegisterType<ComponentTypeB>(GROUP0);
		//
		//RegisterType<ComponentTypeC>(GROUP1);
	}

	// called every frame, for each space that is using it
	void monkey::Update(float dt)
	{
		// access  groups
		//EntityVectorPtr group0_entities = (*entity_filters_)[GROUP0];
		//for (auto it = group0_entities->begin(); it != group0_entities->end(); ++it)
		//{
			//EntityPtr pEnt = *it;
			//auto pComponent = pEnt->GetComponent<ComponentTypeA>();
			// ...do stuff....
		//}

	}

	// any logic that should happen when system is completely removed
	void monkey::Destruct()
	{

	}

	// subscribing to events (optional, but useful)
	void monkey::SpaceInit(const SpacePtr space)
	{
		//auto cb = GenerateHandlerFn(this, &monkey::OnDopeEvent);
		//SubscribeLocal<DopeEvent>(cb);
	}

	// make sure to unsubscribe from events (if applicable)
	void monkey::SpaceDestruct(const SpacePtr space)
	{
		//UnsubscribeLocal<DopeEvent>();

	}

}
