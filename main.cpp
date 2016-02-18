#include "Database.hpp"

struct asdasd {
    int a;
    std::string b;
};

int main() {
    storage::Database db("./testbase");
    int a = 10, b = 178;
    db.put(a, b);
    db.put(b, asdasd{a, "asdasd"});
    std::cout << db.get<int, int>(10) << std::endl;
    auto&& asd = db.get<int, asdasd>(b);
    std::cout << asd.a << " " << asd.b << std::endl;
    return 0;
}