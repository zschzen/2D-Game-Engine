#include "engine/log.hpp"

#include <chrono>
#include <ctime>
#include <iostream>
#include <utility>

namespace en
{
    namespace
    {
        std::string
        CurrentDateTimeToString()
        {
            std::time_t now = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
            std::string output( 30, '\0' );
            output.resize( std::strftime( output.data(), output.size(), "%d-%b-%Y %H:%M:%S", std::localtime( &now ) ) );
            return output;
        }

        void
        Write( LogType type, const char * tag, std::ostream & out, const std::string & message )
        {
            LogEntry entry { type, std::string( tag ) + ": [" + CurrentDateTimeToString() + "]: " + message };
            out << entry.message << std::endl;
            Logger::messages.push_back( std::move( entry ) );
        }
    } // namespace

    void
    Logger::Log( const std::string & message )
    {
        Write( LogType::Info, "LOG", std::cout, message );
    }

    void
    Logger::Warn( const std::string & message )
    {
        Write( LogType::Warning, "WRN", std::cout, message );
    }

    void
    Logger::Err( const std::string & message )
    {
        Write( LogType::Error, "ERR", std::cerr, message );
    }
} // namespace en
