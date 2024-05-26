#include <bloom_filter.h>
#include <hash_set.h>
#include <span>
#include <cstring>
#include <cassert>

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

/**
 * @brief Construct a new Chain Iterator:: Chain Iterator object
 * 
 * @param block_addr referred from block address
 * @param bitmask 用来判断当前的 key 是否在 bloom chain里
 * @param progress 当前迭代的进度
 */
ChainIterator::ChainIterator(std::span<size_t> block_addr, 
    uint64_t bitmask, uint8_t progress) :
block_addr_(block_addr), bitmask_(bitmask), progress_(progress) {}

/**
 * @brief get next block address
 * 
 * @param addr given address by called
 * @param is_depleted 判断是否还可以再继续迭代
 */
void ChainIterator::next(size_t& addr, bool& is_depleted) {
    // 遍历完了所有 bloom filter，也对应所有的 block
    if (this->progress_ == block_addr_.size()) {
        is_depleted = true;
        return;
    }
    // 检查当前 bitmask_ 的progress进度
    auto check_pos = this->block_addr_.size() - this->progress_ - 1;
    if ((this->bitmask_) & (uint64_t{1} << check_pos)) {
        addr = this->block_addr_[this->progress_];
        is_depleted = false;
        this->progress_ += 1;
        return;
    } else { // 不存在，还要继续找
        this->progress_ += 1;
        return next(addr, is_depleted);
    }
}

BloomFilterChain::BloomFilterChain(uint32_t slot_num, uint8_t nfunc, size_t align) : 
    space(0),
    chain_len_(0),
    nfunc_(nfunc)
{}
/**
 * @brief enabled batch operate
 * 
 * @param filter a new bloom filter
 * @param block_addr its block addrress 
 */
void BloomFilterChain::join(BloomFilter&& new_filter, size_t new_block_addr) {
    assert(chain_len_ < 64);
    for (size_t i = 0; i < new_filter.slot_num_; ++i) {
        if (new_filter.slot_vec_[i]) {
            this->matrix_[i] |= (1 << (63 - this->chain_len_));
        }
    }
    this->block_addr_[this->chain_len_] = new_block_addr;
    this->chain_len_ += 1;
}

ChainIterator* BloomFilterChain::test(std::span<uint8_t> key) {
    uint32_t hash_a = hash(key, static_cast<uint32_t>('l'));
    uint32_t hash_b = hash(key, static_cast<uint32_t>('y'));
    // before : bool exist = true; 现在用 uint64_t 类型 表示 真值
    uint64_t exist = ~uint64_t{0};
    for (size_t i = 0; i < nfunc_; i++) {
        uint32_t hash_z = mangle(i, hash_a, hash_b, matrix_.size());
        // 注意：这里和单个 bloom filter检测一个key是否存在的区别，一个key可以同时在多个bloomfilter里检查是否存在
        exist &= this->matrix_[i];
    }
    return new ChainIterator(this->block_addr_, exist, 0);
}

}