#include <vector>
#include <span>
#include <cstdint>
#include <array>
#include <optional>

namespace bloomstore
{

class BloomBuffer {
public:
    BloomBuffer(size_t key_sz, size_t value_sz, size_t capacity);
    
    void put(std::span<uint8_t> key, std::span<uint8_t> value);
    void del(std::span<uint8_t> key);
    // maybe not in buffer
    // std::optional<std::span<uint8_t>> get(std::span<uint8_t> key);
    void get(std::span<uint8_t> key, std::span<uint8_t> value, bool& is_found, bool& is_tombstone);

private:
    std::vector<bool> tombstone_;
    std::vector<uint8_t> kvPairs_; /*连续的一段内存空间，是一个字节数组，需严格按 byte 寻址*/
    
    size_t key_sz_;
    size_t value_sz_;
    size_t vec_sz_;  /*逻辑大小*/
    size_t capacity_;
};

} // namespace bloomstore