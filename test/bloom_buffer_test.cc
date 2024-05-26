#include <gtest/gtest.h>
#include "bloom_buffer.h"
#include "xorshift32.h"
#include <iostream>

#define K 4
#define V 4
#define C 256
#define ARR std::array<uint8_t, 4>

/**
 * 随机操作 total_operaters 次，每次可能是 put / del / get，根据 随机32位数字生成器取模 % 3 定义
 * 
*/
struct ArrayHasher {
    std::size_t operator()(const std::array<uint8_t, 4>& arr) const {
        std::size_t hash = 0;
        for (uint8_t byte : arr) {
            hash ^= std::hash<uint8_t>{}(byte) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

ARR to_arr(uint32_t value) {
    ARR now_value = ARR();
    memcpy(&now_value, &value, sizeof (value));
    return now_value;
}

uint32_t to_uint32(ARR& value) {
    uint32_t tmp = 0xffffffff;
    memcpy(&tmp, &value, sizeof (uint32_t));
    return tmp;
}

TEST(BFBufferTest, ReadWriteTest) {
    std::cout << "=======enter test====\n";
    auto bf_buffer = bloomstore::BloomBuffer(K, V, C);
    std::cout << "====initial bf_buffer failed=========\n";
    auto ground_truth = std::unordered_map<ARR, ARR, ArrayHasher>();
    //  std::cout << "======================\n";
    auto random_number_generator = XorShift32(6);
    int total_operaters = 10;
    int key_range_sz = 32;
    for (size_t i = 0; i < total_operaters; ++i) {
        auto op_type = random_number_generator.sample() % 3;
        auto key = to_arr(random_number_generator.sample() % key_range_sz);
        auto value = to_arr(random_number_generator.sample());

        std::cout << "Operation: " << op_type << ", Key: " << to_uint32(key) << ", Value: " << to_uint32(value) << std::endl;
        switch (op_type)
        {
            case 0: { // put
                std::cout << "Put operation" << std::endl;
                bf_buffer.put(std::span{key}, std::span{value});
                ground_truth.erase(key);
                ground_truth.emplace(key, value);
                break;
            }  
            case 1: { // del
                std::cout << "Del operation" << std::endl;
                bf_buffer.del(std::span{key});
                ground_truth.erase(key);
                break;
            }
            case 2: { // get
                std::cout << "Get operation" << std::endl;
                auto value = ARR();
                bool is_found = false;
                bool is_tombstone = false;
                bf_buffer.get(std::span{key}, std::span{value}, is_found, is_tombstone);
                if (ground_truth.count(key)) {
                    auto expected_value = ground_truth[key];
                    ASSERT_EQ(expected_value, value);
                } else {
                    auto is_null = (!is_found || is_tombstone);
                    ASSERT_TRUE(is_null);
                }
                break;
            }
        }
    }
    
}