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

    struct writer : writer_base<writer>
    {
        using writer_base<writer>::write;
        std::string type_namespace;

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
            write("@::%", type.TypeNamespace(), type.TypeName());
        }

        void write(TypeRef const& type)
        {
            if (type.TypeNamespace() == "System" && type.TypeName() == "Guid")
            {
                write("GUID");
            }
            else
            {
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
                    case ElementType::I:
                    case ElementType::U:
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
                },
                [&](auto&& type)
                {
                    throw std::invalid_argument("Invalid TypeSig type");
                });
        }

        void save_header(std::string const& output_folder)
        {
            auto filename{ output_folder + '/' };
            filename += type_namespace + ".h";
            flush_to_file(filename);
        }
    };
}