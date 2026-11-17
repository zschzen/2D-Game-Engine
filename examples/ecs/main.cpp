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
#include "example.hpp"

#include <vector>

// Components
// Data structs attached to entities, they hold no logic
//----------------------------------------------------------
struct TransformComponent
{
    double x { 0.0 }; // Horizontal position
    double y { 0.0 }; // Vertical position
};

struct RigidBodyComponent
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
        RequireComponent< TransformComponent >();
        RequireComponent< RigidBodyComponent >();
    }

    void
    Update( double deltaTime, en::Registry & registry )
    {
        for( const en::Entity & entity : GetSystemEntities() )
            {
                auto & transform       = registry.GetComponent< TransformComponent >( entity );
                const auto & rigidBody = registry.GetComponent< RigidBodyComponent >( entity );

                // Integrate velocity into position
                transform.x += rigidBody.vx * deltaTime;
                transform.y += rigidBody.vy * deltaTime;

                EN_LOG_INFO( "Entity id '{}' moved to ({}, {})", entity.GetId(), transform.x, transform.y );
            }
    }
};

// Example
// Windowed simulation that spawns moving entities and kills one of them mid run
//----------------------------------------------------------
class EcsExample : public ExampleApp
{
  public:
    EcsExample()
        : ExampleApp( "2D Game Engine example - ecs" )
    {
    }

  protected:
    void
    OnStart() override
    {
        // Entity Creation
        //----------------------------------------------------------
        // Each entity holds a TransformComponent and a RigidBodyComponent to be moved by the MovementSystem
        constexpr int numEntities = 3;
        for( int i = 0; i < numEntities; ++i )
            {
                en::Entity entity = registry.CreateEntity();
                entities.push_back( entity );

                const double offsetX = 10.0 * static_cast< double >( i );
                registry.AddComponent< TransformComponent >( entity, offsetX, 0.0 );
                registry.AddComponent< RigidBodyComponent >( entity, 30.0 + offsetX, 15.0 - offsetX );

                // Entities are added to systems explicitly
                movementSystem.AddEntityToSystem( entity );

                EN_LOG_INFO( "Entity id '{}' ( {} )", entity.GetId(),
                             registry.HasComponent< TransformComponent >( entity ) );
            }
    }

    void
    OnUpdate( double deltaTime ) override
    {
        // Simulation Loop
        //----------------------------------------------------------
        movementSystem.Update( deltaTime, registry );

        // Entity Killing / Lifecycle
        //----------------------------------------------------------
        constexpr int numFrames = 5;
        if( ++frame == numFrames )
            {
                registry.KillEntity( entities[1] );
                registry.Update();

                EN_LOG_INFO( "Simulation finished" );
            }
    }

    void
    OnRender( SDL_Renderer & renderer ) override
    {
        // Entity Rendering
        //----------------------------------------------------------
        // Draw a rectangle at the position of every living entity
        constexpr float entitySize = 32.0f;
        SDL_SetRenderDrawColor( &renderer, 200, 200, 200, 255 );
        for( const en::Entity & entity : movementSystem.GetSystemEntities() )
            {
                if( !registry.IsEntityAlive( entity ) ) continue;

                const auto & transform = registry.GetComponent< TransformComponent >( entity );

                const SDL_FRect rect { static_cast< float >( transform.x ), static_cast< float >( transform.y ),
                                       entitySize, entitySize };
                SDL_RenderFillRectF( &renderer, &rect );
            }
    }

  private:
    en::Registry registry {};
    MovementSystem movementSystem {};
    std::vector< en::Entity > entities {};
    int frame {};
};

// Instantiated by examples/main.cpp
//----------------------------------------------------------
std::unique_ptr< ExampleApp >
CreateExample()
{
    return std::make_unique< EcsExample >();
}
