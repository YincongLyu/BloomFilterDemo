#include<iostream>

namespace bloomstore
{
// directly use murmured hash algorithm
uint32_t hash(std::span<uint8_t> key, uint32_t seed);
uint32_t mangle(uint8_t n, uint32_t hash_a, uint32_t hash_b, uint32_t sz);

} // namespace bloomstore