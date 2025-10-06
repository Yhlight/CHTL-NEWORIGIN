#!/usr/bin/env python3
"""
CHTL项目构建脚本
避免CMake路径问题，统一在项目根目录进行构建
"""

import os
import sys
import subprocess
import shutil
import argparse
from pathlib import Path

class CHTLBuilder:
    def __init__(self, root_dir=None):
        # 确保在项目根目录
        if root_dir:
            self.root_dir = Path(root_dir).resolve()
        else:
            self.root_dir = Path(__file__).parent.resolve()
        
        self.build_dir = self.root_dir / "build"
        
    def clean(self):
        """清理构建目录"""
        print("🧹 清理构建目录...")
        if self.build_dir.exists():
            shutil.rmtree(self.build_dir)
            print("✅ 构建目录已清理")
        else:
            print("ℹ️  构建目录不存在，跳过清理")
    
    def configure(self, build_type="Debug"):
        """配置CMake"""
        print(f"⚙️  配置CMake (构建类型: {build_type})...")
        
        # 创建构建目录
        self.build_dir.mkdir(exist_ok=True)
        
        # 切换到构建目录
        os.chdir(self.build_dir)
        
        # 运行CMake配置
        cmd = [
            "cmake",
            f"-DCMAKE_BUILD_TYPE={build_type}",
            str(self.root_dir)
        ]
        
        try:
            subprocess.run(cmd, check=True)
            print("✅ CMake配置成功")
            return True
        except subprocess.CalledProcessError as e:
            print(f"❌ CMake配置失败: {e}")
            return False
        except FileNotFoundError:
            print("❌ 未找到CMake，请先安装CMake")
            return False
        finally:
            # 返回根目录
            os.chdir(self.root_dir)
    
    def build(self, jobs=None):
        """编译项目"""
        print("🔨 编译项目...")
        
        if not self.build_dir.exists():
            print("❌ 构建目录不存在，请先运行configure")
            return False
        
        # 切换到构建目录
        os.chdir(self.build_dir)
        
        # 运行编译
        cmd = ["cmake", "--build", "."]
        if jobs:
            cmd.extend(["-j", str(jobs)])
        
        try:
            subprocess.run(cmd, check=True)
            print("✅ 编译成功")
            return True
        except subprocess.CalledProcessError as e:
            print(f"❌ 编译失败: {e}")
            return False
        finally:
            # 返回根目录
            os.chdir(self.root_dir)
    
    def test(self):
        """运行测试"""
        print("🧪 运行测试...")
        
        if not self.build_dir.exists():
            print("❌ 构建目录不存在，请先运行build")
            return False
        
        # 切换到构建目录
        os.chdir(self.build_dir)
        
        try:
            subprocess.run(["ctest", "--output-on-failure"], check=True)
            print("✅ 测试通过")
            return True
        except subprocess.CalledProcessError as e:
            print(f"❌ 测试失败: {e}")
            return False
        except FileNotFoundError:
            print("ℹ️  未找到测试程序，跳过测试")
            return True
        finally:
            # 返回根目录
            os.chdir(self.root_dir)
    
    def run(self, args=None):
        """运行编译后的程序"""
        print("🚀 运行CHTL编译器...")
        
        # 查找可执行文件
        if sys.platform == "win32":
            exe_path = self.build_dir / "Debug" / "chtl.exe"
            if not exe_path.exists():
                exe_path = self.build_dir / "Release" / "chtl.exe"
            if not exe_path.exists():
                exe_path = self.build_dir / "chtl.exe"
        else:
            exe_path = self.build_dir / "chtl"
        
        if not exe_path.exists():
            print("❌ 未找到可执行文件，请先编译项目")
            return False
        
        # 运行程序
        cmd = [str(exe_path)]
        if args:
            cmd.extend(args)
        
        try:
            subprocess.run(cmd, check=True)
            return True
        except subprocess.CalledProcessError as e:
            print(f"❌ 程序运行失败: {e}")
            return False

def main():
    parser = argparse.ArgumentParser(description="CHTL项目构建脚本")
    parser.add_argument("action", 
                       choices=["clean", "configure", "build", "test", "run", "all"],
                       help="要执行的操作")
    parser.add_argument("--build-type", "-t",
                       choices=["Debug", "Release"],
                       default="Debug",
                       help="构建类型")
    parser.add_argument("--jobs", "-j",
                       type=int,
                       help="并行编译的作业数")
    parser.add_argument("--root", "-r",
                       help="项目根目录路径")
    parser.add_argument("--args",
                       nargs=argparse.REMAINDER,
                       help="传递给程序的参数")
    
    args = parser.parse_args()
    
    # 创建构建器
    builder = CHTLBuilder(args.root)
    
    # 执行操作
    if args.action == "clean":
        builder.clean()
    elif args.action == "configure":
        builder.configure(args.build_type)
    elif args.action == "build":
        builder.build(args.jobs)
    elif args.action == "test":
        builder.test()
    elif args.action == "run":
        builder.run(args.args)
    elif args.action == "all":
        builder.clean()
        if builder.configure(args.build_type):
            if builder.build(args.jobs):
                builder.test()
    
    print("✨ 完成")

if __name__ == "__main__":
    main()
