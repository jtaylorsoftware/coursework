#include "engine/object_factory.hpp"

View<ObjectFactory::ObjectList> ObjectFactory::GetAllObjects()
{
    return View<ObjectList>(objects);
}
