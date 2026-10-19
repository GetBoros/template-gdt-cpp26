//------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------
#include <core/reflection.h>
#include <godot_cpp/classes/node2d.hpp>
//------------------------------------------------------------------------------------------------------------
class Rotator2D : public godot::Node2D
{
    GDCLASS_REFLECT(Rotator2D, godot::Node2D);

public:
    ~Rotator2D() override = default;
    Rotator2D() = default;

    virtual void _ready() override;
    virtual void _process(double delta) override;

    void Set_Is_Enabled(bool value);
    void Set_Speed(float speed);

    bool Get_Is_Enabled() const;
    float Get_Speed() const;

    [[=Callable]] void Reset_Speed();
    
    Delegate<float> On_Speed_Changed {"on_speed_changed"};

private:
    bool Is_Enabled = true;
    float _speed = 2.0f;

};
//------------------------------------------------------------------------------------------------------------
