#include <gtest/gtest.h>
#include "bloom_buffer.h"
#include "xorshift32.h"

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
    auto now_value = ARR();
    memcpy(&now_value, &value, sizeof (value));
    return now_value;
}
TEST(BFBufferTest, ReadWriteTest) {
    auto bf_buffer = bloomstore::BloomBuffer(K, V, C);
    auto ground_truth = std::unordered_map<ARR, ARR, ArrayHasher>();
    auto random_number_generator = XorShift32(123);

    int total_operaters = 500;
    int key_range_sz = 64;
    for (size_t i = 0; i < total_operaters; ++i) {
        auto op_type = random_number_generator.sample() % 3;
        auto key = to_arr(random_number_generator.sample() % key_range_sz);
        auto value = to_arr(random_number_generator.sample());
        switch (op_type)
        {
            case 0: { // put
                bf_buffer.put(std::span{key}, std::span{value});
                ground_truth.erase(key);
                ground_truth.emplace(key, value);
                break;
            }  
            case 1: { // del
                bf_buffer.del(std::span{key});
                ground_truth.erase(key);
                break;
            }
            
            case 2: { // get
                auto value = bf_buffer.get(std::span{key});
                if (ground_truth.count(key)) {
                    auto expected_value = std::vector(ground_truth[key].begin(), ground_truth[key].end()); // array, 4
                    auto value_in_buffer = std::vector(value.value().begin(), value.value().end()); // std::span<uint8_t>
                    // 统一转化成 vector 比较
                    ASSERT_EQ(expected_value, value_in_buffer);
                } else {
                    auto is_null = !value.has_value();
                    ASSERT_TRUE(is_null);
                }
                break;
            }
        }
    }
    
}