/********************************* ECS ***********************************
 * ecs: Entity Component System
 *************************************************************************/

#include "engine/ecs.hpp"

#include <algorithm> /* std::remove_if */

namespace en
{
    unsigned int IComponent::nextId = 0;

    //--- SYSTEM -----------------------------------------------------------------------------------------------

    void
    System::AddEntityToSystem( Entity entity )
    {
        m_entities.push_back( entity );
    }

    void
    System::RemoveEntityFromSystem( Entity entity )
    {
        m_entities.erase( std::remove_if( m_entities.begin(), m_entities.end(),
                                          [&]( Entity other ) -> bool { return other == entity; } ),
                          m_entities.end() );
    }
} // namespace en
