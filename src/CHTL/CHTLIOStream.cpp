#include "CHTL/CHTLIOStream.h"

namespace CHTL {

CHTLIOStream::CHTLIOStream() = default;

void CHTLIOStream::write(const std::string& data) {
    std::cout << data;
}

std::string CHTLIOStream::read() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

} // namespace CHTL