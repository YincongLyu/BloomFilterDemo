#include<cstdint>
#include<string>
#include<span>

class FileObject {
public:
    FileObject(std::string&& path);
    void Append(std::span<uint8_t> bytes);
    void Read(size_t position, std::span<uint8_t> bytes);

private:
    std::string path;
    int32_t fd;
};
