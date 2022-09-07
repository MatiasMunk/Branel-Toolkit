#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_allegro5.h>

#include "InstalledPrograms.h"
#include "gui.h"

int main()
{
    ALLEGRO_DISPLAY* display = nullptr;
    ALLEGRO_TIMER* timer = nullptr;
    ALLEGRO_EVENT_QUEUE* event_queue = nullptr;

    if (!al_init())
    {
        throw std::runtime_error("Failed to initialize system!");
        return -1;
    }

    if (!al_init_primitives_addon())
    {
        throw std::runtime_error("Failed to initialize primitives addon!");
        return -1;
    }

    if(!al_install_keyboard())
    {
        throw std::runtime_error("Failed to initialize keyboard!");
        return -1;
    }

    if(!al_install_mouse())
    {
        throw std::runtime_error("Failed to initialize mouse!");
        return -1;
    }

    display = al_create_display(400, 200);
    if(!display)
    {
        throw std::runtime_error("Could not create display!");
    }
    al_set_window_title(display, "Branel Toolkit");

    event_queue = al_create_event_queue();
    if(!event_queue)
    {
        throw std::runtime_error("Could not create event queue!");
    }

    timer = al_create_timer(1.0 / 60);
    if(!timer)
    {
        throw std::runtime_error("Could not create timer!");
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    al_start_timer(timer);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGui_ImplAllegro5_Init(display);

    GUI gui;

    bool done = false;
    bool redraw = false;
    while(!done)
    {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        ImGui_ImplAllegro5_ProcessEvent(&event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
        {
            ImGui_ImplAllegro5_InvalidateDeviceObjects();
            al_acknowledge_resize(display);
            ImGui_ImplAllegro5_CreateDeviceObjects();
        }
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
        }
        else if(event.type == ALLEGRO_EVENT_TIMER)
        {
            redraw = true;
        }

        if(redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;
            
            ImGui_ImplAllegro5_NewFrame();

            al_clear_to_color(al_map_rgb(0, 0, 0));

            gui.Process();
            gui.Render();

            al_flip_display();
        }

        al_rest(0.0001);
    }

    ImGui_ImplAllegro5_Shutdown();
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    
	/*vector<Software>* list = InstalledPrograms::GetInstalledPrograms(false);
	for(vector<Software>::iterator iter = list->begin(); iter!=list->end(); iter++)
	{
		std::wcout << L"Software: " << iter->DisplayName << std::endl;
        std::wcout << L"Location: " << iter->InstallLocation << std::endl;
        std::wcout << L"Version: " << iter->Version << std::endl;
        std::wcout << L"Icon: " << iter->Icon << std::endl;
        if(iter->Architecture == X86)
            std::wcout << L"Architecture: X86" << std::endl;
        else if(iter->Architecture == X64)
            std::wcout << L"Architecture: X64" << std::endl;
        else
            std::wcout << L"Architecture: Unknown" << std::endl;

        std::cout << std::endl;
	}*/

    return 0;
}