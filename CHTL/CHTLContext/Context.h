#pragma once

#include <sstream>
#include <string>

/*
 * The CHTLContext holds the state of the compilation process,
 * including the output streams for the final generated code.
 */
class CHTLContext {
public:
    std::stringstream html_output;
    std::stringstream css_output;

    std::string getFinalHtml() {
        std::string css = css_output.str();
        if (css.empty()) {
            return html_output.str();
        }

        // For now, inject the CSS into a <style> tag in the <head>.
        // A more robust solution would find or create the head tag.
        std::string html = html_output.str();
        std::string style_tag = "\n<style>\n" + css + "</style>\n";

        size_t head_pos = html.find("</head>");
        if (head_pos != std::string::npos) {
            html.insert(head_pos, style_tag);
            return html;
        }

        // If no head tag, just prepend for now.
        return style_tag + html;
    }
};
