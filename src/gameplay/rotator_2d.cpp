//------------------------------------------------------------------------------------------------------------
#include <gameplay/rotator_2d.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
//------------------------------------------------------------------------------------------------------------




// ARotator2D
void ARotator2D::_ready()
{
    godot::UtilityFunctions::print("ARotator2D initialized from C++!");
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D::_process(double delta)
{
    if (!Is_Enabled)
        return;

    float sign = (Rotation_Direction == ERotation_Direction::Clockwise) ? 1.0f : -1.0f;  // Direction multiplier: +1.0 for Clockwise, -1.0 for Counter_Clockwise

    float current_rotation = get_rotation();
    set_rotation(current_rotation + sign * Speed * static_cast<float>(delta) );
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D::Set_Is_Enabled(bool value)
{
    Is_Enabled = value;
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D::Set_Speed(float speed)
{
    Speed = speed;

    On_Speed_Changed.Broadcast(this, Speed);
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D::Set_Direction(ERotation_Direction direction)
{
    Rotation_Direction = direction;
}
//------------------------------------------------------------------------------------------------------------
ERotation_Direction ARotator2D::Get_Direction() const
{
    return Rotation_Direction;
}
//------------------------------------------------------------------------------------------------------------
bool ARotator2D::Get_Is_Enabled() const
{
    return Is_Enabled;
}
//------------------------------------------------------------------------------------------------------------

float ARotator2D::Get_Speed() const
{
    return Speed;
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D::Reset_Speed()
{
    Speed = 2.0f;
    
    godot::UtilityFunctions::print("Speed has been reset to 0 from C++!");
}
//------------------------------------------------------------------------------------------------------------
