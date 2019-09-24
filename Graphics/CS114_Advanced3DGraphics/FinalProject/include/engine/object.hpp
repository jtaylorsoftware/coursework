#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "engine/component.hpp"
#include "engine/mesh.hpp"
#include "engine/renderer.hpp"

#include "utility/view.hpp"

class Object
{
public:
    using ComponentList = std::vector<std::shared_ptr<Component>>;

    Object();
    virtual ~Object();

    virtual void Draw();
    virtual void Init();
    virtual void Update( float deltaTime ); // update per-frame values

    template<typename component_t>
    void AttachComponent()
    {
        static_assert( std::is_base_of<Component, component_t>::value,
            "component_t must inherit from Component" );

        components.push_back( std::static_pointer_cast< Object >( std::make_shared<component_t>() ) );
    }

    template<typename component_t>
    std::shared_ptr<component_t> GetFirstComponent()
    {
        static_assert( std::is_base_of<Component, component_t>::value,
            "component_t must inherit from Component" );

        auto iterToComponent = std::find_if( components.begin(), components.end(),
            [] ( std::shared_ptr<Component> comp ) {
            return comp->GetHashedTypeId() == component_t->GetHashedTypeId();
        } );

        return std::dynamic_pointer_cast< component_t >( *iterToComponent );
    }

    template<typename component_t>
    std::vector<std::shared_ptr<component_t>> GetComponents()
    {
        static_assert( std::is_base_of<Component, component_t>::value,
            "component_t must inherit from Component" );

        std::vector<std::weak_ptr<component_t>> matches;
        std::for_each( components.begin(), components.end(),
            [ &matches ] ( std::shared_ptr<Component> comp ) {
            if ( comp->GetHashedTypeId() == component_t->GetHashedTypeId() )
            {
                matches.push_back( std::dynamic_pointer_cast<component_t>(comp) );
            }
        } );
    }

    View<ComponentList> GetAllComponents();

private:

    ComponentList components;
    bool exists;
};
