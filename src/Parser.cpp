#include "chtl/Parser.hpp"
#include <stdexcept>

namespace chtl {

Parser::Parser(Lexer lexer) : lexer_(std::move(lexer)) {
    current_ = lexer_.next();
}

void Parser::advance() {
    current_ = lexer_.next();
}

bool Parser::accept(TokenKind kind) {
    if (current_.kind == kind) { advance(); return true; }
    return false;
}

bool Parser::expect(TokenKind kind, const char* what) {
    if (!accept(kind)) {
        throw std::runtime_error(std::string("期待 ") + what);
    }
    return true;
}

Document Parser::parseDocument() {
    Document doc;
    while (current_.kind != TokenKind::EndOfFile) {
        if (auto n = parseNode()) doc.children.emplace_back(std::move(n));
        else advance();
    }
    return doc;
}

std::unique_ptr<Node> Parser::parseText() {
    if (current_.kind == TokenKind::Text) {
        auto node = std::make_unique<TextNode>();
        node->text = current_.lexeme;
        node->range = current_.range;
        advance();
        return node;
    }
    return nullptr;
}

std::unique_ptr<Node> Parser::parseComment() {
    // already removed by lexer as whitespace. Keep placeholder if later needed
    return nullptr;
}

static bool isStartOfElement(const Token& t) {
    return t.kind == TokenKind::Identifier || t.kind == TokenKind::Dot || t.kind == TokenKind::Hash || t.kind == TokenKind::Less;
}

std::unique_ptr<Node> Parser::parseElement() {
    if (!isStartOfElement(current_)) return nullptr;

    auto elem = std::make_unique<ElementNode>();

    // support shorthand .class or #id -> div with those
    if (current_.kind == TokenKind::Dot || current_.kind == TokenKind::Hash) {
        elem->tagName = "div";
        std::vector<std::string> classes;
        for (;;) {
            if (current_.kind == TokenKind::Dot) {
                advance();
                if (current_.kind == TokenKind::Identifier) {
                    classes.push_back(current_.lexeme);
                    advance();
                }
            } else if (current_.kind == TokenKind::Hash) {
                advance();
                if (current_.kind == TokenKind::Identifier) {
                    elem->attrs.push_back({"id", current_.lexeme, current_.range});
                    advance();
                }
            } else break;
        }
        if (!classes.empty()) {
            std::string cls;
            for (size_t i = 0; i < classes.size(); ++i) {
                if (i) cls += ' ';
                cls += classes[i];
            }
            elem->attrs.push_back({"class", cls, {}});
        }
    } else if (current_.kind == TokenKind::Identifier) {
        elem->tagName = current_.lexeme;
        advance();
    } else if (current_.kind == TokenKind::Less) {
        // <tag ...> ... </tag>
        advance();
        if (current_.kind != TokenKind::Identifier) return nullptr;
        elem->tagName = current_.lexeme; advance();
        auto attrs = parseAttributes();
        elem->attrs = std::move(attrs);
        expect(TokenKind::Greater, ">\n");
        // children until </tag>
        while (!(current_.kind == TokenKind::Less && lexer_.peek().kind == TokenKind::Slash)) {
            if (current_.kind == TokenKind::EndOfFile) break;
            if (auto n = parseNode()) elem->children.emplace_back(std::move(n));
            else advance();
        }
        // consume </tag>
        expect(TokenKind::Less, "</");
        expect(TokenKind::Slash, "/");
        expect(TokenKind::Identifier, "tag name");
        expect(TokenKind::Greater, ">");
        return elem;
    }

    // attributes in chtl form: name : value; ... optional
    if (current_.kind == TokenKind::LBrace) {
        advance();
        // parse children block
        while (!accept(TokenKind::RBrace) && current_.kind != TokenKind::EndOfFile) {
            if (current_.kind == TokenKind::Identifier) {
                // attribute
                auto name = current_.lexeme; advance();
                if (accept(TokenKind::Colon) || accept(TokenKind::Equal)) {
                    std::string value;
                    // value can be string/number/identifier/text until ;
                    while (current_.kind != TokenKind::Semicolon && current_.kind != TokenKind::RBrace && current_.kind != TokenKind::EndOfFile) {
                        if (!value.empty()) value += ' ';
                        value += current_.lexeme;
                        advance();
                    }
                    elem->attrs.push_back({name, value, {}});
                    accept(TokenKind::Semicolon);
                }
            } else if (current_.kind == TokenKind::Text || current_.kind == TokenKind::String || current_.kind == TokenKind::Number) {
                // treat as text child until ; or block end
                auto t = std::make_unique<TextNode>();
                t->text = current_.lexeme; t->range = current_.range; advance();
                elem->children.emplace_back(std::move(t));
            } else if (isStartOfElement(current_)) {
                if (auto n = parseElement()) elem->children.emplace_back(std::move(n));
                else advance();
            } else {
                if (!accept(TokenKind::Semicolon)) advance();
            }
        }
    }

    return elem;
}

std::vector<Attribute> Parser::parseAttributes() {
    std::vector<Attribute> attrs;
    while (current_.kind == TokenKind::Identifier) {
        std::string name = current_.lexeme; advance();
        if (accept(TokenKind::Equal) || accept(TokenKind::Colon)) {
            std::string value;
            if (current_.kind == TokenKind::String || current_.kind == TokenKind::Identifier || current_.kind == TokenKind::Number) {
                value = current_.lexeme; advance();
            }
            attrs.push_back({name, value, {}});
        }
    }
    return attrs;
}

std::unique_ptr<Node> Parser::parseNode() {
    if (auto t = parseText()) return t;
    if (auto e = parseElement()) return e;
    return nullptr;
}

} // namespace chtl
