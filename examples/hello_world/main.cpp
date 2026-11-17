/*******************************************************************************************
 *
 *   2D Game Engine example - hello_world
 *
 *   Example licensed under an unmodified MIT license.
 *
 *   Copyright (c) 2026 Leandro Peres and contributors
 *
 ********************************************************************************************/

#include "engine/engine.hpp"
#include "example.hpp"

#include <string>

// Example
// ImGui + Lua + cglm playground running on the ExampleApp boilerplate
//----------------------------------------------------------
class HelloWorldExample : public ExampleApp
{
  public:
    HelloWorldExample()
        : ExampleApp( "SDL2 + ImGui + sol2 + Lua + cglm (2D)" )
    {
    }

  protected:
    void
    OnStart() override
    {
        // ImGui Setup
        //----------------------------------------------------------
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO & io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplSDL2_InitForSDLRenderer( GetWindow(), GetRenderer() );
        ImGui_ImplSDLRenderer2_Init( GetRenderer() );

        // Lua Setup
        //----------------------------------------------------------
        lua.open_libraries( sol::lib::base, sol::lib::string );

        try
            {
                appData    = lua.script( luaScript );
                luaMessage = appData["message"];
                counter    = appData["counter"];

                EN_LOG_INFO( "Lua script loaded, message: {}", luaMessage );
            }
        catch( const sol::error & e )
            {
                EN_LOG_ERROR( "Lua error: {}", e.what() );

                Stop();
            }
    }

    void
    OnProcessInput( const SDL_Event & event ) override
    {
        ImGui_ImplSDL2_ProcessEvent( &event );
    }

    void
    OnUpdate( double deltaTime ) override
    {
        EN_UNUSED( deltaTime );

        // 2D Transformation Update
        //----------------------------------------------------------
        glm_mat3_identity( modelMatrix );
        glm_translate2d( modelMatrix, position );
        glm_rotate2d( modelMatrix, glm_rad( rotationAngle ) );
        glm_scale2d( modelMatrix, scale );

        // Frame Begin
        //----------------------------------------------------------
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Lua Interaction Window
        //----------------------------------------------------------
        ImGui::Begin( "Lua Controls" );
        ImGui::Text( "From Lua: %s", luaMessage.c_str() );
        ImGui::Text( "Counter: %d", counter );

        if( ImGui::InputText( "Your name", nameBuf, IM_ARRAYSIZE( nameBuf ) ) )
            {
                try
                    {
                        appData["user_name"] = std::string( nameBuf );
                    }
                catch( const sol::error & e )
                    {
                        EN_LOG_ERROR( "Lua error: {}", e.what() );
                    }
            }

        if( ImGui::Button( "Increment Counter" ) )
            {
                try
                    {
                        sol::function increment = appData["increment_counter"];
                        counter                 = increment( appData );
                    }
                catch( const sol::error & e )
                    {
                        EN_LOG_ERROR( "Lua error: {}", e.what() );
                    }
            }

        if( ImGui::Button( "Get Greeting" ) )
            {
                try
                    {
                        sol::function getGreeting = appData["get_greeting"];
                        luaMessage                = getGreeting( nameBuf );
                    }
                catch( const sol::error & e )
                    {
                        EN_LOG_ERROR( "Lua error: {}", e.what() );
                    }
            }
        ImGui::End();

        // cglm 2D Transformations Window
        //----------------------------------------------------------
        ImGui::Begin( "2D Transformations" );
        ImGui::SliderFloat2( "Position", position, -100.0f, 100.0f );
        ImGui::SliderFloat( "Rotation", &rotationAngle, 0.0f, 360.0f, "%.1f deg" );
        ImGui::SliderFloat2( "Scale", scale, 0.1f, 5.0f );

        ImGui::Text( "Model Matrix:" );
        for( int i = 0; i < 3; ++i )
            {
                ImGui::Text( "%.2f %.2f %.2f", modelMatrix[i][0], modelMatrix[i][1], modelMatrix[i][2] );
            }
        ImGui::End();
    }

    void
    OnRender( SDL_Renderer & renderer ) override
    {
        // Frame Rendering and Presenting
        //----------------------------------------------------------
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData( ImGui::GetDrawData(), &renderer );
    }

    void
    OnEnd() override
    {
        // ImGui Shutdown
        //----------------------------------------------------------
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

  private:
    // Script evaluated once at startup, every later interaction goes through the returned table
    const char * luaScript { R"(
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
    )" };

    // Lua State
    sol::state lua {};
    sol::table appData {};
    char nameBuf[128] {};
    std::string luaMessage { "Hello from Lua!" };
    int counter {};

    // 2D transform exposed to the transformation window sliders
    vec2 position { 0.0f, 0.0f };
    float rotationAngle { 0.0f }; // Degrees
    vec2 scale { 1.0f, 1.0f };
    mat3 modelMatrix {};
};

// Instantiated by examples/main.cpp
//----------------------------------------------------------
std::unique_ptr< ExampleApp >
CreateExample()
{
    return std::make_unique< HelloWorldExample >();
}
