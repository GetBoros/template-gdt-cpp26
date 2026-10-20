//------------------------------------------------------------------------------------------------------------
#include <core/register_types.h>
#include <gameplay/rotator_2d.h>

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
//------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------------------
void Initialize_Module(godot::ModuleInitializationLevel level)
{
    if (level != godot::MODULE_INITIALIZATION_LEVEL_SCENE)
        return;

    godot::ClassDB::register_class<ARotator2D>();

}
//------------------------------------------------------------------------------------------------------------
void Uninitialize_Module(godot::ModuleInitializationLevel level)
{
    if (level != godot::MODULE_INITIALIZATION_LEVEL_SCENE)
        return;
}
//------------------------------------------------------------------------------------------------------------




// GDExtension Handshake Entry Point (Pure C ABI)
//------------------------------------------------------------------------------------------------------------
extern "C"
{
    GDExtensionBool GDE_EXPORT game_library_init(GDExtensionInterfaceGetProcAddress address_getter, GDExtensionClassLibraryPtr library_handle, GDExtensionInitialization *initialization_data)
    {
      godot::GDExtensionBinding::InitObject init_object(address_getter, library_handle, initialization_data);
    
      init_object.register_initializer(Initialize_Module);
      init_object.register_terminator(Uninitialize_Module);
      init_object.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);
    
      return init_object.init();
    }
}
//------------------------------------------------------------------------------------------------------------
