#include <cstdint>
#include <span>
#include <vector>
namespace bloomstore
{
class BloomFilter;
class BloomFilterChain;
class ChainIterator;

/**
 * @brief use set to represent bloom filter
 * 
 */
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
    friend BloomFilterChain;
};

/**
 * @brief bloom filters can run in parallel
 * 
 */
class BloomFilterChain {
public:
    BloomFilterChain(uint32_t slot_num, uint8_t nfunc);
    void join(BloomFilter&& new_filter, size_t new_block_addr);
    ChainIterator* test(std::span<uint8_t> key);


private:
    std::vector<size_t> block_addr_;
    std::vector<uint64_t> matrix_;
    size_t chain_len_;
    uint8_t nfunc_;
};

class ChainIterator {
public:
    ChainIterator(std::vector<size_t>& block_addr, uint64_t bitmask, uint8_t progress);
    void next(size_t& addr, bool& is_depleted);


private:
    std::vector<size_t>& block_addr_;
    uint64_t bitmask_;
    uint8_t progress_;
    friend BloomFilterChain;
};

} // namespace bloomstore