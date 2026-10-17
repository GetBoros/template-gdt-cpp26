//------------------------------------------------------------------------------------------------------------
#include <gameplay/rotator_2d.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
//------------------------------------------------------------------------------------------------------------




// Rotator2D
void Rotator2D::_bind_methods()
{
    // 1. Bind methods so Godot's C-ABI can call them
    godot::ClassDB::bind_method(godot::D_METHOD("Get_Speed"), &Rotator2D::Get_Speed);
    godot::ClassDB::bind_method(godot::D_METHOD("Set_Speed", "speed"), &Rotator2D::Set_Speed);

    // 2. Expose property to the Inspector (Analogue of UE5 UPROPERTY(EditAnywhere))
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::FLOAT, "speed"), "Set_Speed", "Get_Speed");
}
//------------------------------------------------------------------------------------------------------------
void Rotator2D::_ready()
{
    godot::UtilityFunctions::print("Rotator2D initialized from C++!");
    godot::UtilityFunctions::print("HOT RELOAD WORKS! New speed applied!");
}
//------------------------------------------------------------------------------------------------------------
void Rotator2D::_process(double delta)
{
    // Zero-Cost: Direct transform update without heap allocations
    float current_rotation = get_rotation();
    set_rotation(current_rotation + _speed * static_cast<float>(delta));
}
//------------------------------------------------------------------------------------------------------------
void Rotator2D::Set_Speed(float speed)
{
    _speed = speed;
}
//------------------------------------------------------------------------------------------------------------
float Rotator2D::Get_Speed() const
{
    return _speed;
}
//------------------------------------------------------------------------------------------------------------
