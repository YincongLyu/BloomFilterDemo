#include "bloom_buffer.h"
#include <cstring>
#include <cassert>

#define KSZ (this->key_sz_)
#define VSZ (this->value_sz_)
namespace bloomstore
{

BloomBuffer::BloomBuffer(size_t key_sz, size_t value_sz, size_t capacity) : 
                key_sz_(key_sz), 
                value_sz_(value_sz), 
                capacity_(capacity), 
                tombstone_(capacity_, false),
                kvPairs_(capacity_ * (key_sz_ + value_sz_), 0) {}

void BloomBuffer::put(std::span<uint8_t> key, std::span<uint8_t> value) {
    assert(key.size() == KSZ);
    assert(value.size() == VSZ);
    assert(this->vec_sz_ <= this->capacity_);
    size_t now_idx = vec_sz_;
    tombstone_[now_idx] = false;
    size_t offset = now_idx * (KSZ + VSZ);
    memcpy(&kvPairs_[offset], key.data(), KSZ);
    memcpy(&kvPairs_[offset + KSZ], value.data(), VSZ);

    vec_sz_ += 1;
    return;
}

// del 和 put 唯一的区别是，vec还是追加写，但此时的 tombstone 是true
void BloomBuffer::del(std::span<uint8_t> key) {
    assert(key.size() == KSZ);
    assert(this->vec_sz_ <= this->capacity_);
    size_t now_idx = vec_sz_;
    tombstone_[now_idx] = true;
    memcpy(&kvPairs_[now_idx * (KSZ + VSZ)], key.data(), KSZ);
    return;
}
// 1. 找 key 对应的最新的value，需要倒序遍历 vec，并结合 tombstone 判断是否可读
// std::optional<std::span<uint8_t>> BloomBuffer::get(std::span<uint8_t> key) {
//     for (size_t i = vec_sz_ - 1; i >= 0; --i) {
//         // 按字节粒度比较
//         if (memcmp(&kvPairs_[i * (KSZ + VSZ)], &key, KSZ) == 0) {
//             // 存在但被删除
//             if (tombstone_[i]) {
//                 return std::nullopt;
//             } else {
//                 std::span<uint8_t> value = std::span{kvPairs_}.subspan(i * (KSZ + VSZ) + KSZ, VSZ);
//                 return std::optional{value};
//             }
//         } else { // not equal
//             continue;
//         }
//     }
//     // key 不存在
//     return std::nullopt;
// }

void BloomBuffer::get(std::span<uint8_t> key, std::span<uint8_t> value, bool& is_found, bool& is_tombstone) { 
    for (size_t i = this->vec_sz_ - 1; i >= 0; --i) {
        size_t offset = i * (KSZ + VSZ);
        if (0 == memcmp(&this->kvPairs_[offset], key.data(), KSZ)) {
            if (this->tombstone_[i]) {
                is_tombstone = true;
            } else {
                is_tombstone = false;
                is_found = true;
                memcmp(&value, &this->kvPairs_[offset + VSZ], VSZ);
            }
        }
    }
    is_found = false;
}

} // namespace bloomstore