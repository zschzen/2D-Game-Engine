#ifndef ENGINE_LOG_HPP
#define ENGINE_LOG_HPP

#include "engine/api.hpp"

#include <format> // IWYU pragma: keep
#include <string>
#include <vector>

#ifdef LOG_SUPPORT
#    define EN_LOG_INFO( ... )  ::en::Logger::Log( std::format( __VA_ARGS__ ) )
#    define EN_LOG_WARN( ... )  ::en::Logger::Warn( std::format( __VA_ARGS__ ) )
#    define EN_LOG_ERROR( ... ) ::en::Logger::Err( std::format( __VA_ARGS__ ) )
#else
#    define EN_LOG_INFO( ... )  ( (void)0 )
#    define EN_LOG_WARN( ... )  ( (void)0 )
#    define EN_LOG_ERROR( ... ) ( (void)0 )
#endif

namespace en
{
    enum class LogType
    {
        Info,
        Warning,
        Error
    };

    struct LogEntry
    {
        LogType     type;
        std::string message;
    };

    class EN_API Logger
    {
      public:
        static inline std::vector< LogEntry > messages {};

        static void Log( const std::string & message );
        static void Warn( const std::string & message );
        static void Err( const std::string & message );
    };
} // namespace en

#endif // !ENGINE_LOG_HPP
