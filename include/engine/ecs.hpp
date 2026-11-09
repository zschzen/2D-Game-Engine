/********************************* ECS ***********************************
 * ecs: Entity Component System
 *
 *                                NOTES
 * ------------------------------------------------------------------------
 * INFO:
 * - DEFINES:
 *
 * TODO:
 *   - Component and Registry are empty placeholders.
 *   - System methods are declared, not defined yet.
 *
 *************************************************************************/

#ifndef ENGINE_ECS_HPP
#define ENGINE_ECS_HPP

//==============================================================================================================
// INCLUDES
//==============================================================================================================
#include "engine/api.hpp"
#include "engine/macros.hpp"

#include <bitset>  /* std::bitset */
#include <cstddef> /* std::size_t */
#include <vector>  /* std::vector */

//==============================================================================================================
// DEFINES
//==============================================================================================================

namespace en
{
    //==========================================================================================================
    // CONSTANTS
    //==========================================================================================================

    // Max number of component types
    constexpr unsigned int MAX_COMPONENTS = 32;

    //==========================================================================================================
    // TYPEDEFS
    //==========================================================================================================

    // Bitmask of component types, one bit per type
    typedef std::bitset< MAX_COMPONENTS > Signature;

    //==========================================================================================================
    // ENUMERATORS
    //==========================================================================================================

    //==========================================================================================================
    // STRUCTS
    //==========================================================================================================

    // Shared counter that hands out component type ids
    struct EN_API IComponent
    {
      protected:
        static unsigned int nextId; // Next unused component type id
    };

    //==========================================================================================================
    // FUNCTIONS DECLARATIONS
    //==========================================================================================================

    //--- COMPONENT --------------------------------------------------------------------------------------------

    // Data attached to an entity, holds no logic
    template < typename T > class Component : public IComponent
    {
      public:
        // Get the unique id of Component<T>, assigned on first use
        static unsigned int GetId();
    };

    //--- ENTITY -----------------------------------------------------------------------------------------------

    // Identifier that groups components together
    class EN_API Entity
    {
      public:
        // Create an entity
        Entity( int id )
            : m_id( id )
        {
        }

        // Get the entity identifier
        EN_FORCEINLINE int GetId() const;

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
        unsigned int m_id { 0 }; // Entity identifier
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
        const std::vector< Entity > & GetSystemEntities() const;

        // Defines the component type that entities must have to be considered by the system
        template < typename TComponent > void RequireComponent();

        // Get the component types an entity needs to be handled by the system
        const Signature & GetComponentSignature() const;

      private:
        Signature             m_componentSignature {}; // Component types an entity needs to be handled by the system
        std::vector< Entity > m_entities {};           // Entities that match the component signature
    };

    //--- POOL -------------------------------------------------------------------------------------------------

    //
    class IPool
    {
      public:
        virtual ~IPool()                                  = default;
        virtual void RemoveEntityFromPool( int entityId ) = 0;
    };

    // Owns the objects of one component type, indexed by entity id
    template < typename T > class Pool : public IPool
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
        EN_FORCEINLINE void Resize( std::size_t newSize );
        // Remove all objects from the pool
        EN_FORCEINLINE void Clear();
        // Add an object to the end of the pool
        EN_FORCEINLINE void Add( T object );
        // Overwrite the object at the given index
        EN_FORCEINLINE void Set( std::size_t index, T object );

        // --- Element Access --------------------------------------------------
        // Get the object at the given index
        EN_FORCEINLINE T & Get( std::size_t index );
        // Get the object at the given index
        EN_FORCEINLINE T & operator[]( std::size_t index );

      private:
        std::vector< T > m_data {};    // Objects stored in the pool, indexed by entity id
        std::size_t      m_size { 0 }; // Number of objects currently stored in the pool
    };

    //--- REGISTRY ---------------------------------------------------------------------------------------------

    // Owns and creates the entities, components and systems
    class EN_API Registry
    {
      public:
        Registry()  = default;
        ~Registry() = default;

        // Entity CreateEntity();
        // void   KillEntity( Entity entity );
        //void AddSystem(...);

      private:
        std::size_t m_numEntities { 0 }; // Total amount of entities was added to the scene
    };

    //==========================================================================================================
    // FUNCTIONS DEFINITIONS
    //==========================================================================================================

    //--- COMPONENT --------------------------------------------------------------------------------------------

    // Get the unique id of Component<T>, assigned on first use
    template < typename T >
    unsigned int
    Component< T >::GetId()
    {
        static const unsigned int id = nextId++;
        return id;
    }

    //--- ENTITY -----------------------------------------------------------------------------------------------

    // Get the entity identifier
    EN_FORCEINLINE int
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
    template < typename TComponent >
    void
    System::RequireComponent()
    {
        m_componentSignature.set( Component< TComponent >::GetId() );
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
    template < typename T >
    EN_FORCEINLINE bool
    Pool< T >::IsEmpty() const
    {
        return m_data.empty();
    }

    // Get the number of objects stored in the pool
    template < typename T >
    EN_FORCEINLINE std::size_t
                   Pool< T >::GetSize() const
    {
        return m_data.size();
    }

    // Resize the pool to hold the given number of objects
    template < typename T >
    EN_FORCEINLINE void
    Pool< T >::Resize( std::size_t newSize )
    {
        m_data.resize( newSize );
    }

    // Remove all objects from the pool
    template < typename T >
    EN_FORCEINLINE void
    Pool< T >::Clear()
    {
        m_data.clear();
    }

    // Add an object to the end of the pool
    template < typename T >
    EN_FORCEINLINE void
    Pool< T >::Add( T object )
    {
        m_data.push_back( object );
    }

    // Overwrite the object at the given index
    template < typename T >
    EN_FORCEINLINE void
    Pool< T >::Set( std::size_t index, T object )
    {
        m_data[index] = object;
    }

    // Get the object at the given index
    template < typename T >
    EN_FORCEINLINE T &
    Pool< T >::Get( std::size_t index )
    {
        return static_cast< T & >( m_data[index] );
    }

    // Get the object at the given index
    template < typename T >
    EN_FORCEINLINE T &
    Pool< T >::operator[]( std::size_t index )
    {
        return m_data[index];
    }
} // namespace en

#endif // !ENGINE_ECS_HPP
