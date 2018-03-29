#include "zzz.h"
#include "core\Space\Entity\Entity.h"

namespace EE
{
	//enum { GROUP0, GROUP1};

	zzz::zzz() :
		System(typeid(zzz))
	{}

	void zzz::Init()
	{
		//RegisterType<ComponentTypeA>(GROUP0);
		//RegisterType<ComponentTypeB>(GROUP0);
		//
		//RegisterType<ComponentTypeC>(GROUP1);
	}

	// called every frame, for each space that is using it
	void zzz::Update(float dt)
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
	void zzz::Destruct()
	{

	}

	// subscribing to events (optional, but useful)
	void zzz::SpaceInit(const SpacePtr space)
	{
		//auto cb = GenerateHandlerFn(this, &zzz::OnDopeEvent);
		//SubscribeLocal<DopeEvent>(cb);
	}

	// make sure to unsubscribe from events (if applicable)
	void zzz::SpaceDestruct(const SpacePtr space)
	{
		//UnsubscribeLocal<DopeEvent>();

	}

}
