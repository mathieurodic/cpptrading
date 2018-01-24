#include "network/RestClient.hpp"

#include <iostream>


int main(int argc, char const *argv[]) {
    RestClient client("https://jsonplaceholder.typicode.com");
    std::cout << "POST\n";
    std::cout << client.post("/posts", {{"title", "foo"}, {"body", "bar"}, {"userId", 1}}) << "\n\n";
    std::cout << "GET\n";
    std::cout << client.get("/posts/1") << "\n\n";
    std::cout << "PATCH\n";
    std::cout << client.patch("/posts/1", {{"title", "foo"}}) << "\n\n";
    std::cout << "DELETE\n";
    std::cout << client.del("/posts/1") << "\n\n";
    return 0;
}
