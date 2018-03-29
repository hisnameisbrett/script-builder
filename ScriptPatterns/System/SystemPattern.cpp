#include "$(CLASS_NAME).h"
#include "core\Space\Entity\Entity.h"

namespace Example
{
	//enum { GROUP0, GROUP1};

	$(CLASS_NAME)::$(CLASS_NAME)() :
		System(typeid($(CLASS_NAME)))
	{}

	void $(CLASS_NAME)::Init()
	{
		//RegisterType<ComponentTypeA>(GROUP0);
		//RegisterType<ComponentTypeB>(GROUP0);
		//
		//RegisterType<ComponentTypeC>(GROUP1);
	}

	// called every frame, for each space that is using it
	void $(CLASS_NAME)::Update(float dt)
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
	void $(CLASS_NAME)::Destruct()
	{

	}

	// subscribing to events (optional, but useful)
	void $(CLASS_NAME)::SpaceInit(const SpacePtr space)
	{
		//auto cb = GenerateHandlerFn(this, &$(CLASS_NAME)::OnDopeEvent);
		//SubscribeLocal<DopeEvent>(cb);
	}

	// make sure to unsubscribe from events (if applicable)
	void $(CLASS_NAME)::SpaceDestruct(const SpacePtr space)
	{
		//UnsubscribeLocal<DopeEvent>();

	}

}
