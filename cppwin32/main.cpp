#include <winmd_reader.h>
#include "text_writer.h"
#include "type_writers.h"
#include "code_writers.h"

using namespace cppwin32;

struct options
{
    std::string input;
    std::filesystem::path output_folder;
};

int main(int const argc, char* argv[])
{
    // Hack prototype command line args for now
    options o;
    o.input = argv[1];
    o.output_folder = std::filesystem::canonical(argv[2]);
    std::filesystem::create_directories(o.output_folder);

    winmd::reader::cache c{ o.input };

    for (auto const& [ns, members] : c.namespaces())
    {
        if (ns.empty())
        {
            continue;
        }
        writer w;
        w.write("#include \"base.h\"\n");
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
            w.write_each<write_delegate>(members.delegates);
            w.write("#pragma endregion delegates\n\n");
        }
        {
            auto wrap = wrap_impl_namespace(w);

            w.write("#pragma region guids\n");
            w.write_each<write_guid>(members.interfaces);
            w.write("#pragma endregion guids\n\n");

            w.write("#pragma region abi_interfaces\n");
            w.write_each<write_interface_abi>(members.interfaces);
            w.write("#pragma endregion abi_interfaces\n\n");

            w.write("#pragma region consume\n");
            w.write_each<write_consume>(members.interfaces);
            w.write("#pragma endregion consume\n\n");
        }
        {
            auto wrap = wrap_type_namespace(w, ns);

            w.write("#pragma region interfaces\n");
            w.write_each<write_interface>(members.interfaces);
            w.write("#pragma endregion interfaces\n\n");

            w.write("#pragma region structs\n");
            write_structs(w, members.structs);
            w.write("#pragma endregion structs\n\n");
        }
        {
            w.write("#pragma region abi_methods\n");
            w.write_each<write_class_abi>(members.classes);
            w.write("#pragma endregion abi_methods\n\n");
        }
        {
            auto wrap = wrap_type_namespace(w, ns);

            w.write("#pragma region methods\n");
            w.write_each<write_class>(members.classes);
            w.write("#pragma endregion methods\n\n");

            w.write("#pragma region enum_operators\n");
            w.write_each<write_enum_operators>(members.enums);
            w.write("#pragma endregion enum_operators\n\n");
        }

        w.save_header(o.output_folder.string());

        std::filesystem::copy_file("base.h", o.output_folder / "base.h", std::filesystem::copy_options::overwrite_existing);
    }
}
