#pragma once

#include <type_traits>
#include <memory>
#include <vector>

#include "engine/object.hpp"
#include "utility/view.hpp"


class ObjectFactory
{
public:
    using ObjectList = std::vector<std::shared_ptr<Object>>;

    template<typename derived_t>
    std::shared_ptr<derived_t> CreateObject();

    View<ObjectList> GetAllObjects();
private:
    ObjectList objects;
};

template<typename derived_t>
inline std::shared_ptr<derived_t> ObjectFactory::CreateObject()
{
    static_assert( std::is_base_of<Object, derived_t>::value, 
        "Type created by ObjectFactory must inherit from Object" );

    auto obj = std::shared_ptr<derived_t>( new derived_t() );

    objects.push_back( std::static_pointer_cast<Object>(obj) );

    return obj;
}
