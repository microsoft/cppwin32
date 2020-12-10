#pragma once

#include "type_writers.h"

namespace cppwin32
{
    struct separator
    {
        writer& w;
        bool first{ true };

        void operator()()
        {
            if (first)
            {
                first = false;
            }
            else
            {
                w.write(", ");
            }
        }
    };

    struct method_signature
    {
        explicit method_signature(MethodDef const& method) :
            m_method(method),
            m_signature(method.Signature())
        {
            auto params = method.ParamList();

            if (m_signature.ReturnType() && params.first != params.second && params.first.Sequence() == 0)
            {
                m_return = params.first;
                ++params.first;
            }

            for (uint32_t i{}; i != size(m_signature.Params()); ++i)
            {
                m_params.emplace_back(params.first + i, &m_signature.Params().first[i]);
            }
        }

        std::vector<std::pair<Param, ParamSig const*>>& params()
        {
            return m_params;
        }

        std::vector<std::pair<Param, ParamSig const*>> const& params() const
        {
            return m_params;
        }

        auto const& return_signature() const
        {
            return m_signature.ReturnType();
        }

        auto return_param_name() const
        {
            std::string_view name;

            if (m_return && !m_return.Name().empty())
            {
                name = m_return.Name();
            }
            else
            {
                name = "win32_impl_result";
            }

            return name;
        }

        auto return_param() const
        {
            return m_return;
        }

        MethodDef const& method() const
        {
            return m_method;
        }

    private:

        MethodDef m_method;
        MethodDefSig m_signature;
        std::vector<std::pair<Param, ParamSig const*>> m_params;
        Param m_return;
    };

    enum class param_category
    {
        enum_type,
        struct_type,
        array_type,
        fundamental_type,
        interface_type,
        raw_interface_type, // not COM
        delegate_type,
        generic_type
    };

    bool is_raw_interface(TypeDef const& type)
    {
        //auto const& nested_types = type.get_cache().nested_types(type);
        //if (size(nested_types) < 4)
        //{
        //    return true;
        //}
        //auto const& vtbl = nested_types.back();
        //auto const field_list = vtbl.FieldList();
        //return field_list.first.Name() != "QueryInterface";
        auto attr = get_attribute(type, "Microsoft.Windows.Sdk.Win32.Interop", "NativeInheritanceAttribute");
        if (!attr)
        {
            return true;
        }
        auto const signature = attr.Value();
        auto const base_name = std::get<std::string_view>(std::get<ElemSig>(signature.FixedArgs()[0].value).value);
        if (base_name == "IUnknown")
        {
            return false;
        }
        auto const base_type = type.get_cache().find_required("Microsoft.Windows.Sdk.Win32", base_name);
        return is_raw_interface(base_type);
    }

    inline param_category get_category(TypeSig const& signature, TypeDef* signature_type = nullptr)
    {
        if (signature.is_szarray())
        {
            return param_category::array_type;
        }

        param_category result{};

        call(signature.Type(),
            [&](ElementType type)
            {
                result = param_category::fundamental_type;
            },
            [&](coded_index<TypeDefOrRef> const& type)
            {
                TypeDef type_def;
                if (type.type() == TypeDefOrRef::TypeDef)
                {
                    type_def = type.TypeDef();
                }
                else
                {
                    auto type_ref = type.TypeRef();
                    if (type_name(type_ref) == "System.Guid")
                    {
                        result = param_category::struct_type;
                        return;
                    }
                    type_def = find_required(type_ref);
                }

                if (signature_type)
                {
                    *signature_type = type_def;
                }

                switch (get_category(type_def))
                {
                case category::interface_type:
                    result = is_raw_interface(type_def) ? param_category::raw_interface_type : param_category::interface_type;
                    return;
                case category::enum_type:
                    result = param_category::enum_type;
                    return;
                case category::struct_type:
                    result = param_category::struct_type;
                    return;
                case category::delegate_type:
                    result = param_category::delegate_type;
                    return;
                default:
                    result = param_category::generic_type;
                    return;
                }
            },
                [&](auto&&)
            {
                result = param_category::generic_type;
            });
        return result;
    }
}