#include "chtl/Generator.hpp"
#include <sstream>

namespace chtl {

static void escapeHTML(const std::string& in, std::ostringstream& out) {
    for (char c : in) {
        switch (c) {
            case '&': out << "&amp;"; break;
            case '<': out << "&lt;"; break;
            case '>': out << "&gt;"; break;
            case '"': out << "&quot;"; break;
            case '\'': out << "&#39;"; break;
            default: out << c; break;
        }
    }
}

void Generator::genNode(const Node* node, std::ostringstream& out) {
    if (auto t = dynamic_cast<const TextNode*>(node)) {
        escapeHTML(t->text, out);
        return;
    }
    if (auto e = dynamic_cast<const ElementNode*>(node)) {
        out << '<' << e->tagName;
        for (const auto& a : e->attrs) {
            out << ' ' << a.name << "=\"" << a.value << "\"";
        }
        out << '>';
        for (const auto& c : e->children) genNode(c.get(), out);
        out << "</" << e->tagName << '>';
        return;
    }
}

std::string Generator::generateHTML(const Document& doc) {
    std::ostringstream out;
    for (const auto& n : doc.children) genNode(n.get(), out);
    return out.str();
}

} // namespace chtl
