//------------------------------------------------------------------------------------------------------------
#include <components/Rotator2D_Component.h>

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/node2d.hpp>
//------------------------------------------------------------------------------------------------------------




// ARotator2D_Component
void ARotator2D_Component::_ready()
{
    godot::Node *parent_node;

    if (Active_Tick_Handler_Ptr != nullptr)
        return;

    parent_node = get_parent();
    if (parent_node != nullptr)
        Bind_Target(parent_node);
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D_Component::_process(double delta)
{
    (this->*Active_Tick_Handler_Ptr)(static_cast<float>(delta) );
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D_Component::Start() noexcept
{
    Is_Enabled = true;

    set_process(true);
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D_Component::Stop() noexcept
{
    Is_Enabled = false;

    set_process(false);
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D_Component::Reset_Speed() noexcept
{
    Set_Speed(2.0f);
}
//------------------------------------------------------------------------------------------------------------
[[nodiscard]] bool ARotator2D_Component::Get_Is_Enabled() const noexcept
{
    return Is_Enabled;
}
//------------------------------------------------------------------------------------------------------------
[[nodiscard]] bool ARotator2D_Component::Get_Auto_Center_Pivot() const noexcept
{
    return Auto_Center_Pivot;
}
//------------------------------------------------------------------------------------------------------------
[[nodiscard]] float ARotator2D_Component::Get_Speed() const noexcept
{
    return Speed;
}
//------------------------------------------------------------------------------------------------------------
[[nodiscard]] ERotation_Direction ARotator2D_Component::Get_Direction() const noexcept
{
    return Rotation_Direction;
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D_Component::Set_Is_Enabled(bool is_enabled) noexcept
{
    Is_Enabled = is_enabled;

    set_process(Is_Enabled);
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D_Component::Set_Auto_Center_Pivot(bool auto_center) noexcept
{
    Auto_Center_Pivot = auto_center;

    if ( (Auto_Center_Pivot == true) && (Target_Control != nullptr) )
        Target_Control->set_pivot_offset(Target_Control->get_size() * 0.5f);
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D_Component::Set_Speed(float speed) noexcept
{
    Speed = speed;

    On_Speed_Changed.Broadcast(this, Speed);
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D_Component::Set_Direction(ERotation_Direction direction) noexcept
{
    Rotation_Direction = direction;
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D_Component::Bind_Target(godot::Node *target) noexcept
{
    Target_Node2D = nullptr;
    Target_Control = nullptr;
    Active_Tick_Handler_Ptr = nullptr;

    if (target == nullptr)
    {
        set_process(false);
        
        return;
    }

    Target_Node2D = godot::Object::cast_to<godot::Node2D>(target);
    if (Target_Node2D != nullptr)
    {
        Active_Tick_Handler_Ptr = &ARotator2D_Component::Tick_Node2D;
        set_process(Is_Enabled);

        return;
    }

    Target_Control = godot::Object::cast_to<godot::Control>(target);
    if (Target_Control != nullptr)
    {
        Active_Tick_Handler_Ptr = &ARotator2D_Component::Tick_Control;

        if (Auto_Center_Pivot == true)
            Target_Control->set_pivot_offset(Target_Control->get_size() * 0.5f);

        set_process(Is_Enabled);

        return;
    }
    set_process(false);
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D_Component::Tick_Node2D(float delta) noexcept
{
    float delta_rotation;
    float sign;

    sign = (Rotation_Direction == ERotation_Direction::Clockwise) ? 1.0f : -1.0f;
    delta_rotation = sign * Speed * delta;

    Target_Node2D->set_rotation(Target_Node2D->get_rotation() + delta_rotation);
}
//------------------------------------------------------------------------------------------------------------
void ARotator2D_Component::Tick_Control(float delta) noexcept
{
    float delta_rotation;
    float sign;

    sign = (Rotation_Direction == ERotation_Direction::Clockwise) ? 1.0f : -1.0f;
    delta_rotation = sign * Speed * delta;

    Target_Control->set_rotation(Target_Control->get_rotation() + delta_rotation);
}
//------------------------------------------------------------------------------------------------------------