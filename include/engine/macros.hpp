/******************************** MACROS *********************************
 * macros: Compiler attribute macros
 *
 *************************************************************************/

#ifndef ENGINE_MACROS_HPP
#define ENGINE_MACROS_HPP

//==============================================================================================================
// INCLUDES
//==============================================================================================================

#include <cassert> /* assert */

//==============================================================================================================
// DEFINES
//==============================================================================================================

// Compiler-specific attributes and hints
#if defined( _MSC_VER )
#    define EN_FORCEINLINE   __forceinline
#    define EN_NORETURN      __declspec( noreturn )
#    define EN_ALIGNED( x )  __declspec( align( x ) )
#    define EN_PACKED        __pragma( pack( push, 1 ) ) // NOTE: requires a matching EN_UNPACK
#    define EN_UNPACK        __pragma( pack( pop ) )
#    define EN_RESTRICT      __restrict
#    define EN_DEPRECATED    __declspec( deprecated )
#    define EN_LIKELY( x )   ( x )
#    define EN_UNLIKELY( x ) ( x )
#elif defined( __GNUC__ ) || defined( __clang__ )
#    define EN_FORCEINLINE  inline __attribute__( ( always_inline ) )
#    define EN_NORETURN     __attribute__( ( noreturn ) )
#    define EN_ALIGNED( x ) __attribute__( ( aligned( x ) ) ) // Align variables/structs
#    define EN_PACKED       __attribute__( ( packed ) )
#    define EN_UNPACK
#    define EN_RESTRICT      __restrict__                     // Non-aliasing pointer hint
#    define EN_DEPRECATED    __attribute__( ( deprecated ) )  // Mark as deprecated
#    define EN_LIKELY( x )   __builtin_expect( !!( x ), 1 )   // Branch prediction hint
#    define EN_UNLIKELY( x ) __builtin_expect( !!( x ), 0 )
#else
#    define EN_FORCEINLINE inline
#    define EN_NORETURN
#    define EN_ALIGNED( x )
#    define EN_PACKED
#    define EN_UNPACK
#    define EN_RESTRICT
#    define EN_DEPRECATED
#    define EN_LIKELY( x )   ( x )
#    define EN_UNLIKELY( x ) ( x )
#endif

// Unused return value warning
#define EN_NODISCARD                        [[nodiscard]]

// Unused parameter suppression
#define EN_UNUSED( x )                      (void)( x )

// Bitmask checks
// Check if all bits set in required are also set in flags
#define EN_FLAGS_CONTAIN( flags, required ) ( ( ( flags ) & ( required ) ) == ( required ) )

// Assertion macros
#if defined( ASSERT_SUPPORT )
#    define EN_ASSERT( ... ) assert( __VA_ARGS__ )
#else  // !ASSERT_SUPPORT
#    define EN_ASSERT( ... ) ( (void)0 )
#endif // ASSERT_SUPPORT

#endif // !ENGINE_MACROS_HPP
