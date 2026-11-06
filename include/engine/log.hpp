/********************************* LOG ***********************************
 * log: Logging system
 *
 *                                NOTES
 * ------------------------------------------------------------------------
 * INFO:
 * - DEFINES:
 *   - LOG_SUPPORT: Enable Logging system
 *
 * TODO:
 *   - Not thread-safe: Logger::messages and std::localtime are shared state.
 *   - Logger::messages grows without limit.
 *
 *************************************************************************/

#ifndef ENGINE_LOG_HPP
#define ENGINE_LOG_HPP

//==============================================================================================================
// INCLUDES
//==============================================================================================================
#include "engine/api.hpp"

#include <format> /* std::format */ // IWYU pragma: keep
#include <string>                   /* std::string */
#include <vector>                   /* std::vector */

//==============================================================================================================
// DEFINES
//==============================================================================================================

// Logging macros
#if defined( LOG_SUPPORT )
#    define EN_LOG_INFO( ... )  ::en::Logger::Log( std::format( __VA_ARGS__ ) )
#    define EN_LOG_WARN( ... )  ::en::Logger::Warn( std::format( __VA_ARGS__ ) )
#    define EN_LOG_ERROR( ... ) ::en::Logger::Err( std::format( __VA_ARGS__ ) )
#else // !LOG_SUPPORT
#    define EN_LOG_INFO( ... )  ( (void)0 )
#    define EN_LOG_WARN( ... )  ( (void)0 )
#    define EN_LOG_ERROR( ... ) ( (void)0 )
#endif // LOG_SUPPORT

namespace en
{
    //==========================================================================================================
    // ENUMERATORS
    //==========================================================================================================

    // Log types
    enum class LogType
    {
        Info,    // Info logging, used for program execution info
        Warning, // Warning logging, used on recoverable failures
        Error    // Error logging, used on unrecoverable failures
    };

    //==========================================================================================================
    // STRUCTS
    //==========================================================================================================

    // Log entry
    struct LogEntry
    {
        LogType     type;    // Severity of the entry
        std::string message; // Formatted text, tag and timestamp included
    };

    //==========================================================================================================
    // FUNCTIONS DECLARATIONS
    //==========================================================================================================

    // Logger
    class EN_API Logger
    {
      public:
        static inline std::vector< LogEntry > messages {}; // Every message logged so far, oldest first

        static void Log( const std::string & message );    // Display an info message
        static void Warn( const std::string & message );   // Display a warning message
        static void Err( const std::string & message );    // Display an error message, goes to stderr
    };
} // namespace en

#endif // !ENGINE_LOG_HPP
