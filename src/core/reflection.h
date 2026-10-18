//------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------
// C++26 P3394 Reflection Annotation Tag
struct Callable_Tag
{

};
//------------------------------------------------------------------------------------------------------------
inline constexpr Callable_Tag Callable { };
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
    // Checks if an AST entity has a specific P3394 annotation tag
    template <typename Tag>
    consteval bool Has_Annotation(std::meta::info member)
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

    template <typename T>
    consteval godot::Variant::Type Deduce_Variant_Type()
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
        else
        {
            return godot::Variant::NIL;
        }
    }

    template <typename T>
    struct Getter_Traits;

    template <typename ClassT, typename ReturnT>
    struct Getter_Traits<ReturnT (ClassT::*)() const>
    {
        using Class = ClassT;
        using Return_Type = ReturnT;
    };

    template <typename ClassT, typename ReturnT>
    struct Getter_Traits<ReturnT (ClassT::*)()>
    {
        using Class = ClassT;
        using Return_Type = ReturnT;
    };

    struct Property_Pair
    {
        std::meta::info getter;
        std::meta::info setter;
    };

    // Scans class AST for matching Get_<Name> and Set_<Name> pairs
    template <typename T>
    consteval auto Collect_Property_Pairs()
    {
        constexpr auto context = std::meta::access_context::current();
        const auto members = std::meta::members_of(^^T, context);

        std::vector<Property_Pair> pairs;

        for (std::meta::info m1 : members)
        {
            if (!std::meta::is_function(m1) || !std::meta::has_identifier(m1))
            {
                continue;
            }

            std::string_view getter_name = std::meta::identifier_of(m1);
            if (!getter_name.starts_with("Get_"))
            {
                continue;
            }

            std::string_view base_name = getter_name.substr(4);

            for (std::meta::info m2 : members)
            {
                if (!std::meta::is_function(m2) || !std::meta::has_identifier(m2))
                {
                    continue;
                }

                std::string_view setter_name = std::meta::identifier_of(m2);
                if (setter_name.starts_with("Set_") && setter_name.substr(4) == base_name)
                {
                    pairs.push_back(Property_Pair{ .getter = m1, .setter = m2 });
                    break;
                }
            }
        }

        return pairs;
    }

    template <typename T>
    consteval std::size_t Get_Property_Count()
    {
        return Collect_Property_Pairs<T>().size();
    }

    template <typename T>
    consteval auto Get_Properties_Array()
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

    template <std::meta::info Getter>
    consteval auto Format_Property_Name()
    {
        constexpr std::string_view getter_name = std::meta::identifier_of(Getter);
        std::string_view base = getter_name.substr(4);

        std::array<char, 64> buffer{};

        for (std::size_t i = 0; i < base.size() && i < 63; ++i)
        {
            char c = base[i];
            if (i == 0 && c >= 'A' && c <= 'Z')
            {
                c = static_cast<char>(c + ('a' - 'A'));
            }
            buffer[i] = c;
        }

        return buffer;
    }

    template <std::meta::info Getter, std::meta::info Setter>
    void Bind_Single_Property()
    {
        using Getter_Ptr_Type = decltype(&[: Getter :]);
        using Traits = Getter_Traits<Getter_Ptr_Type>;
        using Property_Type = typename Traits::Return_Type;
        using Class_Type = typename Traits::Class;

        constexpr auto getter_ptr = &[: Getter :];
        constexpr auto setter_ptr = &[: Setter :];

        constexpr std::string_view getter_name = std::meta::identifier_of(Getter);
        constexpr std::string_view setter_name = std::meta::identifier_of(Setter);
        constexpr auto property_name_buffer = Format_Property_Name<Getter>();

        godot::ClassDB::bind_method(godot::D_METHOD(getter_name.data()), getter_ptr);
        godot::ClassDB::bind_method(godot::D_METHOD(setter_name.data(), property_name_buffer.data()), setter_ptr);

        constexpr godot::Variant::Type variant_type = Deduce_Variant_Type<Property_Type>();
        godot::ClassDB::add_property(
            Class_Type::get_class_static(),
            godot::PropertyInfo(variant_type, property_name_buffer.data()),
            setter_name.data(),
            getter_name.data()
        );
    }

    template <typename T, std::size_t... Is>
    void Bind_Properties_Impl(std::index_sequence<Is...>)
    {
        constexpr auto properties = Get_Properties_Array<T>();
        (Bind_Single_Property<properties[Is].getter, properties[Is].setter>(), ...);
    }

    // Scans class AST for callable methods annotated with [[=Expose]]
    template <typename T>
    consteval auto Collect_Exposed_Methods()
    {
        constexpr auto context = std::meta::access_context::current();
        const auto members = std::meta::members_of(^^T, context);

        std::vector<std::meta::info> methods;

        for (std::meta::info m : members)
        {
            if (!std::meta::is_function(m) || !std::meta::has_identifier(m))
            {
                continue;
            }

            std::string_view name = std::meta::identifier_of(m);

            // Skip engine virtuals (_ready, _process) and property accessors (Get_*, Set_*)
            if (name.starts_with("_") || name.starts_with("Get_") || name.starts_with("Set_"))
            {
                continue;
            }

            if (Has_Annotation<Callable_Tag>(m))
            {
                methods.push_back(m);
            }
        }

        return methods;
    }

    template <typename T>
    consteval std::size_t Get_Exposed_Method_Count()
    {
        return Collect_Exposed_Methods<T>().size();
    }

    template <typename T>
    consteval auto Get_Exposed_Methods_Array()
    {
        constexpr std::size_t count = Get_Exposed_Method_Count<T>();
        std::array<std::meta::info, count> array{};
        const auto vector = Collect_Exposed_Methods<T>();

        for (std::size_t i = 0; i < count; ++i)
        {
            array[i] = vector[i];
        }

        return array;
    }

    template <std::meta::info Method>
    void Bind_Single_Method()
    {
        constexpr auto method_ptr = &[: Method :];
        constexpr std::string_view method_name = std::meta::identifier_of(Method);

        godot::ClassDB::bind_method(godot::D_METHOD(method_name.data()), method_ptr);
    }

    template <typename T, std::size_t... Is>
    void Bind_Methods_Impl(std::index_sequence<Is...>)
    {
        constexpr auto methods = Get_Exposed_Methods_Array<T>();
        (Bind_Single_Method<methods[Is]>(), ...);
    }

    // Main API entry point: Automatically binds properties and [[=Expose]] methods
    template <typename T>
    void Auto_Bind_Class()
    {
        // 1. Auto-bind properties (Get_*/Set_* pairs)
        constexpr std::size_t property_count = Get_Property_Count<T>();
        if constexpr (property_count > 0)
        {
            Bind_Properties_Impl<T>(std::make_index_sequence<property_count>{});
        }

        // 2. Auto-bind [[=Expose]] callable methods
        constexpr std::size_t method_count = Get_Exposed_Method_Count<T>();
        if constexpr (method_count > 0)
        {
            Bind_Methods_Impl<T>(std::make_index_sequence<method_count>{});
        }
    }
}
//------------------------------------------------------------------------------------------------------------
#else
//------------------------------------------------------------------------------------------------------------
namespace GDExtension_Reflect
{
    template <typename T>
    void Auto_Bind_Class()
    {

    }
}
//------------------------------------------------------------------------------------------------------------
#endif // __clang__
//------------------------------------------------------------------------------------------------------------
