//------------------------------------------------------------------------------------------------------------
#ifndef ROTATOR_2D_H
#define ROTATOR_2D_H
//------------------------------------------------------------------------------------------------------------
#include <godot_cpp/classes/node2d.hpp>
//------------------------------------------------------------------------------------------------------------
class Rotator2D : public godot::Node2D
{
    GDCLASS(Rotator2D, godot::Node2D);

public:
    ~Rotator2D() override = default;
    Rotator2D() = default;

    void _ready() override;
    void _process(double delta) override;

    void Set_Speed(float speed);
    float Get_Speed() const;

protected:
    static void _bind_methods();

private:
    float _speed = 2.0f;

};
//------------------------------------------------------------------------------------------------------------
#endif // ROTATOR_2D_H
//------------------------------------------------------------------------------------------------------------
