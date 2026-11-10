/*******************************************************************************************
 *
 *   2D Game Engine example - ecs
 *
 *   Example licensed under an unmodified MIT license.
 *
 *   Copyright (c) 2026 Leandro Peres and contributors
 *
 ********************************************************************************************/

#include "engine/ecs.hpp"

#include <cstdlib>
#include <vector>

// Components
// Data structs attached to entities, they hold no logic
//----------------------------------------------------------
struct Transform
{
    double x { 0.0 }; // Horizontal position
    double y { 0.0 }; // Vertical position
};

struct RigidBody
{
    double vx { 0.0 }; // Horizontal velocity
    double vy { 0.0 }; // Vertical velocity
};

// Systems
// Logic that runs over the entities holding the required components
//----------------------------------------------------------
class MovementSystem : public en::System
{
  public:
    MovementSystem()
    {
        // Entities handled by this system must hold these components
        RequireComponent< Transform >();
        RequireComponent< RigidBody >();
    }

    void
    Update( double deltaTime, en::Registry & registry )
    {
        for( const en::Entity & entity : GetSystemEntities() )
            {
                auto & transform       = registry.GetComponent< Transform >( entity );
                const auto & rigidBody = registry.GetComponent< RigidBody >( entity );

                // Integrate velocity into position
                transform.x += rigidBody.vx * deltaTime;
                transform.y += rigidBody.vy * deltaTime;

                EN_LOG_INFO( "Entity id '{}' moved to ({}, {})", entity.GetId(), transform.x, transform.y );
            }
    }
};

int
main( int argc, char * argv[] )
{
    // Registry and System Creation
    //----------------------------------------------------------
    en::Registry registry {};
    MovementSystem movementSystem {};

    // Entity Creation
    //----------------------------------------------------------
    // Each entity holds a Transform and a RigidBody to be moved by the MovementSystem
    std::vector< en::Entity > entities {};

    constexpr int numEntities = 3;
    for( int i = 0; i < numEntities; ++i )
        {
            en::Entity entity = registry.CreateEntity();
            entities.push_back( entity );

            const double offsetX = 10.0 * static_cast< double >( i );
            registry.AddComponent< Transform >( entity, offsetX, 0.0 );
            registry.AddComponent< RigidBody >( entity, 30.0 + offsetX, 15.0 - offsetX );

            // Entities are added to systems explicitly
            movementSystem.AddEntityToSystem( entity );

            EN_LOG_INFO( "Entity id '{}' ( {} )", entity.GetId(), registry.HasComponent< Transform >( entity ) );
        }

    // Simulation Loop
    //----------------------------------------------------------
    constexpr double deltaTime = 1.0 / 60.0;
    constexpr int numFrames    = 5;

    for( int frame = 0; frame < numFrames; ++frame )
        {
            EN_LOG_INFO( "Frame '{}'", frame );
            movementSystem.Update( deltaTime, registry );
        }

    // Entity Killing / Lifecycle
    //----------------------------------------------------------
    registry.KillEntity( entities[1] );
    registry.Update();

    EN_LOG_INFO( "Simulation finished" );

    return EXIT_SUCCESS;
}
