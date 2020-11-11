#pragma once

#include "text_writer.h"
#include <winmd_reader.h>

namespace cppwin32
{
    using namespace winmd::reader;

    template <typename...T> struct visit_overload : T... { using T::operator()...; };

    template <typename V, typename...C>
    auto call(V&& variant, C&&...call)
    {
        return std::visit(visit_overload<C...>{ std::forward<C>(call)... }, std::forward<V>(variant));
    }

    struct type_name
    {
        std::string_view name;
        std::string_view name_space;

        explicit type_name(TypeDef const& type) :
            name(type.TypeName()),
            name_space(type.TypeNamespace())
        {
        }

        explicit type_name(TypeRef const& type) :
            name(type.TypeName()),
            name_space(type.TypeNamespace())
        {
        }

        explicit type_name(coded_index<TypeDefOrRef> const& type)
        {
            auto const& [type_namespace, type_name] = get_type_namespace_and_name(type);
            name_space = type_namespace;
            name = type_name;
        }
    };

    bool operator==(type_name const& left, type_name const& right)
    {
        return left.name == right.name && left.name_space == right.name_space;
    }

    bool operator==(type_name const& left, std::string_view const& right)
    {
        if (left.name.size() + 1 + left.name_space.size() != right.size())
        {
            return false;
        }

        if (right[left.name_space.size()] != '.')
        {
            return false;
        }

        if (0 != right.compare(left.name_space.size() + 1, left.name.size(), left.name))
        {
            return false;
        }

        return 0 == right.compare(0, left.name_space.size(), left.name_space);
    }

    template <typename First, typename... Rest>
    auto get_impl_name(First const& first, Rest const&... rest)
    {
        std::string result;
        result.reserve(std::string_view(first).size() + ((1 + std::string_view(rest).size()), ...));
        ((result += first), (((result += '_') += rest), ...));
        std::transform(result.begin(), result.end(), result.begin(), [](char c)
            {
                return c == '.' ? '_' : c;
            });
        return result;
    }

    struct writer : writer_base<writer>
    {
        using writer_base<writer>::write;
        std::string type_namespace;
        bool abi_types{};
        bool full_namespace{};
        bool consume_types{};

        template<typename T>
        struct member_value_guard
        {
            writer* const owner;
            T writer::* const member;
            T const previous;
            explicit member_value_guard(writer* arg, T writer::* ptr, T value) :
                owner(arg), member(ptr), previous(std::exchange(owner->*member, value))
            {
            }

            ~member_value_guard()
            {
                owner->*member = previous;
            }

            member_value_guard(member_value_guard const&) = delete;
            member_value_guard& operator=(member_value_guard const&) = delete;
        };

        [[nodiscard]] auto push_abi_types(bool value)
        {
            return member_value_guard(this, &writer::abi_types, value);
        }

        [[nodiscard]] auto push_full_namespace(bool value)
        {
            return member_value_guard(this, &writer::full_namespace, value);
        }

        [[nodiscard]] auto push_consume_types(bool value)
        {
            return member_value_guard(this, &writer::consume_types, value);
        }

        void write_value(int32_t value)
        {
            write_printf("%d", value);
        }

        void write_value(uint32_t value)
        {
            write_printf("%#0x", value);
        }

        void write_code(std::string_view const& value)
        {
            for (auto&& c : value)
            {
                if (c == '.')
                {
                    write("::");
                }
                else
                {
                    write(c);
                }
            }
        }

        void write(Constant const& value)
        {
            switch (value.Type())
            {
            case ConstantType::Int32:
                write_value(value.ValueInt32());
                break;
            case ConstantType::UInt32:
                write_value(value.ValueUInt32());
                break;
            default:
                throw std::invalid_argument("Unexpected constant type");
            }
        }

        void write(TypeDef const& type)
        {
            if (abi_types && get_category(type) == category::interface_type)
            {
                write("void");
            }
            else
            {
                if (full_namespace)
                {
                    write("win32::");
                }
                write("@::%", type.TypeNamespace(), type.TypeName());
            }
        }

        void write(TypeRef const& type)
        {
            if (type.TypeNamespace() == "System" && type.TypeName() == "Guid")
            {
                write("win32::guid");
            }
            else
            {
                if (full_namespace)
                {
                    write("win32::");
                }
                write("@::%", type.TypeNamespace(), type.TypeName());
            }
        }

        void write(coded_index<TypeDefOrRef> const& type)
        {
            switch (type.type())
            {
            case TypeDefOrRef::TypeDef:
                write(type.TypeDef());
                break;
            case TypeDefOrRef::TypeRef:
                write(type.TypeRef());
                break;
            default:
                throw std::invalid_argument("Unexpected TypeDefOrRef type");
            }
        }

        void write(TypeSig const& signature)
        {
            call(signature.Type(),
                [&](ElementType type)
                {
                    switch (type)
                    {
                    case ElementType::Boolean:
                        write("bool");
                        break;
                    case ElementType::I1:
                        write("int8_t");
                        break;
                    case ElementType::U1:
                        write("uint8_t");
                        break;
                    case ElementType::I2:
                        write("int16_t");
                        break;
                    case ElementType::U2:
                        write("uint16_t");
                        break;
                    case ElementType::I4:
                        write("int32_t");
                        break;
                    case ElementType::U4:
                        write("uint32_t");
                        break;
                    case ElementType::I8:
                        write("int64_t");
                        break;
                    case ElementType::U8:
                        write("uint64_t");
                        break;
                    case ElementType::R4:
                        write("float");
                        break;
                    case ElementType::R8:
                        write("double");
                        break;
                    case ElementType::U:
                        write("size_t");
                        break;
                    case ElementType::I:
                        write("void*");
                        break;
                    case ElementType::Void:
                        write("void");
                        break;
                    default:
                        throw std::invalid_argument("Invalid TypeSig type");
                    }
                    for (int i = 0; i < signature.ptr_count(); ++i)
                    {
                        write('*');
                    }
                },
                [&](coded_index<TypeDefOrRef> const& type)
                {
                    write(type);
                    if (!consume_types)
                    {
                        for (int i = 0; i < signature.ptr_count(); ++i)
                        {
                            write('*');
                        }
                    }
                },
                [&](auto&& type)
                {
                    throw std::invalid_argument("Invalid TypeSig type");
                });
        }

        void write(RetTypeSig const& value)
        {
            if (value)
            {
                write(value.Type());
            }
            else
            {
                write("void");
            }
        }

        void save_header(std::string const& output_folder)
        {
            auto filename{ output_folder + '/' };
            filename += type_namespace + ".h";
            flush_to_file(filename);
        }
    };
}