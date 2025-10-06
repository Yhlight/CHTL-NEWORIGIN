#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/SelectorNode.h"
#include "../CHTLJSNode/StringLiteralNode.h"
#include <stdexcept>
#include <algorithm> // for std::sort

namespace CHTLJS {

    std::string CHTLJSGenerator::generate(const std::shared_ptr<BaseNode>& node) {
        if (node) {
            visit(node);
        }
        return js_out.str();
    }

    void CHTLJSGenerator::visit(const std::shared_ptr<BaseNode>& node) {
        if (!node) return;

        switch (node->getType()) {
            case NodeType::NODE_SELECTOR:
                visit(std::dynamic_pointer_cast<SelectorNode>(node));
                break;
            case NodeType::NODE_STRING_LITERAL:
                visit(std::dynamic_pointer_cast<StringLiteralNode>(node));
                break;
            default:
                break;
        }
    }

    void CHTLJSGenerator::visit(const std::shared_ptr<SelectorNode>& node) {
        js_out << "document.querySelector('" << node->getSelector() << "')";
    }

    void CHTLJSGenerator::visit(const std::shared_ptr<StringLiteralNode>& node) {
        js_out << "\"" << node->getValue() << "\"";
    }

    std::string CHTLJSGenerator::generateAnimation(const CHTL::AnimationData& data) {
        std::stringstream ss;
        ss << "(() => {\n";
        ss << "    const targetElement = document.querySelector('" << data.target << "');\n";
        ss << "    if (!targetElement) { console.error('Animation target not found:', '" << data.target << "'); return; }\n\n";

        ss << "    const duration = " << (data.duration.empty() ? "1000" : data.duration) << ";\n";
        ss << "    const easing = '" << (data.easing.empty() ? "linear" : data.easing) << "';\n";
        ss << "    const delay = " << (data.delay.empty() ? "0" : data.delay) << ";\n";
        ss << "    const callback = " << (data.callback.empty() ? "null" : data.callback) << ";\n\n";


        // --- Helper functions ---
        ss << "    function parseUnit(value) { const match = String(value).match(/([a-zA-Z%]*)$/); return match ? match[0] : ''; }\n";
        ss << "    function lerp(start, end, progress) { return start + (end - start) * progress; }\n\n";

        // --- Keyframes ---
        ss << "    const keyframes = [];\n";
        // Begin
        ss << "    keyframes.push({ time: 0, styles: {";
        for (size_t i = 0; i < data.begin_properties.size(); ++i) {
            ss << "\"" << data.begin_properties[i].name << "\": \"" << data.begin_properties[i].value << "\"" << (i < data.begin_properties.size() - 1 ? "," : "");
        }
        ss << "} });\n";
        // When
        for (const auto& kf : data.when_keyframes) {
            ss << "    keyframes.push({ time: " << kf.at << ", styles: {";
            for (size_t i = 0; i < kf.properties.size(); ++i) {
                ss << "\"" << kf.properties[i].name << "\": \"" << kf.properties[i].value << "\"" << (i < kf.properties.size() - 1 ? "," : "");
            }
            ss << "} });\n";
        }
        // End
        ss << "    keyframes.push({ time: 1, styles: {";
        for (size_t i = 0; i < data.end_properties.size(); ++i) {
            ss << "\"" << data.end_properties[i].name << "\": \"" << data.end_properties[i].value << "\"" << (i < data.end_properties.size() - 1 ? "," : "");
        }
        ss << "} });\n";
        ss << "    keyframes.sort((a, b) => a.time - b.time);\n\n";

        ss << "    function applyStyles(styles) {\n";
        ss << "        for (const prop in styles) {\n";
        ss << "            const propInCamelCase = prop.replace(/-([a-z])/g, g => g[1].toUpperCase());\n";
        ss << "            targetElement.style[propInCamelCase] = styles[prop];\n";
        ss << "        }\n";
        ss << "    }\n\n";

        // --- Animation Logic ---
        ss << "    let startTime = null;\n";
        ss << "    function animationStep(timestamp) {\n";
        ss << "        if (!startTime) startTime = timestamp;\n";
        ss << "        const elapsedTime = timestamp - startTime;\n";
        ss << "        let progress = Math.min(elapsedTime / duration, 1);\n\n";

        ss << "        let startFrame, endFrame;\n";
        ss << "        for (let i = 0; i < keyframes.length - 1; i++) {\n";
        ss << "            if (progress >= keyframes[i].time && progress <= keyframes[i+1].time) {\n";
        ss << "                startFrame = keyframes[i];\n";
        ss << "                endFrame = keyframes[i+1];\n";
        ss << "                break;\n";
        ss << "            }\n";
        ss << "        }\n\n";

        ss << "        if (startFrame && endFrame) {\n";
        ss << "            const frameDuration = endFrame.time - startFrame.time;\n";
        ss << "            const frameProgress = frameDuration > 0 ? (progress - startFrame.time) / frameDuration : 1;\n";
        ss << "            for (const prop in endFrame.styles) {\n";
        ss << "                if (startFrame.styles[prop]) {\n";
        ss << "                    const startVal = parseFloat(startFrame.styles[prop]);\n";
        ss << "                    const endVal = parseFloat(endFrame.styles[prop]);\n";
        ss << "                    const unit = parseUnit(startFrame.styles[prop]);\n";
        ss << "                    if (!isNaN(startVal) && !isNaN(endVal)) {\n";
        ss << "                        const currentVal = lerp(startVal, endVal, frameProgress);\n";
        ss << "                        const propInCamelCase = prop.replace(/-([a-z])/g, g => g[1].toUpperCase());\n";
        ss << "                        targetElement.style[propInCamelCase] = currentVal + unit;\n";
        ss << "                    }\n";
        ss << "                }\n";
        ss << "            }\n";
        ss << "        }\n\n";

        ss << "        if (progress < 1) {\n";
        ss << "            requestAnimationFrame(animationStep);\n";
        ss << "        } else {\n";
        ss << "            applyStyles(keyframes[keyframes.length - 1].styles);\n";
        ss << "            if (typeof callback === 'function') { callback(); }\n";
        ss << "        }\n";
        ss << "    }\n\n";

        ss << "    setTimeout(() => {\n";
        ss << "        applyStyles(keyframes[0].styles);\n";
        ss << "        requestAnimationFrame(animationStep);\n";
        ss << "    }, delay);\n";

        ss << "})();";

        return ss.str();
    }

}