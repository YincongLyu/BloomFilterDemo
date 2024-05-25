#include <gtest/gtest.h>
#include <bloom_filter.h>

TEST(TestBFInsert, TestBFInsert) {
    auto bf = bloomstore::BloomFilter(32, 4);
    std::vector<uint8_t> vec = {'o'};
    std::span<uint8_t> span_vec(vec);
    bf.insert(span_vec);
    bool res = bf.test(span_vec);
    ASSERT_EQ(true, res);
}