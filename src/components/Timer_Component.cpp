//------------------------------------------------------------------------------------------------------------
#include <components/Timer_Component.h>

#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/rich_text_label.hpp>

#include <algorithm>
#include <cmath>
#include <format>
#include <string>
//------------------------------------------------------------------------------------------------------------




// ATimer_Component
void ATimer_Component::_ready()
{
    godot::Node *parent_node;

    if (Active_Display_Handler_Ptr == nullptr)
    {
        parent_node = get_parent();
        if (parent_node != nullptr)
            Bind_Target(parent_node);
    }

    Time_Left = Duration;
    Update_Display_Text(true);

    if (Auto_Start == true)
        Start();
    else
        Pause();
}
//------------------------------------------------------------------------------------------------------------
void ATimer_Component::_process(double delta)
{
    if (Is_Running == false)
        return;

    Time_Left -= static_cast<float>(delta);
    if (Time_Left <= 0.0f)
    {
        Time_Left = 0.0f;
        Is_Running = false;

        set_process(false);
        Update_Display_Text(true);
        On_Timer_Finished.Broadcast(this);

        return;
    }

    Update_Display_Text(false);
}
//------------------------------------------------------------------------------------------------------------
void ATimer_Component::Start() noexcept
{
    if (Time_Left <= 0.0f)
        Time_Left = Duration;

    Is_Running = true;

    set_process(true);
}
//------------------------------------------------------------------------------------------------------------
void ATimer_Component::Pause() noexcept
{
    Is_Running = false;

    set_process(false);
}
//------------------------------------------------------------------------------------------------------------
void ATimer_Component::Reset() noexcept
{
    Time_Left = Duration;
    Last_Displayed_Second = -1;

    Update_Display_Text(true);
}
//------------------------------------------------------------------------------------------------------------
void ATimer_Component::Add_Time(float seconds) noexcept
{
    Time_Left = std::max(0.0f, Time_Left + seconds);
    Update_Display_Text(true);

    if (Time_Left <= 0.0f)
    {
        if (Is_Running == true)
        {
            Is_Running = false;

            set_process(false);
            On_Timer_Finished.Broadcast(this);
        }

        return;
    }

    if (Is_Running == true)
        set_process(true);
}
//------------------------------------------------------------------------------------------------------------
[[nodiscard]] bool ATimer_Component::Get_Auto_Start() const noexcept
{
    return Auto_Start;
}
//------------------------------------------------------------------------------------------------------------
[[nodiscard]] bool ATimer_Component::Get_Is_Running() const noexcept
{
    return Is_Running;
}
//------------------------------------------------------------------------------------------------------------
[[nodiscard]] float ATimer_Component::Get_Duration() const noexcept
{
    return Duration;
}
//------------------------------------------------------------------------------------------------------------
[[nodiscard]] float ATimer_Component::Get_Time_Left() const noexcept
{
    return Time_Left;
}
//------------------------------------------------------------------------------------------------------------
void ATimer_Component::Set_Auto_Start(bool auto_start) noexcept
{
    Auto_Start = auto_start;
}
//------------------------------------------------------------------------------------------------------------
void ATimer_Component::Set_Is_Running(bool running) noexcept
{
    if (running == true)
        Start();
    else
        Pause();
}
//------------------------------------------------------------------------------------------------------------
void ATimer_Component::Set_Duration(float duration) noexcept
{
    Duration = std::max(0.0f, duration);
}
//------------------------------------------------------------------------------------------------------------
void ATimer_Component::Bind_Target(godot::Node *target) noexcept
{
    Target_Label = nullptr;
    Target_Rich_Text_Label = nullptr;
    Active_Display_Handler_Ptr = nullptr;

    if (target == nullptr)
        return;

    Target_Label = godot::Object::cast_to<godot::Label>(target);
    if (Target_Label != nullptr)
    {
        Active_Display_Handler_Ptr = &ATimer_Component::Update_Label;
        Update_Display_Text(true);

        return;
    }

    Target_Rich_Text_Label = godot::Object::cast_to<godot::RichTextLabel>(target);
    if (Target_Rich_Text_Label != nullptr)
    {
        Active_Display_Handler_Ptr = &ATimer_Component::Update_Rich_Text_Label;
        Update_Display_Text(true);

        return;
    }
}
//------------------------------------------------------------------------------------------------------------
void ATimer_Component::Update_Display_Text(bool force_update)
{
    int32_t current_second;
    int32_t minutes;
    int32_t seconds;
    std::string formatted_time;

    if (Active_Display_Handler_Ptr == nullptr)
        return;

    current_second = static_cast<int32_t>(std::ceil(Time_Left) );
    if ( (force_update == false) && (current_second == Last_Displayed_Second) )
        return;

    Last_Displayed_Second = current_second;
    minutes = current_second / 60;
    seconds = current_second % 60;
    formatted_time = std::format("{:02}:{:02}", minutes, seconds);

    (this->*Active_Display_Handler_Ptr)(formatted_time.c_str() );
}
//------------------------------------------------------------------------------------------------------------
void ATimer_Component::Update_Label(const char *text) noexcept
{
    Target_Label->set_text(text);
}
//------------------------------------------------------------------------------------------------------------
void ATimer_Component::Update_Rich_Text_Label(const char *text) noexcept
{
    Target_Rich_Text_Label->set_text(text);
}
//------------------------------------------------------------------------------------------------------------
ATimer_Component *ATimer_Component::Attach(godot::Node *target, float duration, bool auto_start)
{
    ATimer_Component *timer_component;

    if (target == nullptr)
        return nullptr;

    timer_component = memnew(ATimer_Component);
    timer_component->Set_Duration(duration);
    timer_component->Set_Auto_Start(auto_start);
    timer_component->Bind_Target(target);
    target->add_child(timer_component);

    return timer_component;
}
//------------------------------------------------------------------------------------------------------------