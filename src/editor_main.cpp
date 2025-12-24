#include "editor.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Editor editor;
    
    if (!editor.initialize()) {
        std::cerr << "Failed to initialize editor" << std::endl;
        return 1;
    }
    
    editor.run();
    
    return 0;
}
