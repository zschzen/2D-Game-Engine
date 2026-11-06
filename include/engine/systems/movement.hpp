/******************************* MOVEMENT ********************************
 * movement: Moves entities based on their velocity
 *
 *************************************************************************/

#ifndef ENGINE_SYSTEMS_MOVEMENT_HPP
#define ENGINE_SYSTEMS_MOVEMENT_HPP

//==============================================================================================================
// INCLUDES
//==============================================================================================================
#include "engine/ecs.hpp"

namespace en
{
    //==========================================================================================================
    // FUNCTIONS DECLARATIONS
    //==========================================================================================================

    // Movement system
    class MovementSystem : public System
    {
      public:
        MovementSystem()
        {
            // TODO:
            // RequireComponent<TransformComponent>();
            // RequireComponent<...>();
        }

        // Update the position of every entity the system is interested in
        void
        Update()
        {
            // TODO:
            // Loop all entities that the system is interested in
            // for (auto entity: GetEntities()) {
            // TODO: Update entity position based on its velocity
            // }
        }
    };
} // namespace en

#endif // !ENGINE_SYSTEMS_MOVEMENT_HPP
