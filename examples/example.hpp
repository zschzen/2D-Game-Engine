/*************************** EXAMPLE APP ********************************
 * example_app: SDL window, renderer and main loop boilerplate for examples
 *
 *************************************************************************/

#ifndef EXAMPLE_HPP
#define EXAMPLE_HPP

//==============================================================================================================
// INCLUDES
//==============================================================================================================
#include <SDL.h>

#ifdef __EMSCRIPTEN__
#    include <emscripten.h>
#endif

#include <cstdlib>     /* EXIT_SUCCESS, EXIT_FAILURE */
#include <memory>      /* std::unique_ptr */
#include <string_view> /* std::string_view */

#include "engine/log.hpp"

// Application boilerplate
// Every example derives from it and fills the On... hooks
//----------------------------------------------------------
class ExampleApp
{
  public:
    ExampleApp( std::string_view title, int width = 640, int height = 480 );
    virtual ~ExampleApp() = default;

    // Runs the SDL initialization, the main loop and the cleanup, returns the process code
    int Run();

  protected:
    // Called once, right after the window and the renderer are ready
    virtual void
    OnStart()
    {
    }

    // Called for every event polled during the frame
    virtual void
    OnProcessInput( const SDL_Event & event )
    {
    }

    // Called every frame with the time in seconds elapsed since the last frame
    virtual void
    OnUpdate( double deltaTime )
    {
    }

    // Called every frame after the frame is cleared, drawing happens on the given renderer
    virtual void
    OnRender( SDL_Renderer & renderer )
    {
    }

    // Called once, right before the window and the renderer are destroyed
    virtual void
    OnEnd()
    {
    }

    // Closes the main loop
    void
    Stop()
    {
        m_running = false;
    }

    // Borrows the window handle, handed to the example during setup
    SDL_Window *
    GetWindow() const
    {
        return m_window;
    }

    // Borrows the renderer handle, handed to the example during setup
    SDL_Renderer *
    GetRenderer() const
    {
        return m_renderer;
    }

  private:
    // Initializes SDL and creates the window and the renderer, returns false on failure
    bool Initialize();

    // Destroys the window and the renderer, quits SDL
    void Destroy();

    // Polls and dispatches the window events of the frame
    void ProcessInput();

    // Computes the frame time and runs the update hook
    void Update();

    // Clears the frame with the clear color, runs the render hook and presents the frame
    void Render();

#ifdef __EMSCRIPTEN__
    // Runs one frame per browser callback, exits the runtime when the loop closes
    static void Iterate( void * application );

    // Tears down and releases the application through the runtime exit path
    static void ExitCleanup();

    // Application waiting for the runtime exit
    inline static ExampleApp * s_active {};
#endif

    std::string_view m_title {};                // Window title
    int m_width {};                             // Window width
    int m_height {};                            // Window height
    SDL_Window * m_window {};                   // Window handle
    SDL_Renderer * m_renderer {};               // Renderer handle
    SDL_Color m_clearColor { 45, 45, 45, 255 }; // Color the frame is cleared with
    Uint32 m_previousTicks {};                  // Ticks of the previous frame
    bool m_running { true };                    // Main loop flag
};

// Implemented by every example, instantiates its concrete application
//----------------------------------------------------------
std::unique_ptr< ExampleApp > CreateExample();

inline ExampleApp::ExampleApp( std::string_view title, int width, int height )
    : m_title( title )
    , m_width( width )
    , m_height( height )
{
}

// Runs the SDL initialization, the main loop and the cleanup, returns the process code
inline int
ExampleApp::Run()
{
    // SDL Initialization
    //----------------------------------------------------------
    if( !Initialize() )
        {
            Destroy();

            return EXIT_FAILURE;
        }

    // Example Setup
    //----------------------------------------------------------
    OnStart();

#ifdef __EMSCRIPTEN__
    // Browser Main Loop
    //----------------------------------------------------------
    // The browser owns the loop, each callback runs one frame of the simulation
    // Ownership moves to the exit path, the atexit release covers what the unwind skips
    ExampleApp::s_active = this;
    atexit( &ExampleApp::ExitCleanup );

    emscripten_set_main_loop_arg( ExampleApp::Iterate, this, 0, 1 );

    return EXIT_SUCCESS;
#else
    // Main Loop
    //----------------------------------------------------------
    while( m_running )
        {
            ProcessInput();
            Update();
            Render();
        }

    // Shutdown and Resource Cleanup
    //----------------------------------------------------------
    // The example releases what it created during setup
    OnEnd();

    Destroy();
#endif

    return EXIT_SUCCESS;
}

#ifdef __EMSCRIPTEN__
// Runs one frame per browser callback, exits the runtime when the loop closes
inline void
ExampleApp::Iterate( void * application )
{
    ExampleApp & app = *static_cast< ExampleApp * >( application );

    if( app.m_running )
        {
            app.ProcessInput();
            app.Update();
            app.Render();

            return;
        }

    // Loop closed, the runtime exit runs the teardown and releases the application
    emscripten_cancel_main_loop();

    emscripten_force_exit( EXIT_SUCCESS );
}

// Tears down and releases the application through the runtime exit path
inline void
ExampleApp::ExitCleanup()
{
    ExampleApp * app = s_active;
    s_active         = nullptr;

    if( app )
        {
            // The example releases what it created during setup, then the window goes down
            app->OnEnd();
            app->Destroy();

            delete app;
        }
}
#endif

// Initializes SDL and creates the window and the renderer, returns false on failure
inline bool
ExampleApp::Initialize()
{
    // SDL Initialization
    //----------------------------------------------------------
    if( SDL_Init( SDL_INIT_VIDEO ) != 0 )
        {
            EN_LOG_ERROR( "SDL_Init failed: {}", SDL_GetError() );
            return false;
        }

    // Window Creation
    //----------------------------------------------------------
    m_window = SDL_CreateWindow( m_title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height,
                                 SDL_WINDOW_SHOWN );

    if( !m_window )
        {
            EN_LOG_ERROR( "SDL_CreateWindow failed: {}", SDL_GetError() );
            return false;
        }

    // Renderer Creation
    //----------------------------------------------------------
    // Prefer an accelerated vsync renderer
    m_renderer = SDL_CreateRenderer( m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

    if( !m_renderer )
        {
            m_renderer = SDL_CreateRenderer( m_window, -1, 0 );
        }

    if( !m_renderer )
        {
            EN_LOG_ERROR( "SDL_CreateRenderer failed: {}", SDL_GetError() );
            return false;
        }

    m_previousTicks = SDL_GetTicks();

    return true;
}

// Destroys the window and the renderer, quits SDL
inline void
ExampleApp::Destroy()
{
    EN_LOG_INFO( "Shutting down" );

    SDL_DestroyRenderer( m_renderer );
    SDL_DestroyWindow( m_window );
    SDL_Quit();
}

// Polls and dispatches the window events of the frame
inline void
ExampleApp::ProcessInput()
{
    // Event Handling
    //----------------------------------------------------------
    SDL_Event event;
    while( SDL_PollEvent( &event ) )
        {
            OnProcessInput( event );

            switch( event.type )
                {
                case SDL_QUIT: Stop(); break;

                case SDL_KEYDOWN:
                    // A single ESC press closes the app, OS repeats of a held key are ignored
                    if( event.key.repeat == 0 && event.key.keysym.sym == SDLK_ESCAPE )
                        {
                            Stop();
                        }
                    break;

                default: break;
                }
        }
}

// Computes the frame time and runs the update hook
inline void
ExampleApp::Update()
{
    // Frame Time
    //----------------------------------------------------------
    // The difference in ticks since the last frame, converted to seconds
    const double deltaTime = ( SDL_GetTicks() - m_previousTicks ) / 1000.0;
    m_previousTicks        = SDL_GetTicks();

    OnUpdate( deltaTime );
}

// Clears the frame with the clear color, runs the render hook and presents the frame
inline void
ExampleApp::Render()
{
    // Frame Rendering and Presenting
    //----------------------------------------------------------
    SDL_SetRenderDrawColor( m_renderer, m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a );
    SDL_RenderClear( m_renderer );

    OnRender( *m_renderer );

    SDL_RenderPresent( m_renderer );
}

#endif // !EXAMPLE_HPP
