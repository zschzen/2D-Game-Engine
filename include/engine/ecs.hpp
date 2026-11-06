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

#include <bitset> /* std::bitset */
#include <vector> /* std::vector */

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
        static unsigned int
        GetId()
        {
            static const unsigned int id = nextId++;
            return id;
        }
    };

    //--- ENTITY -----------------------------------------------------------------------------------------------

    // Identifier that groups components together
    class EN_API Entity
    {
      public:
        // Create an entity
        Entity( int id )
            : m_id( id ) {};

        // Get the entity identifier
        EN_FORCEINLINE int
        GetId() const
        {
            return m_id;
        }

        // Operator overloading for entity objects
        Entity & operator=( const Entity & other ) = default;
        bool
        operator==( const Entity & other ) const
        {
            return m_id == other.m_id;
        }
        bool
        operator!=( const Entity & other ) const
        {
            return m_id != other.m_id;
        }
        bool
        operator>( const Entity & other ) const
        {
            return m_id > other.m_id;
        }
        bool
        operator<( const Entity & other ) const
        {
            return m_id < other.m_id;
        }

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

    //--- REGISTRY ---------------------------------------------------------------------------------------------

    // Owns and creates the entities, components and systems
    class EN_API Registry
    {
    };

    //==========================================================================================================
    // FUNCTIONS DEFINITIONS
    //==========================================================================================================

    //--- SYSTEM -----------------------------------------------------------------------------------------------

    // Set the component type bit in the system signature
    template < typename TComponent >
    void
    System::RequireComponent()
    {
        m_componentSignature.set( Component< TComponent >::GetId() );
    }

    //--- SYSTEM -----------------------------------------------------------------------------------------------

    // Get the entities handled by the system
    EN_FORCEINLINE const std::vector< Entity > &
                         System::GetSystemEntities() const
    {
        return m_entities;
    }

    // Get the component types an entity needs to be handled by the system
    EN_FORCEINLINE const Signature &
    System::GetComponentSignature() const
    {
        return m_componentSignature;
    }
} // namespace en

#endif // !ENGINE_ECS_HPP
