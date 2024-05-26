#include <cstdint>
#include <span>
#include <vector>
namespace bloomstore
{

class BloomFilter {
public:
    BloomFilter(uint32_t slot_num, uint8_t nfunc);
    // simulate put(key)
    void insert(std::span<uint8_t> key); 
    // test if key exist
    bool test(std::span<uint8_t> key);

private:
    std::vector<bool> slot_vec_;
    uint32_t slot_num_;
    uint8_t nfunc_;

};

} // namespace bloomstore