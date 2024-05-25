class XorShift32{
public:
    XorShift32(uint32_t state) : state_(state) {}

    uint32_t sample() {
        uint32_t x = this->state_;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5; 
        return this->state_ = x;
    }

    void fill(std::span<uint8_t> buf) {
        uint32_t x = 0;
        for (size_t i = 0; i < buf.size(); ++i) {
            if (i % 4 == 0) {
                x = sample();
            }
            // reverse uint32_t value per 8 byte
            buf[i] = (x >> (i % 4) * 8) & (0xff);
        }
    }

private:
    uint32_t state_;
};