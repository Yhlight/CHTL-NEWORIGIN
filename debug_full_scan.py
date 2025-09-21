#!/usr/bin/env python3
"""
完整调试统一扫描器
"""

from unified_scanner import UnifiedScanner

def debug_full_scan():
    """调试完整扫描过程"""
    scanner = UnifiedScanner()
    
    # 测试用例1：全局style块
    test_content1 = """
    <style>
        body {
            data-theme: Theme(Sum);  /* CHTL语法：模板变量 */
        }
        
        .box {
            width: 100px;
            height: 100px / 2;  /* CHTL语法：属性运算 */
            @Style BoxStyle;    /* CHTL语法：模板样式组 */
        }
    </style>
    """
    
    print("测试用例1：全局style块")
    print("原始内容:")
    print(test_content1)
    print("\n" + "="*50 + "\n")
    
    result1 = scanner.scan(test_content1)
    print("扫描结果:")
    print(result1['processed_content'])
    print(f"\n占位符数量: {len(result1['placeholders'])}")
    for placeholder_id, info in result1['placeholders'].items():
        print(f"  {placeholder_id}: {info.original_content} ({info.token_type.value})")
    
    print("\n" + "="*50 + "\n")
    
    # 测试用例2：局部script
    test_content2 = """
    <div>
        <script>
            let v = "Test";
            {{v}} -> Listen {       /* CHTL JS语法 */
                click: () => { console.log({{v}}) }  /* JS语法 */
            }
        </script>
    </div>
    """
    
    print("测试用例2：局部script")
    print("原始内容:")
    print(test_content2)
    print("\n" + "="*50 + "\n")
    
    result2 = scanner.scan(test_content2)
    print("扫描结果:")
    print(result2['processed_content'])
    print(f"\n占位符数量: {len(result2['placeholders'])}")
    for placeholder_id, info in result2['placeholders'].items():
        print(f"  {placeholder_id}: {info.original_content} ({info.token_type.value})")
    
    print("\n" + "="*50 + "\n")
    
    # 测试用例3：纯JS代码
    test_content3 = """
    function test(a,b,c) {
        return "test";
    }
    
    const testVar = () => {
        console.log("arrow function");
    };
    """
    
    print("测试用例3：纯JS代码")
    print("原始内容:")
    print(test_content3)
    print("\n" + "="*50 + "\n")
    
    result3 = scanner.scan(test_content3)
    print("扫描结果:")
    print(result3['processed_content'])
    print(f"\n占位符数量: {len(result3['placeholders'])}")
    for placeholder_id, info in result3['placeholders'].items():
        print(f"  {placeholder_id}: {info.original_content} ({info.token_type.value})")

if __name__ == "__main__":
    debug_full_scan()