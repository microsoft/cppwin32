#pragma once

namespace cppwin32
{
    static void write_namespace_0_h(std::string_view const& ns, cache::namespace_members const& members)
    {
        writer w;
        w.type_namespace = ns;

        {
            auto wrap = wrap_type_namespace(w, ns);

            w.write("#pragma region enums\n");
            w.write_each<write_enum>(members.enums);
            w.write("#pragma endregion enums\n\n");

            w.write("#pragma region forward_declarations\n");
            w.write_each<write_forward>(members.structs);
            w.write_each<write_forward>(members.interfaces);
            w.write("#pragma endregion forward_declarations\n\n");

            w.write("#pragma region delegates\n");
            write_delegates(w, members.delegates);
            w.write("#pragma endregion delegates\n\n");
        }
        {
            auto wrap = wrap_impl_namespace(w);

            w.write("#pragma region guids\n");
            w.write_each<write_guid>(members.interfaces);
            w.write("#pragma endregion guids\n\n");
        }

        write_close_file_guard(w);
        w.swap();

        write_preamble(w);
        write_open_file_guard(w, ns, '0');

        w.save_header('0');
    }

    static void write_namespace_1_h(std::string_view const& ns, cache::namespace_members const& members)
    {
        writer w;
        w.type_namespace = ns;

        {
            auto wrap = wrap_type_namespace(w, ns);

            w.write("#pragma region structs\n");
            write_structs(w, members.structs);
            w.write("#pragma endregion structs\n\n");

            w.write("#pragma region interfaces\n");
            write_interfaces(w, members.interfaces);
            w.write("#pragma endregion interfaces\n\n");
        }

        write_close_file_guard(w);
        w.swap();
        write_preamble(w);
        write_open_file_guard(w, ns, '1');

        for (auto&& depends : w.depends)
        {
            w.write_depends(depends.first, '1');
        }

        w.write_depends(w.type_namespace, '0');
        w.save_header('1');
    }

    static void write_namespace_2_h(std::string_view const& ns, cache::namespace_members const& members)
    {
        writer w;
        w.type_namespace = ns;

        write_close_file_guard(w);
        w.swap();
        write_preamble(w);
        write_open_file_guard(w, ns, '2');

        w.write_depends(w.type_namespace, '1');
        w.save_header('2');
    }

    static void write_namespace_h(std::string_view const& ns, cache::namespace_members const& members)
    {
        writer w;
        w.type_namespace = ns;

        write_close_file_guard(w);
        w.swap();
        write_preamble(w);
        write_open_file_guard(w, ns, '2');
        write_version_assert(w);

        w.write_depends(w.type_namespace, '2');
        w.save_header();
    }
}