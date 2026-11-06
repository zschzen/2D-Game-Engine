/***************************** TRANSFORM *********************************
 * transform: Position, scale and rotation component
 *
 *************************************************************************/

#ifndef ENGINE_COMPONENTS_TRANSFORM_HPP
#define ENGINE_COMPONENTS_TRANSFORM_HPP

//==============================================================================================================
// INCLUDES
//==============================================================================================================
#include <cglm/cglm.h> /* vec2 */

namespace en
{
    //==========================================================================================================
    // STRUCTS
    //==========================================================================================================

    // Transform component
    struct TransformComponent
    {
        vec2   position; // Position of the entity
        vec2   scale;    // Scale factor on each axis
        double rotation; // Rotation angle
    };
} // namespace en

#endif // !ENGINE_COMPONENTS_TRANSFORM_HPP
