//------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------
#include <core/reflection.h>
#include <godot_cpp/classes/node2d.hpp>
//------------------------------------------------------------------------------------------------------------
enum ERotation_Direction
{
    Clockwise,
    Counter_Clockwise
};
//------------------------------------------------------------------------------------------------------------
VARIANT_ENUM_CAST(ERotation_Direction);
//------------------------------------------------------------------------------------------------------------
class ARotator2D : public godot::Node2D
{
    GDCLASS_REFLECT(ARotator2D, godot::Node2D);

public:
    ~ARotator2D() override = default;
    ARotator2D() = default;

    virtual void _ready() override;
    virtual void _process(double delta) override;

    void Set_Is_Enabled(bool value);
    void Set_Speed(float speed);
    void Set_Direction(ERotation_Direction direction);

    ERotation_Direction Get_Direction() const;
    bool Get_Is_Enabled() const;
    float Get_Speed() const;

    GD_CALLABLE void Reset_Speed();
    
    Delegate<float> On_Speed_Changed {"on_speed_changed"};

private:
    ERotation_Direction Rotation_Direction = ERotation_Direction::Clockwise;
    bool Is_Enabled = true;
    float Speed = 2.0f;

};
//------------------------------------------------------------------------------------------------------------
