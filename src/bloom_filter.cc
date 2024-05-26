#include <bloom_filter.h>
#include <hash_set.h>
#include <span>
#include <cstring>

namespace bloomstore
{

// 初始化一个 bloomfilter，固定 slot_num 个 slot，指定hash函数是 seed
BloomFilter::BloomFilter(uint32_t slot_num, uint8_t nfunc) : slot_num_(slot_num), nfunc_(nfunc) {
    slot_vec_.resize(slot_num_);
}

void BloomFilter::insert(std::span<uint8_t> key) {
    uint32_t hash_a = hash(key, static_cast<uint32_t>('l'));
    uint32_t hash_b = hash(key, static_cast<uint32_t>('y'));
    for (size_t i = 0; i < nfunc_; i++) {
        uint32_t hash_z = mangle(i, hash_a, hash_b, slot_num_);
        slot_vec_[hash_z] = true;
    }
}
// do the same as insert, test nfunc_ slot whether is true
bool BloomFilter::test(std::span<uint8_t> key) {
    uint32_t hash_a = hash(key, static_cast<uint32_t>('l'));
    uint32_t hash_b = hash(key, static_cast<uint32_t>('y'));
    bool exit = true;
    for (size_t i = 0; i < nfunc_; i++) {
        uint32_t hash_z = mangle(i, hash_a, hash_b, slot_num_);
        exit &= slot_vec_[hash_z];
    }
    return exit;
}

}