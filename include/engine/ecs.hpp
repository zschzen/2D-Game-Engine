/********************************* ECS ***********************************
 * ecs: Entity Component System
 *
 *                                   NOTES
 * ------------------------------------------------------------------------
 * INFO:
 * - DEFINES:
 *
 *************************************************************************/

#ifndef ENGINE_ECS_HPP
#define ENGINE_ECS_HPP

//==============================================================================================================
// INCLUDES
//==============================================================================================================
#include "engine/api.hpp"
#include "engine/log.hpp"
#include "engine/macros.hpp"

#include <bitset>        /* std::bitset */
#include <concepts>      /* std::semiregular, std::derived_from, std::constructible_from */
#include <cstddef>       /* std::size_t */
#include <cstdint>       /* std::uint32_t */
#include <limits>        /* std::numeric_limits */
#include <memory>        /* std::unique_ptr, std::make_unique */
#include <set>           /* std::set */
#include <type_traits>   /* std::is_const_v, std::is_pointer_v, std::is_volatile_v */
#include <typeindex>     /* std::type_index */
#include <unordered_map> /* std::unordered_map */
#include <utility>       /* std::forward, std::move */
#include <vector>        /* std::vector */

//==============================================================================================================
// DEFINES
//==============================================================================================================

namespace en
{
    //==========================================================================================================
    // CONSTANTS
    //==========================================================================================================

    // Max number of component types
    constexpr std::size_t MAX_COMPONENTS = 32;

    //==========================================================================================================
    // TYPEDEFS
    //==========================================================================================================

    // Unique identifier of an entity
    typedef std::uint32_t EntityId;

    // Unique identifier of a component type
    typedef std::uint32_t ComponentId;

    // Sentinel that refers to no entity
    constexpr EntityId INVALID_ENTITY_ID { std::numeric_limits< EntityId >::max() };

    // Bitmask of component types, one bit per type
    typedef std::bitset< MAX_COMPONENTS > Signature;

    //==========================================================================================================
    // CONCEPTS
    //==========================================================================================================

    class System;

    // Concept for types that can be stored in a component pool
    template < typename T >
    concept ComponentType = std::semiregular< T >        // Pool copies values and default-constructs them
                            && !std::is_pointer_v< T >   // Pool owns the component, not the pointee
                            && !std::is_volatile_v< T >; // std::vector cannot hold volatile objects

    // Concept for types the registry owns and retrieves as borrowed System & views
    template < typename T >
    concept SystemType = std::derived_from< T, System > // Public, unambiguous inheritance: T* converts to System*
                         && !std::is_const_v< T >       // const T* does not convert to System*
                         && !std::is_volatile_v< T >;   // volatile T* does not convert to System*

    //==========================================================================================================
    // STRUCTS
    //==========================================================================================================

    // Shared counter that hands out component type ids
    struct EN_API IComponent
    {
      protected:
        static ComponentId nextId; // Next unused component type id
    };

    //==========================================================================================================
    // FUNCTIONS DECLARATIONS
    //==========================================================================================================

    //--- COMPONENT --------------------------------------------------------------------------------------------

    // Data attached to an entity, holds no logic
    template < ComponentType T > class Component : public IComponent
    {
      public:
        // Get the unique id of Component<T>, assigned on first use
        static ComponentId GetId();
    };

    //--- ENTITY -----------------------------------------------------------------------------------------------

    // Identifier that groups components together
    class EN_API Entity
    {
      public:
        // Create an entity
        Entity( EntityId id )
            : m_id( id )
        {
        }

        // Get the entity identifier
        EN_FORCEINLINE EntityId GetId() const;

        // Operator overloading for entity objects
        Entity & operator=( const Entity & other ) = default;

        // Check if two entities are equal
        EN_FORCEINLINE bool operator==( const Entity & other ) const;

        // Check if two entities are not equal
        EN_FORCEINLINE bool operator!=( const Entity & other ) const;

        // Check if this entity compares greater than another
        EN_FORCEINLINE bool operator>( const Entity & other ) const;

        // Check if this entity compares less than another
        EN_FORCEINLINE bool operator<( const Entity & other ) const;

      private:
        EntityId m_id { INVALID_ENTITY_ID }; // Entity identifier
    };

    //--- SYSTEM -----------------------------------------------------------------------------------------------

    // Logic that runs over the entities holding a given set of components
    class EN_API System
    {
      public:
        // Default construction and destruction
        System()  = default;
        ~System() = default;

        // Add the entity to the system
        void AddEntityToSystem( Entity entity );
        // Remove the entity from the system
        void RemoveEntityFromSystem( Entity entity );

        // Get the entities handled by the system
        EN_FORCEINLINE const std::vector< Entity > & GetSystemEntities() const;

        // Defines the component type that entities must have to be considered by the system
        template < ComponentType TComponent > void RequireComponent();

        // Get the component types an entity needs to be handled by the system
        EN_FORCEINLINE const Signature & GetComponentSignature() const;

      private:
        Signature m_componentSignature {};   // Component types an entity needs to be handled by the system
        std::vector< Entity > m_entities {}; // Entities that match the component signature
    };

    //--- POOL -------------------------------------------------------------------------------------------------

    // Base interface for component pools
    class IPool
    {
      public:
        virtual ~IPool()                         = default;
        virtual void Remove( EntityId entityId ) = 0;
    };

    // Owns the objects of one component type, indexed by entity id
    template < ComponentType T > class Pool : public IPool
    {
      public:
        Pool( std::size_t capacity = 100u )
            : m_data( capacity )
            , m_size( 0u )
        {
        }

        virtual ~Pool() = default;

        // --- Capacity --------------------------------------------------------
        // Check if the pool holds no objects
        EN_FORCEINLINE bool IsEmpty() const;
        // Get the number of objects stored in the pool
        EN_FORCEINLINE std::size_t GetSize() const;

        // --- Modifiers -------------------------------------------------------
        // Resize the pool to hold the given number of objects
        void Resize( std::size_t newSize );
        // Remove all objects from the pool
        void Clear();
        // Add an object to the end of the pool
        void Add( T object );
        // Overwrite the object at the given index
        void Set( std::size_t index, T object );

        // --- Element Access --------------------------------------------------
        // Get the object at the given index
        EN_FORCEINLINE T & Get( std::size_t index );
        // Get the object at the given index
        EN_FORCEINLINE T & operator[]( std::size_t index );

        // Implementation for IPool interface method
        virtual void
        Remove( EntityId entityId ) override
        {
            if( entityId < m_data.size() )
                {
                    // The slot dies with its entity, a default value avoids leaking the old owner's data
                    m_data[entityId] = T {};
                }
        }

      private:
        std::vector< T > m_data {}; // Objects stored in the pool, indexed by entity id
        std::size_t m_size { 0 };   // Number of objects currently stored in the pool
    };

    //--- REGISTRY ---------------------------------------------------------------------------------------------

    // Owns and creates the entities, components and systems
    class EN_API Registry
    {
      public:
        // --- Constructors ----------------------------------------------------
        Registry()  = default;
        ~Registry() = default;

        // Process the entities queued for addition and removal
        void Update();

        // --- Entities --------------------------------------------------------
        // Signals to create an entity
        Entity CreateEntity();
        // Signals to remove an entity
        void KillEntity( Entity entity );

        // Returns if the given entity is alive
        bool IsEntityAlive( Entity entity ) const;

        // --- Components ------------------------------------------------------
        // Attach a component to the given entity
        template < typename TComponent, typename... TArgs >
        requires ComponentType< TComponent > && std::constructible_from< TComponent, TArgs &&... >
        void AddComponent( Entity entity, TArgs &&... args );

        // Detach a component from the given entity
        template < typename TComponent >
        requires ComponentType< TComponent >
        void RemoveComponent( Entity entity );

        // Returns if the given entity has the component
        template < typename TComponent >
        requires ComponentType< TComponent >
        bool HasComponent( Entity entity ) const;

        // Returns the requested component from the given entity
        template < typename TComponent >
        requires ComponentType< TComponent >
        TComponent & GetComponent( Entity entity ) const;

        // --- Systems ---------------------------------------------------------
        // Add a system of the given type, constructing it with the given arguments
        template < typename TSystem, typename... TArgs >
        requires SystemType< TSystem > && std::constructible_from< TSystem, TArgs &&... >
        void AddSystem( TArgs &&... args );

        // Remove the system of the given type
        template < typename TSystem >
        requires SystemType< TSystem >
        void RemoveSystem();

        // Returns if the given system type is registered
        template < typename TSystem >
        requires SystemType< TSystem >
        bool HasSystem() const;

        // Returns the requested system from the registry
        // Throws std::out_of_range if it is not registered
        template < typename TSystem >
        requires SystemType< TSystem >
        TSystem & GetSystem();

        // Returns the requested system from the registry, read-only
        // Throws std::out_of_range if it is not registered
        template < typename TSystem >
        requires SystemType< TSystem >
        const TSystem & GetSystem() const;

        // Adds the entity to all systems that match its component signature
        void AddEntityToSystems( Entity entity );

        // Removes the entity from all systems that hold it
        void RemoveEntityFromSystems( Entity entity );

      private:
        // --- Component Pool Helpers ------------------------------------------
        // Get the pool for the given type, or nullptr if it doesn't exist
        template < typename TComponent >
        requires ComponentType< TComponent >
        Pool< TComponent > * GetComponentPool() const;

        // Create the pool for the given type
        template < typename TComponent >
        requires ComponentType< TComponent >
        Pool< TComponent > & CreateComponentPool();

        // Get the pool for the given type, creating it if it doesn't exist
        template < typename TComponent >
        requires ComponentType< TComponent >
        Pool< TComponent > & GetOrCreateComponentPool();

        // --- System Helpers --------------------------------------------------
        // Get the unique key of the given system type
        template < typename TSystem >
        requires SystemType< TSystem >
        EN_FORCEINLINE static std::type_index SystemKey();

        // --- Entities --------------------------------------------------------
        std::size_t m_numEntities { 0 };            // Total amount of entities was added to the scene

        std::set< Entity > m_entitiesToBeAdded {};  // Entities to add on next update
        std::set< Entity > m_entitiesToBeKilled {}; // Entities to remove on next update

        std::vector< EntityId >
            m_freeIds {}; // Ids of dead entities waiting in line to be reused (FIFO: oldest death first)
        std::vector< bool > m_entityAlive {}; // One flag per entity id, true while the id belongs to a living entity

        // --- Components ------------------------------------------------------
        std::vector< std::unique_ptr< IPool > > m_componentPools {}; // Pools indexed by component type id
        std::vector< Signature > m_entityComponentSignatures {};     // Components are enabled per entity

        // --- Systems ---------------------------------------------------------
        std::unordered_map< std::type_index, std::unique_ptr< System > > m_systems {}; // Systems indexed by their type
    };

    //==========================================================================================================
    // FUNCTIONS DEFINITIONS
    //==========================================================================================================

    //--- COMPONENT --------------------------------------------------------------------------------------------

    // Get the unique id of Component<T>, assigned on first use
    template < ComponentType T >
    ComponentId
    Component< T >::GetId()
    {
        static const ComponentId id = nextId++;
        return id;
    }

    //--- ENTITY -----------------------------------------------------------------------------------------------

    // Get the entity identifier
    EN_FORCEINLINE EntityId
    Entity::GetId() const
    {
        return m_id;
    }

    // Check if two entities are equal
    EN_FORCEINLINE bool
    Entity::operator==( const Entity & other ) const
    {
        return m_id == other.m_id;
    }

    // Check if two entities are not equal
    EN_FORCEINLINE bool
    Entity::operator!=( const Entity & other ) const
    {
        return m_id != other.m_id;
    }

    // Check if this entity compares greater than another
    EN_FORCEINLINE bool
    Entity::operator>( const Entity & other ) const
    {
        return m_id > other.m_id;
    }

    // Check if this entity compares less than another
    EN_FORCEINLINE bool
    Entity::operator<( const Entity & other ) const
    {
        return m_id < other.m_id;
    }

    //--- SYSTEM -----------------------------------------------------------------------------------------------

    // Set the component type bit in the system signature
    template < ComponentType TComponent >
    void
    System::RequireComponent()
    {
        const std::size_t componentId = static_cast< std::size_t >( Component< TComponent >::GetId() );
        m_componentSignature.set( componentId );
    }

    // Get the entities handled by the system
    EN_FORCEINLINE
    const std::vector< Entity > &
    System::GetSystemEntities() const
    {
        return m_entities;
    }

    // Get the component types an entity needs to be handled by the system
    EN_FORCEINLINE
    const Signature &
    System::GetComponentSignature() const
    {
        return m_componentSignature;
    }

    //--- POOL -------------------------------------------------------------------------------------------------

    // Check if the pool holds no objects
    template < ComponentType T >
    EN_FORCEINLINE bool
    Pool< T >::IsEmpty() const
    {
        return m_data.empty();
    }

    // Get the number of objects stored in the pool
    template < ComponentType T >
    EN_FORCEINLINE std::size_t
    Pool< T >::GetSize() const
    {
        return m_data.size();
    }

    // Resize the pool to hold the given number of objects
    template < ComponentType T >
    void
    Pool< T >::Resize( std::size_t newSize )
    {
        m_data.resize( newSize );
    }

    // Remove all objects from the pool
    template < ComponentType T >
    void
    Pool< T >::Clear()
    {
        m_data.clear();
    }

    // Add an object to the end of the pool
    template < ComponentType T >
    void
    Pool< T >::Add( T object )
    {
        m_data.push_back( object );
    }

    // Overwrite the object at the given index
    template < ComponentType T >
    void
    Pool< T >::Set( std::size_t index, T object )
    {
        if( index >= m_data.size() )
            {
                // Ensure the pool can hold the given index
                m_data.resize( index + 1 );
            }

        m_data[index] = object;
        ++m_size;
    }

    // Get the object at the given index
    template < ComponentType T >
    EN_FORCEINLINE T &
    Pool< T >::Get( std::size_t index )
    {
        return static_cast< T & >( m_data[index] );
    }

    // Get the object at the given index
    template < ComponentType T >
    EN_FORCEINLINE T &
    Pool< T >::operator[]( std::size_t index )
    {
        return m_data[index];
    }

    //--- REGISTRY ---------------------------------------------------------------------------------------------

    // Get the unique key of the given system type
    template < typename TSystem >
    requires SystemType< TSystem >
    EN_FORCEINLINE std::type_index
    Registry::SystemKey()
    {
        return std::type_index( typeid( TSystem ) );
    }

    // Add a system of the given type, constructing it with the given arguments
    template < typename TSystem, typename... TArgs >
    requires SystemType< TSystem > && std::constructible_from< TSystem, TArgs &&... >
    void
    Registry::AddSystem( TArgs &&... args )
    {
        const auto systemKey = SystemKey< TSystem >();

        EN_ASSERT( !m_systems.contains( systemKey ) && "System is already registered!" );

        m_systems.try_emplace( systemKey, std::make_unique< TSystem >( std::forward< TArgs >( args )... ) );
    }

    // Remove the system of the given type
    template < typename TSystem >
    requires SystemType< TSystem >
    void
    Registry::RemoveSystem()
    {
        m_systems.erase( SystemKey< TSystem >() );
    }

    // Returns if the given system type is registered
    template < typename TSystem >
    requires SystemType< TSystem >
    bool
    Registry::HasSystem() const
    {
        return m_systems.contains( SystemKey< TSystem >() );
    }

    // Returns the requested system from the registry
    template < typename TSystem >
    requires SystemType< TSystem >
    TSystem &
    Registry::GetSystem()
    {
        System & system = *( m_systems.at( SystemKey< TSystem >() ) );
        return static_cast< TSystem & >( system );
    }

    // Returns the requested system from the registry, read-only
    template < typename TSystem >
    requires SystemType< TSystem >
    const TSystem &
    Registry::GetSystem() const
    {
        const System & system = *( m_systems.at( SystemKey< TSystem >() ) );
        return static_cast< const TSystem & >( system );
    }

    // Get the pool for the given type, or nullptr if it doesn't exist
    template < typename TComponent >
    requires ComponentType< TComponent >
    Pool< TComponent > *
    Registry::GetComponentPool() const
    {
        const auto componentId = Component< TComponent >::GetId();

        if( componentId >= m_componentPools.size() )
            {
                return nullptr;
            }

        return static_cast< Pool< TComponent > * >( m_componentPools[componentId].get() );
    }

    // Create the pool for the given type
    template < typename TComponent >
    requires ComponentType< TComponent >
    Pool< TComponent > &
    Registry::CreateComponentPool()
    {
        const auto componentId = Component< TComponent >::GetId();

        // Ensure component pools vector has enough capacity
        //----------------------------------------------------------
        if( m_componentPools.size() <= componentId )
            {
                m_componentPools.resize( componentId + 1 );
            }

        // Initialize pool if it doesn't exist yet
        //----------------------------------------------------------
        if( !m_componentPools[componentId] )
            {
                m_componentPools[componentId] = std::make_unique< Pool< TComponent > >();
            }

        return static_cast< Pool< TComponent > & >( *m_componentPools[componentId] );
    }

    // Get the pool for the given type, creating it if it doesn't exist
    template < typename TComponent >
    requires ComponentType< TComponent >
    Pool< TComponent > &
    Registry::GetOrCreateComponentPool()
    {
        if( auto componentPool = GetComponentPool< TComponent >() )
            {
                return *componentPool;
            }

        return CreateComponentPool< TComponent >();
    }

    // Attach a component to the given entity
    template < typename TComponent, typename... TArgs >
    requires ComponentType< TComponent > && std::constructible_from< TComponent, TArgs &&... >
    void
    Registry::AddComponent( Entity entity, TArgs &&... args )
    {
        const auto componentId = Component< TComponent >::GetId();
        const auto entityId    = entity.GetId();

        // Ensure component pool
        //----------------------------------------------------------
        auto & componentPool   = GetOrCreateComponentPool< TComponent >();

        // Attach component to entity
        //----------------------------------------------------------
        // Instantiate component on the stack
        TComponent newComponent( std::forward< TArgs >( args )... );

        componentPool.Set( entityId, newComponent );              // Add component to the pool

        m_entityComponentSignatures[entityId].set( componentId ); // Enable component signature of the entity

        EN_LOG_INFO( "Component id '{}' was added to entity id '{}'", componentId, entityId );
    }

    // Detach a component from the given entity
    template < typename TComponent >
    requires ComponentType< TComponent >
    void
    Registry::RemoveComponent( Entity entity )
    {
        const auto componentId = Component< TComponent >::GetId();
        const auto entityId    = entity.GetId();

        if( auto componentPool = GetComponentPool< TComponent >() )
            {
                // TODO: componentPool->RemoveEntityFromPool( entityId );
            }

        if( entityId < m_entityComponentSignatures.size() )
            {
                m_entityComponentSignatures[entityId].set( componentId, false );
            }

        EN_LOG_INFO( "Component id '{}' was removed from entity id '{}'", componentId, entityId );
    }

    // Returns if the given entity has the component
    template < typename TComponent >
    requires ComponentType< TComponent >
    bool
    Registry::HasComponent( Entity entity ) const
    {
        const auto componentId = Component< TComponent >::GetId();
        const auto entityId    = entity.GetId();

        return ( entityId < m_entityComponentSignatures.size() )
               && m_entityComponentSignatures[entityId].test( componentId );
    }

    // Returns the requested component from the given entity
    template < typename TComponent >
    requires ComponentType< TComponent >
    TComponent &
    Registry::GetComponent( Entity entity ) const
    {
        auto componentPool = GetComponentPool< TComponent >();
        EN_ASSERT( componentPool && "Entity does not have this component pool!" );

        return componentPool->Get( entity.GetId() );
    }

} // namespace en

#endif // !ENGINE_ECS_HPP
