#include "persist.h"
#include <cassert>
#include <cstdint>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string>
#include <span>

FileObject::FileObject(std::string&& path) : path(path) {
    this->fd = open(path.c_str(), O_RDWR | O_CREAT | O_SYNC | O_APPEND, 0600);
}

void FileObject::Append(std::span<uint8_t> bytes) {
    // assert(bytes.size() % 1024 == 0);
    // O_APPEND, so no lseek
    int32_t flag = write(this->fd, bytes.data(), bytes.size());
    assert(flag > 0);
}

/**
 * @brief from given position to bytes
 * 
 * @param position 
 * @param bytes 
 */
void FileObject::Read(size_t position, std::span<uint8_t> bytes) {
    // assert(bytes.size() % 1024 == 0);
    // assert(position % 1024 == 0);
    lseek(this->fd, position, SEEK_SET);
    int32_t flag = read(this->fd, &bytes[0], bytes.size());
    // assert(flag > 0);
}