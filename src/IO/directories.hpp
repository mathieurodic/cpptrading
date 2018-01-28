#ifndef CTRADING__IO__FOLDERS__HPP
#define CTRADING__IO__FOLDERS__HPP


#include <experimental/filesystem>
// #include <filesystem>

#include <exceptions/Exception.hpp>


const inline void make_directory(std::experimental::filesystem::path path) {
    path = std::experimental::filesystem::absolute(path);
    if (!std::experimental::filesystem::is_directory(path)) {
        if (std::experimental::filesystem::exists(path)) {
            throw FileException("Could not create directory `" + path.string() + "`, path exists as something else than a directory");
        }
        std::experimental::filesystem::path parent_path = path.parent_path();
        if (path.string().size()) {
            make_directory(path.parent_path());
        }
        std::experimental::filesystem::create_directory(path);
    }
}

const std::string extract_directory(std::string path) {
    return std::experimental::filesystem::absolute(path).parent_path();
}


#endif // CTRADING__IO__FOLDERS__HPP
