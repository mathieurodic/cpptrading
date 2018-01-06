#include <IO/directories.hpp>

#include <iostream>


int main(int argc, char const *argv[]) {
    const std::string path1 = "/tmp/tests/truc.btree";
    const std::string path2 = "/tmp//test/azerty//truc.btree";
    const std::string path3 = "tmp/test/azerty//truc.btree";

    for (const std::string path : {path1, path2, path3}) {
        std::cout << path << "   ->   " << extract_directory(path) << '\n';
        make_directory(extract_directory(path));
    }


    return 0;
}
