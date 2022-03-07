#include"pch.h"
#include<MeshView.h>
#include<Event/MouseEvent.h>



int main(void)
{
    
    MV::Log::Init();
    MV::Application* app = MV::Application::Create("MeshView");
    MESH_INFO("Welcome to MeshView!");

    app->Run();
    app->Close();
   
}