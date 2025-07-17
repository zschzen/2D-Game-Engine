#include "engine/engine.hpp"
#include <cstdlib>

int
main( int argc, char * argv[] )
{
    // Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) != 0 )
        {
            return EXIT_FAILURE;
        }

    // Create window
    const int    window_width  = 640;
    const int    window_height = 480;
    SDL_Window * window        = SDL_CreateWindow( "SDL2 + ImGui + sol2 + Lua + cglm (2D)", SDL_WINDOWPOS_CENTERED,
                                                   SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN );

    SDL_Renderer * renderer    = SDL_CreateRenderer( window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED );

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO & io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForSDLRenderer( window, renderer );
    ImGui_ImplSDLRenderer2_Init( renderer );

    // Initialize Lua
    sol::state lua;
    lua.open_libraries( sol::lib::base, sol::lib::string );

    // Embedded Lua script
    const char * lua_script = R"(
        return {
            message = "Hello from Lua!",
            counter = 0,
            increment_counter = function(self)
                self.counter = self.counter + 1
                return self.counter
            end,
            get_greeting = function(name)
                return "Hello " .. (name or "Anonymous") .. "!"
            end
        }
    )";

    sol::table app_data;
    try
        {
            app_data = lua.script( lua_script );
        }
    catch( const sol::error & e )
        {
            SDL_Log( "Lua error: %s", e.what() );
            return EXIT_FAILURE;
        }

    // cglm variables for 2D transformations
    vec2  position       = { 0.0f, 0.0f };
    float rotation_angle = 0.0f; // Degrees
    vec2  scale          = { 1.0f, 1.0f };
    mat3  model_matrix;

    // Main loop variables
    bool        running       = true;
    char        name_buf[128] = "";
    std::string lua_message   = app_data["message"];
    int         counter       = app_data["counter"];

    while( running )
        {
            SDL_Event event;
            while( SDL_PollEvent( &event ) )
                {
                    ImGui_ImplSDL2_ProcessEvent( &event );
                    if( event.type == SDL_QUIT ) running = false;
                    if( event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE
                        && event.window.windowID == SDL_GetWindowID( window ) )
                        {
                            running = false;
                        }
                }

            // Update cglm 2D transformations
            glm_mat3_identity( model_matrix );
            glm_translate2d( model_matrix, position );
            glm_rotate2d( model_matrix, glm_rad( rotation_angle ) );
            glm_scale2d( model_matrix, scale );

            // Start ImGui frame
            ImGui_ImplSDLRenderer2_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            // Lua Interaction Window
            ImGui::Begin( "Lua Controls" );
            ImGui::Text( "From Lua: %s", lua_message.c_str() );
            ImGui::Text( "Counter: %d", counter );

            if( ImGui::InputText( "Your name", name_buf, IM_ARRAYSIZE( name_buf ) ) )
                {
                    try
                        {
                            app_data["user_name"] = std::string( name_buf );
                        }
                    catch( const sol::error & e )
                        {
                            SDL_Log( "Lua error: %s", e.what() );
                        }
                }

            if( ImGui::Button( "Increment Counter" ) )
                {
                    try
                        {
                            sol::function increment = app_data["increment_counter"];
                            counter                 = increment( app_data );
                        }
                    catch( const sol::error & e )
                        {
                            SDL_Log( "Lua error: %s", e.what() );
                        }
                }

            if( ImGui::Button( "Get Greeting" ) )
                {
                    try
                        {
                            sol::function get_greeting = app_data["get_greeting"];
                            lua_message                = get_greeting( name_buf );
                        }
                    catch( const sol::error & e )
                        {
                            SDL_Log( "Lua error: %s", e.what() );
                        }
                }
            ImGui::End();

            // cglm 2D Transformations Window
            ImGui::Begin( "2D Transformations" );
            ImGui::SliderFloat2( "Position", position, -100.0f, 100.0f );
            ImGui::SliderFloat( "Rotation", &rotation_angle, 0.0f, 360.0f, "%.1f deg" );
            ImGui::SliderFloat2( "Scale", scale, 0.1f, 5.0f );

            ImGui::Text( "Model Matrix:" );
            for( int i = 0; i < 3; ++i )
                {
                    ImGui::Text( "%.2f %.2f %.2f", model_matrix[i][0], model_matrix[i][1], model_matrix[i][2] );
                }
            ImGui::End();

            // Rendering
            ImGui::Render();
            SDL_SetRenderDrawColor( renderer, 45, 45, 45, 255 );
            SDL_RenderClear( renderer );
            ImGui_ImplSDLRenderer2_RenderDrawData( ImGui::GetDrawData(), renderer );
            SDL_RenderPresent( renderer );
        }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit();

    return EXIT_SUCCESS;
}
