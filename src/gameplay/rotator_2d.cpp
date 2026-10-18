//------------------------------------------------------------------------------------------------------------
#include <gameplay/rotator_2d.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
//------------------------------------------------------------------------------------------------------------




// Rotator2D
void Rotator2D::_ready()
{
    godot::UtilityFunctions::print("Rotator2D initialized from C++!");
    godot::UtilityFunctions::print("HOT RELOAD WORKS! New speed applied!");
}
//------------------------------------------------------------------------------------------------------------
void Rotator2D::_process(double delta)
{
    if (!Is_Enabled)
        return;
    
    float current_rotation = get_rotation();  // Zero-Cost: Direct transform update without heap allocations

    set_rotation(current_rotation + _speed * static_cast<float>(delta));
}
//------------------------------------------------------------------------------------------------------------
void Rotator2D::Set_Is_Enabled(bool value)
{
    Is_Enabled = value;
}
//------------------------------------------------------------------------------------------------------------
void Rotator2D::Set_Speed(float speed)
{
    _speed = speed;
}
//------------------------------------------------------------------------------------------------------------
bool Rotator2D::Get_Is_Enabled() const
{
    return Is_Enabled;
}
//------------------------------------------------------------------------------------------------------------
float Rotator2D::Get_Speed() const
{
    return _speed;
}
//------------------------------------------------------------------------------------------------------------
