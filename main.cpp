#include <unistd.h>
#include <fcntl.h>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <cerrno>

int main() {
    // 这台环境不支持 O_DIRECT 操作
    int32_t fd = open("./test-file", O_RDWR | O_CREAT | O_SYNC | O_APPEND, 0600);
    if (fd == -1) {
        std::cerr << "Failed to open file: " << strerror(errno) << std::endl;
        return 1;
    }

    auto s = std::string{""};
    for (int i = 0; i < 1024; ++i) {
        s += "y";
    }

    int32_t bytes = write(fd, s.c_str(), s.size());
    if (bytes == -1) {
        std::cerr << "Failed to write to file: " << strerror(errno) << std::endl;
    } else {
        std::cerr << bytes << std::endl;
    }
    
    close(fd);
    return 0;
}