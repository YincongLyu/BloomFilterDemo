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
    std::optional<std::span<uint8_t>> get(std::span<uint8_t> key);

private:
    std::vector<bool> tombstone_;
    std::vector<uint8_t> kvPairs_;
    
    size_t key_sz_;
    size_t value_sz_;
    size_t vec_sz_;
    size_t capacity_;
};

} // namespace bloomstore