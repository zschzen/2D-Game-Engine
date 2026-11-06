#ifndef ENGINE_API_HPP
#define ENGINE_API_HPP

#if !defined( EN_SHARED )
#    define EN_API
#else
#    if defined( _WIN32 ) || defined( __CYGWIN__ )
#        ifdef EN_EXPORTS
#            ifdef __GNUC__
#                define EN_API __attribute__( ( dllexport ) )
#            else
#                define EN_API __declspec( dllexport )
#            endif
#        else
#            ifdef __GNUC__
#                define EN_API __attribute__( ( dllimport ) )
#            else
#                define EN_API __declspec( dllimport )
#            endif
#        endif
#    else
#        if __GNUC__ >= 4
#            define EN_API __attribute__( ( visibility( "default" ) ) )
#        else
#            define EN_API
#        endif
#    endif
#endif

#endif // !ENGINE_API_HPP
