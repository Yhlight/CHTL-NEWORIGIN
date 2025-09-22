#!/usr/bin/env python3
"""
CHTL Use指令系统实现
支持html5、其他指令等功能
"""

from typing import Dict, List, Optional, Any
from enum import Enum
from dataclasses import dataclass

class UseDirectiveType(Enum):
    """Use指令类型"""
    HTML5 = "html5"
    CSS3 = "css3"
    JAVASCRIPT = "javascript"
    JQUERY = "jquery"
    BOOTSTRAP = "bootstrap"
    REACT = "react"
    VUE = "vue"
    ANGULAR = "angular"
    TYPESCRIPT = "typescript"
    SASS = "sass"
    LESS = "less"
    WEBPACK = "webpack"
    BABEL = "babel"
    ESLINT = "eslint"
    PRETTIER = "prettier"
    CUSTOM = "custom"

@dataclass
class UseDirective:
    """Use指令数据结构"""
    type: UseDirectiveType
    directive: str
    version: Optional[str] = None
    options: Dict[str, Any] = None
    line: int = 0
    column: int = 0

class UseDirectiveProcessor:
    """Use指令处理器"""
    
    def __init__(self):
        self.directives: List[UseDirective] = []
        self.directive_templates = {
            UseDirectiveType.HTML5: {
                "doctype": "<!DOCTYPE html>",
                "meta": [
                    '<meta charset="UTF-8">',
                    '<meta name="viewport" content="width=device-width, initial-scale=1.0">',
                    '<meta http-equiv="X-UA-Compatible" content="IE=edge">'
                ],
                "scripts": [],
                "styles": []
            },
            UseDirectiveType.CSS3: {
                "prefixes": ["-webkit-", "-moz-", "-ms-", "-o-"],
                "features": ["flexbox", "grid", "transitions", "transforms", "animations"]
            },
            UseDirectiveType.JAVASCRIPT: {
                "version": "ES6+",
                "features": ["modules", "async/await", "arrow functions", "destructuring"]
            },
            UseDirectiveType.JQUERY: {
                "version": "3.6.0",
                "cdn": "https://code.jquery.com/jquery-3.6.0.min.js",
                "features": ["DOM manipulation", "AJAX", "events", "animations"]
            },
            UseDirectiveType.BOOTSTRAP: {
                "version": "5.3.0",
                "cdn_css": "https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css",
                "cdn_js": "https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js",
                "features": ["grid", "components", "utilities", "responsive"]
            },
            UseDirectiveType.REACT: {
                "version": "18.2.0",
                "cdn": "https://unpkg.com/react@18/umd/react.production.min.js",
                "features": ["components", "hooks", "jsx", "virtual dom"]
            },
            UseDirectiveType.VUE: {
                "version": "3.3.0",
                "cdn": "https://unpkg.com/vue@3/dist/vue.global.js",
                "features": ["components", "reactivity", "template", "composition api"]
            },
            UseDirectiveType.ANGULAR: {
                "version": "16.0.0",
                "cdn": "https://unpkg.com/@angular/core@16.0.0/bundles/core.umd.js",
                "features": ["components", "services", "dependency injection", "routing"]
            },
            UseDirectiveType.TYPESCRIPT: {
                "version": "5.0.0",
                "compiler": "tsc",
                "features": ["static typing", "interfaces", "generics", "decorators"]
            },
            UseDirectiveType.SASS: {
                "version": "1.63.0",
                "compiler": "sass",
                "features": ["variables", "mixins", "nesting", "functions"]
            },
            UseDirectiveType.LESS: {
                "version": "4.2.0",
                "compiler": "lessc",
                "features": ["variables", "mixins", "nesting", "functions"]
            },
            UseDirectiveType.WEBPACK: {
                "version": "5.88.0",
                "config": "webpack.config.js",
                "features": ["bundling", "code splitting", "hot reload", "optimization"]
            },
            UseDirectiveType.BABEL: {
                "version": "7.22.0",
                "config": ".babelrc",
                "features": ["transpilation", "polyfills", "plugins", "presets"]
            },
            UseDirectiveType.ESLINT: {
                "version": "8.45.0",
                "config": ".eslintrc.js",
                "features": ["linting", "code quality", "style enforcement", "custom rules"]
            },
            UseDirectiveType.PRETTIER: {
                "version": "3.0.0",
                "config": ".prettierrc",
                "features": ["code formatting", "consistency", "auto-fix", "customization"]
            }
        }
    
    def parse_use_directive(self, directive: str, version: str = None, 
                          options: Dict[str, Any] = None) -> UseDirective:
        """解析use指令"""
        # 标准化指令名称
        directive_lower = directive.lower()
        
        # 确定指令类型
        directive_type = UseDirectiveType.CUSTOM
        for use_type in UseDirectiveType:
            if use_type.value == directive_lower:
                directive_type = use_type
                break
        
        # 创建指令对象
        use_directive = UseDirective(
            type=directive_type,
            directive=directive,
            version=version,
            options=options or {}
        )
        
        self.directives.append(use_directive)
        return use_directive
    
    def generate_html5_boilerplate(self) -> str:
        """生成HTML5样板代码"""
        template = self.directive_templates[UseDirectiveType.HTML5]
        
        html = template["doctype"] + "\n"
        html += '<html lang="en">\n'
        html += '<head>\n'
        html += '    <title>Document</title>\n'
        
        for meta in template["meta"]:
            html += f'    {meta}\n'
        
        for style in template["styles"]:
            html += f'    {style}\n'
        
        html += '</head>\n'
        html += '<body>\n'
        html += '    <!-- Content goes here -->\n'
        html += '</body>\n'
        
        for script in template["scripts"]:
            html += f'{script}\n'
        
        html += '</html>'
        
        return html
    
    def generate_css3_features(self) -> str:
        """生成CSS3特性代码"""
        template = self.directive_templates[UseDirectiveType.CSS3]
        
        css = "/* CSS3 Features */\n"
        css += ".container {\n"
        css += "    display: flex;\n"
        css += "    display: -webkit-flex;\n"
        css += "    display: -moz-flex;\n"
        css += "    display: -ms-flex;\n"
        css += "    display: -o-flex;\n"
        css += "}\n\n"
        
        css += ".grid {\n"
        css += "    display: grid;\n"
        css += "    display: -webkit-grid;\n"
        css += "    display: -moz-grid;\n"
        css += "    display: -ms-grid;\n"
        css += "    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));\n"
        css += "}\n\n"
        
        css += ".transition {\n"
        css += "    transition: all 0.3s ease;\n"
        css += "    -webkit-transition: all 0.3s ease;\n"
        css += "    -moz-transition: all 0.3s ease;\n"
        css += "    -ms-transition: all 0.3s ease;\n"
        css += "    -o-transition: all 0.3s ease;\n"
        css += "}\n"
        
        return css
    
    def generate_javascript_features(self) -> str:
        """生成JavaScript特性代码"""
        template = self.directive_templates[UseDirectiveType.JAVASCRIPT]
        
        js = "// JavaScript ES6+ Features\n"
        js += "// Arrow functions\n"
        js += "const arrowFunction = () => {\n"
        js += "    console.log('Hello from arrow function!');\n"
        js += "};\n\n"
        
        js += "// Async/await\n"
        js += "async function fetchData() {\n"
        js += "    try {\n"
        js += "        const response = await fetch('/api/data');\n"
        js += "        const data = await response.json();\n"
        js += "        return data;\n"
        js += "    } catch (error) {\n"
        js += "        console.error('Error:', error);\n"
        js += "    }\n"
        js += "}\n\n"
        
        js += "// Destructuring\n"
        js += "const { name, age, email } = user;\n"
        js += "const [first, second, ...rest] = array;\n"
        
        return js
    
    def generate_library_includes(self, directive_type: UseDirectiveType) -> str:
        """生成库包含代码"""
        template = self.directive_templates.get(directive_type, {})
        
        if not template:
            return ""
        
        includes = []
        
        if "cdn" in template:
            includes.append(f'<script src="{template["cdn"]}"></script>')
        
        if "cdn_css" in template:
            includes.append(f'<link rel="stylesheet" href="{template["cdn_css"]}">')
        
        if "cdn_js" in template:
            includes.append(f'<script src="{template["cdn_js"]}"></script>')
        
        return "\n".join(includes)
    
    def generate_config_files(self, directive_type: UseDirectiveType) -> Dict[str, str]:
        """生成配置文件"""
        template = self.directive_templates.get(directive_type, {})
        
        if not template:
            return {}
        
        configs = {}
        
        if directive_type == UseDirectiveType.WEBPACK:
            configs["webpack.config.js"] = self._generate_webpack_config()
        elif directive_type == UseDirectiveType.BABEL:
            configs[".babelrc"] = self._generate_babel_config()
        elif directive_type == UseDirectiveType.ESLINT:
            configs[".eslintrc.js"] = self._generate_eslint_config()
        elif directive_type == UseDirectiveType.PRETTIER:
            configs[".prettierrc"] = self._generate_prettier_config()
        elif directive_type == UseDirectiveType.TYPESCRIPT:
            configs["tsconfig.json"] = self._generate_typescript_config()
        
        return configs
    
    def _generate_webpack_config(self) -> str:
        """生成Webpack配置"""
        return """const path = require('path');

module.exports = {
    entry: './src/index.js',
    output: {
        filename: 'bundle.js',
        path: path.resolve(__dirname, 'dist')
    },
    module: {
        rules: [
            {
                test: /\\.js$/,
                exclude: /node_modules/,
                use: {
                    loader: 'babel-loader'
                }
            },
            {
                test: /\\.css$/,
                use: ['style-loader', 'css-loader']
            }
        ]
    },
    devServer: {
        contentBase: './dist',
        hot: true
    }
};"""
    
    def _generate_babel_config(self) -> str:
        """生成Babel配置"""
        return """{
    "presets": ["@babel/preset-env"],
    "plugins": [
        "@babel/plugin-proposal-class-properties",
        "@babel/plugin-transform-runtime"
    ]
}"""
    
    def _generate_eslint_config(self) -> str:
        """生成ESLint配置"""
        return """module.exports = {
    env: {
        browser: true,
        es2021: true,
        node: true
    },
    extends: [
        'eslint:recommended'
    ],
    parserOptions: {
        ecmaVersion: 12,
        sourceType: 'module'
    },
    rules: {
        'indent': ['error', 2],
        'linebreak-style': ['error', 'unix'],
        'quotes': ['error', 'single'],
        'semi': ['error', 'always']
    }
};"""
    
    def _generate_prettier_config(self) -> str:
        """生成Prettier配置"""
        return """{
    "semi": true,
    "trailingComma": "es5",
    "singleQuote": true,
    "printWidth": 80,
    "tabWidth": 2
}"""
    
    def _generate_typescript_config(self) -> str:
        """生成TypeScript配置"""
        return """{
    "compilerOptions": {
        "target": "ES2020",
        "module": "commonjs",
        "lib": ["ES2020", "DOM"],
        "outDir": "./dist",
        "rootDir": "./src",
        "strict": true,
        "esModuleInterop": true,
        "skipLibCheck": true,
        "forceConsistentCasingInFileNames": true
    },
    "include": ["src/**/*"],
    "exclude": ["node_modules", "dist"]
}"""
    
    def get_directive_info(self, directive_type: UseDirectiveType) -> Dict[str, Any]:
        """获取指令信息"""
        return self.directive_templates.get(directive_type, {})
    
    def list_available_directives(self) -> List[str]:
        """列出可用的指令"""
        return [directive_type.value for directive_type in UseDirectiveType]
    
    def generate_directive_code(self, directive: str, args: List[str] = None) -> str:
        """生成指令代码"""
        if not args:
            args = []
        
        try:
            directive_type = UseDirectiveType(directive)
        except ValueError:
            # 自定义指令
            return self._generate_custom_directive(directive, args)
        
        if directive_type == UseDirectiveType.HTML5:
            return self.generate_html5_boilerplate()
        elif directive_type == UseDirectiveType.CSS3:
            return self.generate_css3_features()
        elif directive_type == UseDirectiveType.JAVASCRIPT:
            return self.generate_javascript_features()
        elif directive_type in [UseDirectiveType.JQUERY, UseDirectiveType.BOOTSTRAP, 
                               UseDirectiveType.REACT, UseDirectiveType.VUE, UseDirectiveType.ANGULAR]:
            return self.generate_library_includes(directive_type)
        elif directive_type in [UseDirectiveType.WEBPACK, UseDirectiveType.BABEL, 
                               UseDirectiveType.ESLINT, UseDirectiveType.PRETTIER, UseDirectiveType.TYPESCRIPT]:
            configs = self.generate_config_files(directive_type)
            return '\n\n'.join([f"{name}:\n{content}" for name, content in configs.items()])
        else:
            return self._generate_custom_directive(directive, args)
    
    def is_valid_directive(self, directive: str) -> bool:
        """检查指令是否有效"""
        try:
            UseDirectiveType(directive)
            return True
        except ValueError:
            return False
    
    def _generate_custom_directive(self, directive: str, args: List[str]) -> str:
        """生成自定义指令代码"""
        return f"<!-- Custom directive: {directive} with args: {args} -->"

def main():
    """测试Use指令系统"""
    print("🧪 测试CHTL Use指令系统")
    print("=" * 50)
    
    # 创建指令处理器
    processor = UseDirectiveProcessor()
    
    # 测试HTML5指令
    print("测试HTML5指令:")
    html5_directive = processor.parse_use_directive("html5")
    print(f"指令类型: {html5_directive.type.value}")
    print(f"指令名称: {html5_directive.directive}")
    
    html5_boilerplate = processor.generate_html5_boilerplate()
    print("生成的HTML5样板代码:")
    print(html5_boilerplate)
    
    print("\n" + "=" * 50)
    
    # 测试CSS3指令
    print("测试CSS3指令:")
    css3_directive = processor.parse_use_directive("css3")
    print(f"指令类型: {css3_directive.type.value}")
    
    css3_features = processor.generate_css3_features()
    print("生成的CSS3特性代码:")
    print(css3_features)
    
    print("\n" + "=" * 50)
    
    # 测试JavaScript指令
    print("测试JavaScript指令:")
    js_directive = processor.parse_use_directive("javascript")
    print(f"指令类型: {js_directive.type.value}")
    
    js_features = processor.generate_javascript_features()
    print("生成的JavaScript特性代码:")
    print(js_features)
    
    print("\n" + "=" * 50)
    
    # 测试库包含
    print("测试库包含:")
    bootstrap_includes = processor.generate_library_includes(UseDirectiveType.BOOTSTRAP)
    print("Bootstrap包含代码:")
    print(bootstrap_includes)
    
    print("\n" + "=" * 50)
    
    # 测试配置文件生成
    print("测试配置文件生成:")
    webpack_configs = processor.generate_config_files(UseDirectiveType.WEBPACK)
    print("Webpack配置文件:")
    for filename, content in webpack_configs.items():
        print(f"\n{filename}:")
        print(content)
    
    print("\n" + "=" * 50)
    
    # 列出可用指令
    print("可用指令:")
    available_directives = processor.list_available_directives()
    for directive in available_directives:
        print(f"  - {directive}")

if __name__ == "__main__":
    main()