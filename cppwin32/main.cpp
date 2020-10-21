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
            w.write_each<write_enum>(members.enums);
            w.write_each<write_forward>(members.structs);
            w.write_each<write_forward>(members.interfaces);
            w.write_each<write_delegate>(members.delegates);

            write_structs(w, members.structs);
        }
        {
            w.write_each<write_class_abi>(members.classes);
        }
        {
            auto wrap = wrap_type_namespace(w, ns);
            w.write_each<write_class>(members.classes);
            w.write_each<write_enum_operators>(members.enums);
        }

        w.save_header(o.output_folder.string());
    }
}
