//------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------
#include <core/reflection.h>

#include <godot_cpp/classes/node.hpp>
//------------------------------------------------------------------------------------------------------------
enum ERotation_Direction : uint8_t  // 1-byte enum for rotation direction
{
    Clockwise,
    Counter_Clockwise
};
//------------------------------------------------------------------------------------------------------------
VARIANT_ENUM_CAST(ERotation_Direction);  // Exposes enum to Godot Variant, Inspector, and GDScript
//------------------------------------------------------------------------------------------------------------
namespace godot
{
    class Control;
    class Node2D;
}
//------------------------------------------------------------------------------------------------------------
class ARotator2D_Component : public godot::Node  // Branchless 2D rotator for Node2D or Control
{
    GDCLASS_REFLECT(ARotator2D_Component, godot::Node);

public:
    virtual ~ARotator2D_Component() override = default;
    ARotator2D_Component() = default;

    virtual void _ready() override;  // Auto-binds parent node via Bind_Target(get_parent())
    virtual void _process(double delta) override;  // Invokes Active_Tick_Handler_Ptr; disabled when stopped or unbound

    GD_CALLABLE void Start() noexcept;  // Enables rotation
    GD_CALLABLE void Stop() noexcept;  // Disables rotation
    GD_CALLABLE void Reset_Speed() noexcept;  // Resets speed to default value

    [[nodiscard]] bool Get_Is_Enabled() const noexcept;
    [[nodiscard]] bool Get_Auto_Center_Pivot() const noexcept;
    [[nodiscard]] float Get_Speed() const noexcept;
    [[nodiscard]] ERotation_Direction Get_Direction() const noexcept;

    void Set_Is_Enabled(bool is_enabled) noexcept;
    void Set_Auto_Center_Pivot(bool auto_center) noexcept;
    void Set_Speed(float speed) noexcept;
    void Set_Direction(ERotation_Direction direction) noexcept;

    void Bind_Target(godot::Node *target) noexcept;  // Resolves target type once; sets Active_Tick_Handler_Ptr
    void Tick_Node2D(float delta) noexcept;  // Handler bound for godot::Node2D targets
    void Tick_Control(float delta) noexcept;  // Handler bound for godot::Control targets

    Delegate<float> On_Speed_Changed { "on_speed_changed" };  // Emitted when Speed changes

private:
    using Tick_Handler_Ptr = void (ARotator2D_Component::*)(float);  // Member function pointer; avoids branching on tick

    bool Auto_Center_Pivot { true };  // Centers Control pivot_offset so UI nodes do not spin around top-left corner
    bool Is_Enabled { true };
    ERotation_Direction Rotation_Direction { ERotation_Direction::Clockwise };
    float Speed { 2.0f };

    Tick_Handler_Ptr Active_Tick_Handler_Ptr { nullptr };  // Active tick handler; nullptr disables processing
    godot::Node2D *Target_Node2D { nullptr };
    godot::Control *Target_Control { nullptr };
};
//------------------------------------------------------------------------------------------------------------