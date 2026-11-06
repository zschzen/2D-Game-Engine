/******************************** MACROS *********************************
 * macros: Compiler attribute macros
 *
 *************************************************************************/

#ifndef ENGINE_MACROS_HPP
#define ENGINE_MACROS_HPP

//==============================================================================================================
// DEFINES
//==============================================================================================================

// Force the compiler to inline, the definition must be visible in the header
// Prefixed with EN_ since <windows.h> defines FORCEINLINE
#if defined( _MSC_VER )
#    define EN_FORCEINLINE __forceinline
#elif defined( __GNUC__ ) || defined( __clang__ )
#    define EN_FORCEINLINE inline __attribute__( ( always_inline ) )
#else
#    define EN_FORCEINLINE inline
#endif

#endif // !ENGINE_MACROS_HPP
