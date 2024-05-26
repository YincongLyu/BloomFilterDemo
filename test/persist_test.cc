#include <gtest/gtest.h>
#include "persist.h"
#include "xorshift32.h"
#include <iostream>

TEST(TestPersist, TestPersist) {
    auto fileOBJ = FileObject("./tmp-test");
    auto random_number_generator = XorShift32(123);
    uint32_t key = random_number_generator.sample();

    std::array<uint8_t, 4> number;
    memcpy(&number, &key, sizeof (key));

    fileOBJ.Append(std::span{number});
    std::span<uint8_t> readbytes;
    fileOBJ.Read(0, readbytes);
    std::cout << readbytes.data() << "========";

}