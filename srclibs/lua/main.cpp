#include "sol/sol.hpp"
#include <iostream>
#include "windows.h"

void clearScreen() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(console, &info);
    DWORD out;
    FillConsoleOutputCharacter(console, ' ', info.dwSize.X * info.dwSize.Y , {0,0}, &out);
    SetConsoleCursorPosition(console, {0,0});
}

void stringReplace(std::string& in, const std::string& find, const std::string& replace) {
    
    size_t pos = 0;
    do {
        pos = in.find(find, pos);
        if(pos == std::string::npos) break;

        in.replace(pos, find.length(), replace);
        pos += replace.length();
    } while(1);

}

int main() {
    sol::state lua;

    lua.open_libraries(sol::lib::base);
    lua.script("function _onUpdate (delta) onUpdate(delta) end");

    sol::function onUpdate = lua["_onUpdate"];

    lua.set_function("cls", &clearScreen);

    lua.set_function("exit", [](){
        exit(0);
    });

    lua.set("test", &clearScreen);

    do {
        onUpdate(double(0));
        std::string data;
        char buf[1024];
        std::cin.getline(buf, 1024);
        data.assign(buf, strlen(buf));

        stringReplace(data, "\\n", "\n");

        auto result = lua.script(data);
        if(!result.valid()){
            std::cout << "bad statement\n";
        } else {
            for(int i=0;i < result.return_count(); ++i){
                switch(result[i].get_type()){
                    case sol::type::number:
                        std::cout << result[i].get<double>();
                    break;
                    case sol::type::string:
                        std::cout << result[i].get<std::string>();
                    break;
                    case sol::type::boolean:
                        std::cout << (result[i].get<bool>() ? "true" : "false");
                    break;
                    case sol::type::function:
                        std::cout << "function";
                    break;
                    case sol::type::none:
                    case sol::type::nil:
                        std::cout << "nil";
                    break;
                }
                std::cout << "\n";
            }
        }

    } while(1);

    return 0;
}