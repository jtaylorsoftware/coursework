#include "engine/object.hpp"

Object::Object()
{
}


Object::~Object()
{
}


void Object::Draw()
{
}

void Object::Init()
{
    for ( auto& component : components )
    {
        component->Init();
    }
}

void Object::Update( float deltaTime )
{
    for ( auto& component : components )
    {
        component->Update( deltaTime );
    }
}

View<Object::ComponentList> Object::GetAllComponents()
{
    return View<ComponentList>(components);
}

