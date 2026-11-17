//------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------
#include <core/reflection.h>

#include <godot_cpp/classes/node.hpp>

#include <cstdint>
//------------------------------------------------------------------------------------------------------------
namespace godot
{
    class Label;
    class RichTextLabel;
}
//------------------------------------------------------------------------------------------------------------
class ATimer_Component : public godot::Node  // Countdown component for Label/RichTextLabel or headless logic
{
    GDCLASS_REFLECT(ATimer_Component, godot::Node);

public:
    virtual ~ATimer_Component() override = default;
    ATimer_Component() = default;

    virtual void _ready() override;  // Auto-binds parent node if Attach() was not used
    virtual void _process(double delta) override;  // Ticks Time_Left; disabled via set_process(false) when idle

    GD_CALLABLE void Start() noexcept;  // Starts or resumes countdown
    GD_CALLABLE void Pause() noexcept;  // Pauses countdown
    GD_CALLABLE void Reset() noexcept;  // Resets countdown to Duration
    GD_CALLABLE void Add_Time(float seconds) noexcept;  // Adds or subtracts time

    [[nodiscard]] bool Get_Auto_Start() const noexcept;
    [[nodiscard]] bool Get_Is_Running() const noexcept;
    [[nodiscard]] float Get_Duration() const noexcept;
    [[nodiscard]] float Get_Time_Left() const noexcept;

    void Set_Auto_Start(bool auto_start) noexcept;
    void Set_Is_Running(bool running) noexcept;
    void Set_Duration(float duration) noexcept;

    void Bind_Target(godot::Node *target) noexcept;  // Resolves target type once; sets Active_Display_Handler
    void Update_Display_Text(bool force_update);  // Formats MM:SS only when displayed second changes
    void Update_Label(const char *text) noexcept;  // Handler bound for godot::Label targets
    void Update_Rich_Text_Label(const char *text) noexcept;  // Handler bound for godot::RichTextLabel targets

    Delegate<> On_Timer_Finished { "on_timer_finished" };  // Emitted when countdown reaches 0

    static ATimer_Component *Attach(godot::Node *target, float duration = 300.0f, bool auto_start = true);  // C++ factory: memnew + Bind_Target + add_child (unused in .tscn)

private:
    using Display_Handler_Ptr = void (ATimer_Component::*)(const char *) noexcept;  // Member function pointer; avoids branching on tick

    bool Auto_Start { true };
    bool Is_Running { true };
    float Duration { 300.0f };
    float Time_Left { 300.0f };
    int32_t Last_Displayed_Second { -1 };  // Throttle cache: skips std::format until integer second changes

    Display_Handler_Ptr Active_Display_Handler_Ptr { nullptr };  // Active UI writer; nullptr = zero-cost headless mode
    godot::Label *Target_Label { nullptr };
    godot::RichTextLabel *Target_Rich_Text_Label { nullptr };
};
//------------------------------------------------------------------------------------------------------------