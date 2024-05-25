#include <bloom_filter.h>
#include <span>
#include <cstring>

namespace bloomstore
{

// 初始化一个 bloomfilter，固定 slot_num 个 slot，指定hash函数是 seed
BloomFilter::BloomFilter(uint32_t slot_num, uint8_t nfunc) : slot_num_(slot_num), nfunc_(nfunc) {
    slot_vec_.resize(slot_num_);
}

void BloomFilter::insert(std::span<uint8_t> key) {
    uint32_t hash_a = BloomFilter::hash(key, static_cast<uint32_t>('l'));
    uint32_t hash_b = BloomFilter::hash(key, static_cast<uint32_t>('y'));
    for (size_t i = 0; i < nfunc_; i++) {
        uint32_t hash_z = BloomFilter::mangle(i, hash_a, hash_b, slot_num_);
        slot_vec_[hash_z] = true;
    }
}
// do the same as insert, test nfunc_ slot whether is true
bool BloomFilter::test(std::span<uint8_t> key) {
    uint32_t hash_a = BloomFilter::hash(key, static_cast<uint32_t>('l'));
    uint32_t hash_b = BloomFilter::hash(key, static_cast<uint32_t>('y'));
    bool exit = true;
    for (size_t i = 0; i < nfunc_; i++) {
        uint32_t hash_z = BloomFilter::mangle(i, hash_a, hash_b, slot_num_);
        exit &= slot_vec_[hash_z];
    }
    return exit;
}

inline uint32_t Scramble(uint32_t k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}

// copied from MurmurHash
uint32_t BloomFilter::hash(std::span<uint8_t> key, uint32_t seed) {
    uint32_t h = seed;
    for (size_t i = key.size() / 4; i != 0; --i) {
        uint32_t k = 0;
        memcpy(&k, &*key.subspan(4*i, 4).begin(), sizeof(uint32_t));
        h ^= Scramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }
    uint32_t k = 0;
    for (size_t i = key.size() % 4; i != 0; --i) {
        k <<= 8;
        k |= key[i];
    }
    h ^= Scramble(k);
    h ^= key.size();
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}



// reproduce another hash func from hash_a and hash_b
uint32_t BloomFilter::mangle(uint8_t n, uint32_t hash_a, uint32_t hash_b, uint32_t sz) {
    return (n * hash_a + hash_b) % sz;
}

}