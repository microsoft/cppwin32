#pragma once

#include "type_writers.h"
#include "helpers.h"

namespace cppwin32
{
    struct finish_with
    {
        writer& w;
        void (*finisher)(writer&);

        finish_with(writer& w, void (*finisher)(writer&)) : w(w), finisher(finisher) {}
        finish_with(finish_with const&) = delete;
        void operator=(finish_with const&) = delete;

        ~finish_with() { finisher(w); }
    };

    void write_include_guard(writer& w)
    {
        auto format = R"(#pragma once
)";

        w.write(format);
    }

    void write_close_namespace(writer& w)
    {
        auto format = R"(}
)";

        w.write(format);
    }

    [[nodiscard]] static finish_with wrap_impl_namespace(writer& w)
    {
        auto format = R"(namespace win32::impl
{
)";

        w.write(format);

        return { w, write_close_namespace };
    }

    [[nodiscard]] finish_with wrap_type_namespace(writer& w, std::string_view const& ns)
    {
        // TODO: Move into forwards
        auto format = R"(WIN32_EXPORT namespace win32::@
{
)";

        w.write(format, ns);

        return { w, write_close_namespace };
    }

    void write_enum_field(writer& w, Field const& field)
    {
        auto format = R"(        % = %,
)";

        if (auto constant = field.Constant())
        {
            w.write(format, field.Name(), *constant);
        }
    }

    void write_enum(writer& w, TypeDef const& type)
    {
        auto format = R"(    enum class % : %
    {
%    };
)";

        auto fields = type.FieldList();
        w.write(format, type.TypeName(), fields.first.Signature().Type(), bind_each<write_enum_field>(fields));
    }

    void write_forward(writer& w, TypeDef const& type)
    {
        auto format = R"(    struct %;
)";

        w.write(format, type.TypeName());
    }

    struct struct_field
    {
        std::string_view name;
        std::string type;
        std::optional<int32_t> array_count;
    };

    void write_struct_field(writer& w, struct_field const& field)
    {
        if (field.array_count)
        {
            w.write("        @ %[%];\n",
                field.type, field.name, field.array_count.value());
        }
        else
        {
            w.write("        @ %;\n",
                field.type, field.name);
        }
    }

    void write_struct(writer& w, TypeDef const& type)
    {
        auto format = R"(    struct %
    {
%    };
)";
        auto const name = type.TypeName();
        struct complex_struct
        {
            complex_struct(writer& w, TypeDef const& type)
                : type(type)
            {
                fields.reserve(size(type.FieldList()));
                for (auto&& field : type.FieldList())
                {
                    auto const name = field.Name();
                    auto field_type = field.Signature().Type();
                    std::optional<int32_t> array_count;
                    
                    if (auto nested_type = get_nested_type(field_type, type.get_database()))
                    {
                        auto buffer_attribute = get_attribute(field, "System.Runtime.CompilerServices", "FixedBufferAttribute");
                        if (buffer_attribute)
                        {
                            auto const& sig = buffer_attribute.Value();
                            if (sig.FixedArgs().size() != 2)
                            {
                                throw std::invalid_argument("FixedBufferAttribute should have 2 args");
                            }
                            array_count = std::get<int32_t>(std::get<ElemSig>(sig.FixedArgs()[1].value).value);
                            auto nested_type = std::get<coded_index<TypeDefOrRef>>(field_type.Type());
                            field_type = nested_type.TypeDef().FieldList().first.Signature().Type();
                        }
                        else if (nested_type.TypeName().find("__FixedBuffer") != std::string_view::npos)
                        {
                            array_count = static_cast<int32_t>(size(nested_type.FieldList()));
                            field_type = nested_type.FieldList().first.Signature().Type();
                        }
                        else if (nested_type.Flags().Layout() == TypeLayout::ExplicitLayout && nested_type.TypeName().find("_e__Union") != std::string_view::npos)
                        {
                            // TODO: unions
                            continue;
                        }
                        else if (nested_type.Flags().Layout() == TypeLayout::ExplicitLayout && nested_type.TypeName().find("_e__Struct") != std::string_view::npos)
                        {
                            // TODO: unions
                            continue;
                        }
                    }
                    
                    fields.push_back({ name, w.write_temp("%", field_type), array_count });
                }
            }

            static TypeDef get_nested_type(TypeSig const& sig, database const& db)
            {
                auto possible_nested_type = std::get_if<coded_index<TypeDefOrRef>>(&sig.Type());
                if (!possible_nested_type)
                {
                    return {};
                }

                if (possible_nested_type->type() != TypeDefOrRef::TypeDef)
                {
                    return {};
                }

                auto type = possible_nested_type->TypeDef();
                auto range = equal_range(db.NestedClass, type);
                if (empty(range))
                {
                    return {};
                }
                else
                {
                    return type;
                }
            }

            TypeDef type;
            std::vector<struct_field> fields;
        };

        complex_struct s{ w, type };

        w.write(format, type.TypeName(), bind_each<write_struct_field>(s.fields));
    }

    void write_abi_params(writer& w, method_signature const& method_signature)
    {
        separator s{ w };
        for (auto&& [param, param_signature] : method_signature.params())
        {
            s();
            std::string type;
            if (param.Flags().HasFieldMarshal())
            {
                auto fieldMarshal = param.FieldMarshal();
                switch (fieldMarshal.Signature())
                {
                case NativeType::Lpstr:
                    if (param.Flags().In())
                    {
                        type = "const char*";
                    }
                    else
                    {
                        type = "char*";
                    }
                    break;

                case NativeType::Lpwstr:
                    if (param.Flags().In())
                    {
                        type = "const wchar_t*";
                    }
                    else
                    {
                        type = "wchar_t*";
                    }
                    break;

                default:
                    type = w.write_temp("%", param_signature->Type());
                    break;
                }
            }
            else
            {
                type = w.write_temp("%", param_signature->Type());
            }
            w.write("% %", type, param.Name());
        }
    }

    void write_abi_return(writer& w, RetTypeSig const& sig)
    {
        if (sig)
        {
            w.write(sig.Type());
        }
        else
        {
            w.write("void");
        }
    }

    int get_param_size(ParamSig const& param)
    {
        if (auto e = std::get_if<ElementType>(&param.Type().Type()))
        {
            if (param.Type().ptr_count() == 0)
            {
                switch (*e)
                {
                case ElementType::U8:
                case ElementType::I8:
                case ElementType::R8:
                    return 8;

                default:
                    return 4;
                }
            }
            else
            {
                return 4;
            }
        }
        else
        {
            return 4;
        }
    }

    void write_abi_link(writer& w, method_signature const& method_signature)
    {
        int count = 0;
        for (auto&& [param, param_signature] : method_signature.params())
        {
            count += get_param_size(*param_signature);
        }
        w.write("%, %", method_signature.method().Name(), count);
    }

    void write_class_abi(writer& w, TypeDef const& type)
    {
        w.write(R"(extern "C"
{
)");
        auto const format = R"xyz(    % __stdcall WIN32_IMPL_%(%) noexcept;
)xyz";
        for (auto&& method : type.MethodList())
        {
            if (method.Flags().Access() == MemberAccess::Public)
            {
                method_signature signature{ method };
                w.write(format, bind<write_abi_return>(signature.return_signature()), method.Name(), bind<write_abi_params>(signature));
            }
        }
        w.write(R"(}
)");

        for (auto&& method : type.MethodList())
        {
            if (method.Flags().Access() == MemberAccess::Public)
            {
                method_signature signature{ method };
                w.write("WIN32_IMPL_LINK(%)\n", bind<write_abi_link>(signature));
            }
        }
        w.write("\n");
    }

    void write_class_method(writer& w, MethodDef const& method)
    {

    }

    void write_class(writer& w, TypeDef const& type)
    {

    }
}