/********************************* LOG ***********************************
 * log: Logging system
  *************************************************************************/

#include "engine/log.hpp"

#include <chrono>   /* std::chrono::system_clock */
#include <ctime>    /* std::time_t, std::strftime, std::localtime */
#include <iostream> /* std::ostream, std::cout, std::cerr, std::endl */
#include <utility>  /* std::move */

namespace en
{
    //------------------------------------------------------------------------------------------------------------------
    // Module Functions Definition: Helpers (internal linkage)
    //------------------------------------------------------------------------------------------------------------------
    namespace
    {
        // Get the current local time as "dd-Mon-yyyy HH:MM:SS"
        std::string
        CurrentDateTimeToString()
        {
            std::time_t now = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
            std::string output( 30, '\0' );
            // Trim the buffer to the written size, or the message would carry trailing NULs
            output.resize( std::strftime( output.data(), output.size(), "%d-%b-%Y %H:%M:%S", std::localtime( &now ) ) );
            return output;
        }

        // Print the entry to the given stream and keep it in Logger::messages
        void
        Write( LogType type, const char * tag, std::ostream & out, const std::string & message )
        {
            LogEntry entry { type, std::string( tag ) + ": [" + CurrentDateTimeToString() + "]: " + message };
            out << entry.message << std::endl;
            Logger::messages.push_back( std::move( entry ) );
        }
    } // namespace

    //------------------------------------------------------------------------------------------------------------------
    // Module Functions Definition: Logger
    //------------------------------------------------------------------------------------------------------------------
    // Display an info message
    void
    Logger::Log( const std::string & message )
    {
        Write( LogType::Info, "LOG", std::cout, message );
    }

    // Display a warning message
    void
    Logger::Warn( const std::string & message )
    {
        Write( LogType::Warning, "WRN", std::cout, message );
    }

    // Display an error message, goes to stderr
    void
    Logger::Err( const std::string & message )
    {
        Write( LogType::Error, "ERR", std::cerr, message );
    }
} // namespace en
