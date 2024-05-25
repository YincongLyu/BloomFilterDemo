#include <gtest/gtest.h>
#include "bloom_filter.h"
#include "xorshift32.h"
#include <unordered_set>
#include <cmath>

TEST(TestBFInsert, TestBFInsert) {
    auto bf = bloomstore::BloomFilter(32, 4);
    std::vector<uint8_t> vec = {'o'};
    std::span<uint8_t> span_vec(vec);
    bf.insert(span_vec);
    bool res = bf.test(span_vec);
    ASSERT_EQ(true, res);
}

TEST(TestBFInsert, NoFalseNegtive) {
    // 1. insert to set
    int total_number = 100;
    int bf_slot_num = 1000;
    int bf_func_num = 4;

    auto bloom_filter = bloomstore::BloomFilter(bf_slot_num, bf_func_num);
    // collect for test
    auto ground_truth = std::unordered_set<uint32_t>{};
    auto random_number_generator = XorShift32(123);


    for (size_t i = 0; i < total_number; i++) {
        uint32_t value = random_number_generator.sample();
        ground_truth.insert(value);
        // 按 8byte 一组 处理 uint32_t 的value
        std::array<uint8_t, 4> value2array;
        memcpy(&value2array, &value, sizeof (value));
        bloom_filter.insert(std::span<uint8_t>{value2array});

    }
    
    // 2. test
    bool ok = true;
    for (const auto& value : ground_truth) {
        std::array<uint8_t, 4> value2array;
        memcpy(&value2array, &value, sizeof (value));
        ok &= bloom_filter.test(std::span<uint8_t>{value2array});
    }
    ASSERT_EQ(ok, true);

}

TEST(TestBFInsert, FPRateCloseToTheory) {
    // auto toFloat = [](uint32_t x) -> float {
    //     return static_cast<float>(x);
    // };

   // 1. insert to set
    int total_number = 100;
    int test_total_number = 5000;
    int bf_slot_num = 1000;
    int bf_func_num = 4;

    auto bloom_filter = bloomstore::BloomFilter(bf_slot_num, bf_func_num);
    // collect for test
    auto ground_truth = std::unordered_set<uint32_t>{};
    auto random_number_generator = XorShift32(123);

    // 随机插入 1000 个value
    for (size_t i = 0; i < total_number; i++) {
        uint32_t value = random_number_generator.sample();
        ground_truth.insert(value);
        std::array<uint8_t, 4> value2array;
        memcpy(&value2array, &value, sizeof (value));
        bloom_filter.insert(std::span<uint8_t>{value2array});

    }
    
    // 2. 插入更多的值，计算假阳性的概率
    int fp_count_now = 0;
    for (size_t i = 0; i < test_total_number; ++i) {
        uint32_t value = random_number_generator.sample();
        std::array<uint8_t, 4> value2array;
        memcpy(&value2array, &value, sizeof (value));
        if (bloom_filter.test(std::span<uint8_t>{value2array}) && !ground_truth.count(value))
            fp_count_now += 1;
    }
    // float fp_rate_now = toFloat(fp_count_now) / toFloat(test_total_number);
    // fp_rate_theory = std::pow((1.0 - (1.0 - std::pow(1.0 - 1.0 / toFloat(bf_slot_num), toFloat(total_number * bf_func_num)))), bf_func_num);
    float fp_rate_now = fp_count_now * 1.0 / test_total_number;
    float fp_rate_theory = pow(1.0 - pow(1.0 - (1.0 - 1.0 / bf_slot_num), total_number * bf_func_num), bf_func_num);
    ASSERT_TRUE(std::abs(fp_rate_now - fp_rate_theory) < 0.01);
}
