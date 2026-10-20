//------------------------------------------------------------------------------------------------------------
#include <gameplay/rotator_2d.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
//------------------------------------------------------------------------------------------------------------




// Rotator2D
void Rotator2D::_ready()
{
    godot::UtilityFunctions::print("Rotator2D initialized from C++!");
}
//------------------------------------------------------------------------------------------------------------
void Rotator2D::_process(double delta)
{
    if (!Is_Enabled)
        return;

    float sign = (Rotation_Direction == ERotation_Direction::Clockwise) ? 1.0f : -1.0f;  // Direction multiplier: +1.0 for Clockwise, -1.0 for Counter_Clockwise

    float current_rotation = get_rotation();
    set_rotation(current_rotation + sign * Speed * static_cast<float>(delta) );
}
//------------------------------------------------------------------------------------------------------------
void Rotator2D::Set_Is_Enabled(bool value)
{
    Is_Enabled = value;
}
//------------------------------------------------------------------------------------------------------------
void Rotator2D::Set_Speed(float speed)
{
    Speed = speed;

    On_Speed_Changed.Broadcast(this, Speed);
}
//------------------------------------------------------------------------------------------------------------
void Rotator2D::Set_Direction(ERotation_Direction direction)
{
    Rotation_Direction = direction;
}
//------------------------------------------------------------------------------------------------------------
ERotation_Direction Rotator2D::Get_Direction() const
{
    return Rotation_Direction;
}
//------------------------------------------------------------------------------------------------------------
bool Rotator2D::Get_Is_Enabled() const
{
    return Is_Enabled;
}
//------------------------------------------------------------------------------------------------------------

float Rotator2D::Get_Speed() const
{
    return Speed;
}
//------------------------------------------------------------------------------------------------------------
void Rotator2D::Reset_Speed()
{
    Speed = 2.0f;
    
    godot::UtilityFunctions::print("Speed has been reset to 0 from C++!");
}
//------------------------------------------------------------------------------------------------------------
