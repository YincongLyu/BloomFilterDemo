#include <iostream>
#include <span>
#include <cstring>

namespace bloomstore
{

inline uint32_t scramble(uint32_t k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}

// copied from MurmurHash
uint32_t hash(std::span<uint8_t> key, uint32_t seed) {
    uint32_t h = seed;
    for (size_t i = key.size() / 4; i != 0; --i) {
        uint32_t k = 0;
        memcpy(&k, &*key.subspan(4*i, 4).begin(), sizeof(uint32_t));
        h ^= scramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }
    uint32_t k = 0;
    for (size_t i = key.size() % 4; i != 0; --i) {
        k <<= 8;
        k |= key[i];
    }
    h ^= scramble(k);
    h ^= key.size();
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

// reproduce another hash func from hash_a and hash_b
uint32_t mangle(uint8_t n, uint32_t hash_a, uint32_t hash_b, uint32_t sz) {
    return (n * hash_a + hash_b) % sz;
}
} // namespace bloomstore