//------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
//------------------------------------------------------------------------------------------------------------
struct Callable_Tag  // C++26 P3394 Reflection Annotation Tag
{
    
};
//------------------------------------------------------------------------------------------------------------
inline constexpr Callable_Tag Callable { };
//------------------------------------------------------------------------------------------------------------
template <typename... Args> class Delegate
{
public:
    constexpr explicit Delegate(const char *signal_name)
        : Signal_Name(signal_name) { }

    // Type-safe broadcast: compiler verifies all arguments match Args...
    void Broadcast(godot::Object *owner, const Args&... args) const
    {
        if (owner != nullptr)
        {
            owner->emit_signal(Signal_Name, args...);
        }
    }

    [[nodiscard]] const godot::StringName &Get_Name() const
    {
        return Signal_Name;
    }

private:
    godot::StringName Signal_Name;

};
//------------------------------------------------------------------------------------------------------------
// Replaces standard GDCLASS and completely eliminates manual _bind_methods boilerplate
#define GDCLASS_REFLECT(m_class, m_inherits)                                     \
    GDCLASS(m_class, m_inherits);                                              \
protected:                                                                     \
    static void _bind_methods()                                                \
    {                                                                          \
        GDExtension_Reflect::Auto_Bind_Class<m_class>();                       \
    }                                                                          \
private:

//------------------------------------------------------------------------------------------------------------
// Generates both variable name and string literal automatically:
#define DECLARE_DELEGATE(name, ...) \
    Delegate<__VA_ARGS__> name{#name}
//------------------------------------------------------------------------------------------------------------
#if !defined(__clang__)
    #define GD_CALLABLE [[=Callable]]  // GCC 16 (Linux Desktop) - Native C++26 P3394 attribute
#else
    #define GD_CALLABLE  // Clang / Emscripten (WebAssembly) - Fallback
#endif
//------------------------------------------------------------------------------------------------------------
#if !defined(__clang__)  // Fallback interface for IDE language server (Clangd) to eliminate parser errors
//------------------------------------------------------------------------------------------------------------
#include <godot_cpp/core/class_db.hpp>

// Real C++26 P2996 static reflection implementation for GCC 16
#include <array>
#include <meta>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
//------------------------------------------------------------------------------------------------------------
namespace GDExtension_Reflect
{
    // Trait to detect Delegate<Args...> types inside GDExtension_Reflect namespace
    template <typename T> struct Is_Delegate_Trait : std::false_type
    {
    };

    template <typename... Args> struct Is_Delegate_Trait<Delegate<Args...>> : std::true_type
    {
    };

    template <typename T> inline constexpr bool Is_Delegate_V = Is_Delegate_Trait<std::remove_cvref_t<T>>::value;

    template <typename T> struct Delegate_Binder;  // Unpacks Delegate<Args...> and registers godot::MethodInfo signal

    template <typename Tag> consteval bool Has_Annotation(std::meta::info member)
    {
        const auto annotations = std::meta::annotations_of(member);

        for (std::meta::info ann : annotations)
        {
            const std::meta::info type = std::meta::type_of(ann);

            if (type == ^^Tag || type == ^^const Tag)
            {
                return true;
            }
        }
        return false;
    }

    template <typename T> consteval godot::Variant::Type Deduce_Variant_Type()
    {
        using Decayed = std::decay_t<T>;

        if constexpr (std::is_same_v<Decayed, bool>)
        {
            return godot::Variant::BOOL;
        }
        else if constexpr (std::is_integral_v<Decayed>)
        {
            return godot::Variant::INT;
        }
        else if constexpr (std::is_floating_point_v<Decayed>)
        {
            return godot::Variant::FLOAT;
        }
        else if constexpr (std::is_same_v<Decayed, godot::String>)
        {
            return godot::Variant::STRING;
        }
        else if constexpr (std::is_same_v<Decayed, godot::Vector2>)
        {
            return godot::Variant::VECTOR2;
        }
        else if constexpr (std::is_pointer_v<Decayed> && std::is_base_of_v<godot::Object, std::remove_pointer_t<Decayed> >)
        {
            return godot::Variant::OBJECT;
        }
        else if constexpr (std::is_enum_v<Decayed>)
        {
            return godot::Variant::INT;
        }
        else
        {
            return godot::Variant::NIL;
        }
    }

    template <typename T>
    struct Getter_Traits;
    
    // 1. Const getter
    template <typename Class_Type, typename Prop_Type>
    struct Getter_Traits<Prop_Type (Class_Type::*)() const>
    {
        using Return_Type = Prop_Type;
        using Class = Class_Type;
    };
    
    // 2. Const getter (noexcept)
    template <typename Class_Type, typename Prop_Type>
    struct Getter_Traits<Prop_Type (Class_Type::*)() const noexcept>
    {
        using Return_Type = Prop_Type;
        using Class = Class_Type;
    };
    
    // 3. Non-const getter
    template <typename Class_Type, typename Prop_Type>
    struct Getter_Traits<Prop_Type (Class_Type::*)()>
    {
        using Return_Type = Prop_Type;
        using Class = Class_Type;
    };
    
    // 4. Non-const getter (noexcept)
    template <typename Class_Type, typename Prop_Type>
    struct Getter_Traits<Prop_Type (Class_Type::*)() noexcept>
    {
        using Return_Type = Prop_Type;
        using Class = Class_Type;
    };


    struct Property_Pair
    {
        std::meta::info getter;
        std::meta::info setter;
    };

    template <typename T> consteval auto Collect_Property_Pairs()
    {
        constexpr auto context = std::meta::access_context::current();
        const auto members = std::meta::members_of(^^T, context);
        std::vector<Property_Pair> pairs;

        for (std::meta::info m1 : members)
        {
            if (!std::meta::is_function(m1) || !std::meta::has_identifier(m1) )
            {
                continue;
            }

            std::string_view getter_name = std::meta::identifier_of(m1);
            if (!getter_name.starts_with("Get_") )
            {
                continue;
            }

            std::string_view base_name = getter_name.substr(4);
            for (std::meta::info m2 : members)
            {
                if (!std::meta::is_function(m2) || !std::meta::has_identifier(m2) )
                {
                    continue;
                }

                std::string_view setter_name = std::meta::identifier_of(m2);
                if (setter_name.starts_with("Set_") && setter_name.substr(4) == base_name)
                {
                    pairs.push_back(Property_Pair{ .getter = m1, .setter = m2 } );

                    break;
                }
            }
        }
        return pairs;
    }

    template <typename T> consteval std::size_t Get_Property_Count()
    {
        return Collect_Property_Pairs<T>().size();
    }

    template <typename T> consteval auto Get_Properties_Array()
    {
        constexpr std::size_t count = Get_Property_Count<T>();
        std::array<Property_Pair, count> array{};
        const auto vector = Collect_Property_Pairs<T>();

        for (std::size_t i = 0; i < count; ++i)
        {
            array[i] = vector[i];
        }
        return array;
    }

    template <std::meta::info Getter> consteval auto Format_Property_Name()
    {
        constexpr std::string_view getter_name = std::meta::identifier_of(Getter);
        std::array<char, 64> buffer{};
        std::string_view base = getter_name.substr(4);  // Strips "Get_" and preserves casing (e.g. "Speed", "Is_Enabled")

        for (std::size_t i = 0; i < base.size() && i < 63; ++i)
        {
            buffer[i] = base[i];
        }
        return buffer;
    }

    template <typename EnumT> consteval auto Build_Enum_Hint_String()
    {// C++26 P2996: Builds comma-separated string for Godot Inspector dropdown at compile time!

        std::array<char, 256> buffer { };
        std::size_t offset = 0;

        const auto enumerators = std::meta::enumerators_of(^^EnumT); 

        for (std::size_t i = 0; i < enumerators.size(); ++i)
        {
            std::string_view name = std::meta::identifier_of(enumerators[i]);

            for (char c : name)
            {
                if (offset < 255)
                {
                    buffer[offset++] = c;
                }
            }

            if (i + 1 < enumerators.size() && offset < 255)  // Comma separator between enum elements
            {
                buffer[offset++] = ',';
            }
        }
        return buffer;
    }

    template <typename EnumT> consteval std::size_t Get_Enumerator_Count()
    {
        return std::meta::enumerators_of(^^EnumT).size();
    }

    template <typename EnumT> consteval auto Get_Enumerators_Array()
    {
        constexpr std::size_t count = Get_Enumerator_Count<EnumT>();
        std::array<std::meta::info, count> array{};
        const auto vector = std::meta::enumerators_of(^^EnumT);

        for (std::size_t i = 0; i < count; ++i)
        {
            array[i] = vector[i];
        }
        return array;
    }

    template <typename ClassT, typename EnumT, std::size_t... Is> void Bind_Enum_Constants_Impl(std::index_sequence<Is...>)
    {
        constexpr auto enumerators = Get_Enumerators_Array<EnumT>();
        constexpr std::string_view enum_name = std::meta::identifier_of(^^EnumT);

        ([&]()
        {
            constexpr auto e = enumerators[Is];
            constexpr std::string_view constant_name = std::meta::identifier_of(e);
            constexpr int64_t constant_value = static_cast<int64_t>([: e :]);

            godot::ClassDB::bind_integer_constant(
                ClassT::get_class_static(),
                enum_name.data(),
                constant_name.data(),
                constant_value
            );
        }(), ...);
    }

    template <std::meta::info Getter, std::meta::info Setter> void Bind_Single_Property()
    {
        using getter_ptr_type = decltype(&[: Getter :]);
        using traits = Getter_Traits<getter_ptr_type>;
        using property_type = typename traits::Return_Type;
        using class_type = typename traits::Class;

        constexpr auto getter_ptr = &[: Getter :];
        constexpr auto setter_ptr = &[: Setter :];

        constexpr std::string_view getter_name = std::meta::identifier_of(Getter);
        constexpr std::string_view setter_name = std::meta::identifier_of(Setter);
        constexpr auto property_name_buffer = Format_Property_Name<Getter>();

        godot::ClassDB::bind_method(godot::D_METHOD(getter_name.data() ), getter_ptr);
        godot::ClassDB::bind_method(godot::D_METHOD(setter_name.data(), property_name_buffer.data() ), setter_ptr);

        constexpr godot::Variant::Type variant_type = Deduce_Variant_Type<property_type>();
        if constexpr (std::is_enum_v<property_type>)
        {
            constexpr auto hint_string_buffer = Build_Enum_Hint_String<property_type>();

            godot::ClassDB::add_property(
                class_type::get_class_static(),
                godot::PropertyInfo(
                    variant_type,
                    property_name_buffer.data(),
                    godot::PROPERTY_HINT_ENUM,
                    hint_string_buffer.data()
                ),
                setter_name.data(),
                getter_name.data()
            );
        }
        else
        {
            godot::ClassDB::add_property(
                class_type::get_class_static(),
                godot::PropertyInfo(variant_type, property_name_buffer.data()),
                setter_name.data(),
                getter_name.data()
            );
        }
    }

    template <typename T, std::size_t... Is> void Bind_Properties_Impl(std::index_sequence<Is...>)
    {
        constexpr auto properties = Get_Properties_Array<T>();
        (Bind_Single_Property<properties[Is].getter, properties[Is].setter>(), ...);
    }

    template <typename T>
    consteval auto Collect_Exposed_Methods()
    {
        constexpr auto context = std::meta::access_context::current();
        const auto members = std::meta::members_of(^^T, context);

        std::vector<std::meta::info> methods;

        for (std::meta::info m : members)
        {
            if (!std::meta::is_function(m) || !std::meta::has_identifier(m) )
            {
                continue;
            }

            std::string_view name = std::meta::identifier_of(m);
            if (name.starts_with("_") || name.starts_with("Get_") || name.starts_with("Set_") )
            {
                continue;
            }

            if (Has_Annotation<Callable_Tag>(m) )
            {
                methods.push_back(m);
            }
        }
        return methods;
    }

    template <typename T> consteval std::size_t Get_Exposed_Method_Count()
    {
        return Collect_Exposed_Methods<T>().size();
    }

    template <typename T> consteval auto Get_Exposed_Methods_Array()
    {
        constexpr std::size_t count = Get_Exposed_Method_Count<T>();
        std::array<std::meta::info, count> array { };
        const auto vector = Collect_Exposed_Methods<T>();

        for (std::size_t i = 0; i < count; ++i)
        {
            array[i] = vector[i];
        }
        return array;
    }

    template <std::meta::info Method> void Bind_Single_Method()
    {
        constexpr auto method_ptr = &[: Method :];
        constexpr std::string_view method_name = std::meta::identifier_of(Method);

        godot::ClassDB::bind_method(godot::D_METHOD(method_name.data() ), method_ptr);
    }

    template <typename T, std::size_t... Is> void Bind_Methods_Impl(std::index_sequence<Is...>)
    {
        constexpr auto methods = Get_Exposed_Methods_Array<T>();

        (Bind_Single_Method<methods[Is]>(), ...);
    }

    template <std::meta::info Member> consteval auto Format_Signal_Name()  // Converts "On_Speed_Changed" -> "on_speed_changed"
    {
        constexpr std::string_view member_name = std::meta::identifier_of(Member);
        std::array<char, 64> buffer{};

        for (std::size_t i = 0; i < member_name.size() && i < 63; ++i)
        {
            char c = member_name[i];

            if (c >= 'A' && c <= 'Z')
            {
                c = static_cast<char>(c + ('a' - 'A') );
            }

            buffer[i] = c;
        }
        return buffer;
    }

    template <typename T> struct Delegate_Binder;

    template <typename... Args> struct Delegate_Binder<Delegate<Args...>>
    {
        static void Bind(const char *signal_name, const godot::StringName &class_name)
        {
            godot::MethodInfo signal_info(signal_name);
            std::size_t arg_index = 0;

            ([&]()
            {
                constexpr godot::Variant::Type var_type = Deduce_Variant_Type<Args>();
                const std::string arg_name = "arg" + std::to_string(arg_index++);

                signal_info.arguments.push_back(godot::PropertyInfo(var_type, arg_name.c_str() ) );
            }(), ...);
            godot::ClassDB::add_signal(class_name, signal_info);
        }
    };

    template <typename T> consteval auto Collect_Delegates()
    {
        constexpr auto context = std::meta::access_context::current();
        const auto fields = std::meta::nonstatic_data_members_of(^^T, context);
        std::vector<std::meta::info> delegates;

        for (std::meta::info field : fields)
        {
            if (!std::meta::has_identifier(field) )
            {
                continue;
            }

            std::string_view name = std::meta::identifier_of(field);
            if (name.starts_with("_") )
            {
                continue;
            }
            delegates.push_back(field);
        }
        return delegates;
    }

    template <typename T> consteval std::size_t Get_Delegate_Count()
    {
        return Collect_Delegates<T>().size();
    }

    template <typename T> consteval auto Get_Delegates_Array()
    {
        constexpr std::size_t count = Get_Delegate_Count<T>();
        std::array<std::meta::info, count> array{};
        const auto vector = Collect_Delegates<T>();

        for (std::size_t i = 0; i < count; ++i)
        {
            array[i] = vector[i];
        }
        return array;
    }

    template <typename ClassT, std::meta::info Member> void Bind_Single_Delegate()
    {
        using Field_Type = [: std::meta::type_of(Member) :];

        if constexpr (Is_Delegate_V<Field_Type>)
        {
            constexpr auto signal_name_buffer = Format_Signal_Name<Member>();
            Delegate_Binder<Field_Type>::Bind(signal_name_buffer.data(), ClassT::get_class_static() );
        }
    }

    template <typename T, std::size_t... Is> void Bind_Delegates_Impl(std::index_sequence<Is...>)
    {
        constexpr auto delegates = Get_Delegates_Array<T>();

        (Bind_Single_Delegate<T, delegates[Is]>(), ...);
    }

    template <typename T> void Auto_Bind_Class()
    {
        constexpr std::size_t property_count = Get_Property_Count<T>();
        if constexpr (property_count > 0)  // Auto-bind properties (Get_*/Set_ *pairs)
        {
            Bind_Properties_Impl<T>(std::make_index_sequence<property_count> { } );
        }

        constexpr std::size_t method_count = Get_Exposed_Method_Count<T>();
        if constexpr (method_count > 0)  // Auto-bind [[=Callable]] methods
        {
            Bind_Methods_Impl<T>(std::make_index_sequence<method_count> { } );
        }

        constexpr std::size_t delegate_count = Get_Delegate_Count<T>();
        if constexpr (delegate_count > 0)  // Auto-bind Delegate<Args...> signals
        {
            Bind_Delegates_Impl<T>(std::make_index_sequence<delegate_count> { } );
        }
    }
}
//------------------------------------------------------------------------------------------------------------
#else
//------------------------------------------------------------------------------------------------------------
namespace GDExtension_Reflect
{
    template <typename T> void Auto_Bind_Class()
    {

    }
}
//------------------------------------------------------------------------------------------------------------
#endif // __clang__
//------------------------------------------------------------------------------------------------------------
