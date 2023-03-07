#include <iostream>
#include <vector>

class BitStreamEncoder {
public:
    BitStreamEncoder() : byte_(0), bit_position_(0) {}

    void add_bits(uint8_t value, uint8_t num_bits) {
        while (num_bits > 0) {
            uint8_t bits_to_add = std::min(num_bits, uint8_t(8 - bit_position_));
            byte_ |= (value << bit_position_) & ((1 << bits_to_add) - 1);
            bit_position_ += bits_to_add;
            num_bits -= bits_to_add;

            if (bit_position_ == 8) {
                bytes_.push_back(byte_);
                byte_ = 0;
                bit_position_ = 0;
            }
        }
    }

    std::vector<uint8_t> get_bytes() const {
        if (bit_position_ > 0) {
            bytes_.push_back(byte_);
        }
        return bytes_;
    }

private:
    std::vector<uint8_t> bytes_;
    uint8_t byte_;
    uint8_t bit_position_;
};

struct S {
    int a; // 8 bits
    bool b; // 1 bit
    double c; // 64 bits
    char d; // 4 bits
};

std::vector<uint8_t> encode(const S& s) {
    BitStreamEncoder encoder;

    encoder.add_bits(s.a, 8);
    encoder.add_bits(s.b, 1);

    uint64_t c_bits = *reinterpret_cast<const uint64_t*>(&s.c);
    encoder.add_bits(c_bits >> 56, 8);
    encoder.add_bits(c_bits >> 48, 8);
    encoder.add_bits(c_bits >> 40, 8);
    encoder.add_bits(c_bits >> 32, 8);
    encoder.add_bits(c_bits >> 24, 8);
    encoder.add_bits(c_bits >> 16, 8);
    encoder.add_bits(c_bits >> 8, 8);
    encoder.add_bits(c_bits, 8);

    encoder.add_bits(s.d, 4);

    return encoder.get_bytes();
}

int main() {
    S s = {42, true, 3.14, 'A'};
    auto bytes = encode(s);

    std::cout << "Encoded bytes: ";
    for (auto b : bytes) {
        std::cout << std::hex << static_cast<int>(b) << " ";
    }
    std::cout << std::endl;
}
