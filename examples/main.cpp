/*************************** EXAMPLE MAIN *****************************
 * main: entry point shared by every example, owns and runs the app
 *
 *************************************************************************/

#include "example.hpp"
#include "engine/macros.hpp"

int
main( int argc, char * argv[] )
{
    EN_UNUSED( argc );
    EN_UNUSED( argv );

    std::unique_ptr< ExampleApp > example = CreateExample();

#ifdef __EMSCRIPTEN__
    // Ownership moves to the runtime exit path, the loop unwind skips this stack frame
    ExampleApp * app = example.release();
    const int code   = app->Run();

    // Only reached when the initialization fails before the loop starts
    delete app;

    return code;
#else
    return example->Run();
#endif
}
