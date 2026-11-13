/********************************* ECS ***********************************
 * ecs: Entity Component System
 *************************************************************************/

#include "engine/ecs.hpp"
#include "engine/log.hpp"

#include <algorithm> /* std::remove_if */

namespace en
{
    ComponentId IComponent::nextId = 0;

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
                                          [&entity]( Entity other ) -> bool { return entity == other; } ),
                          m_entities.end() );
    }

    //--- REGISTRY ---------------------------------------------------------------------------------------------

    // Process the entities queued for addition and removal
    void
    Registry::Update()
    {
        // Process entities to be created
        //----------------------------------------------------------
        for( const auto & entity : m_entitiesToBeAdded )
            {
                AddEntityToSystems( entity );
            }
        m_entitiesToBeAdded.clear();

        // Process entities to be killed
        //----------------------------------------------------------
        for( const auto & entity : m_entitiesToBeKilled )
            {
                RemoveEntityFromSystems( entity );

                if( m_entityComponentSignatures.size() > entity.GetId() )
                    {
                        m_entityComponentSignatures[entity.GetId()].reset(); // Dead entity holds no components
                    }
            }
        m_entitiesToBeKilled.clear();
    }

    // Signals to create an entity
    Entity
    Registry::CreateEntity()
    {
        EntityId entityId;

        entityId = static_cast< EntityId >( m_numEntities++ );
        Entity entity( entityId );

        if( m_entityComponentSignatures.size() <= entityId )
            {
                m_entityComponentSignatures.resize( entityId + 1 );
            }
        m_entitiesToBeAdded.insert( entity );

        EN_LOG_INFO( "Entity created with id '{}'", entityId );

        return entity;
    }

    // Signals to remove an entity
    void
    Registry::KillEntity( Entity entity )
    {
        m_entitiesToBeKilled.insert( entity );
        EN_LOG_INFO( "Entity '{}' was killed", entity.GetId() );
    }

    // Adds the entity to all systems that match its component signature
    void
    Registry::AddEntityToSystems( Entity entity )
    {
        const auto entityId                   = entity.GetId();
        const auto & entityComponentSignature = m_entityComponentSignatures[entityId];

        for( const auto & [typeIndex, system] : m_systems )
            {
                const auto & systemComponentSignature = system->GetComponentSignature();

                // Does the system component signature match the entity one?
                if( EN_FLAGS_CONTAIN( entityComponentSignature, systemComponentSignature ) )
                    {
                        system->AddEntityToSystem( entity );
                    }
            }
    }

    // Removes the entity from all systems that hold it
    void
    Registry::RemoveEntityFromSystems( Entity entity )
    {
        for( const auto & [typeIndex, system] : m_systems )
            {
                system->RemoveEntityFromSystem( entity );
            }
    }

} // namespace en
