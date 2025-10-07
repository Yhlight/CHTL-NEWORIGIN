const vscode = require('vscode');
const path = require('path');
const { exec } = require('child_process');
const fs = require('fs');

/**
 * 激活扩展
 */
function activate(context) {
    console.log('CHTL Language Support activated');

    // 注册编译命令
    let compileCommand = vscode.commands.registerCommand('chtl.compile', function () {
        compileCurrentFile(false);
    });

    // 注册内联编译命令
    let compileInlineCommand = vscode.commands.registerCommand('chtl.compileWithInline', function () {
        compileCurrentFile(true);
    });

    context.subscriptions.push(compileCommand);
    context.subscriptions.push(compileInlineCommand);
}

/**
 * 编译当前文件
 */
function compileCurrentFile(inline) {
    const editor = vscode.window.activeTextEditor;
    if (!editor) {
        vscode.window.showErrorMessage('No active editor');
        return;
    }

    const document = editor.document;
    if (document.languageId !== 'chtl') {
        vscode.window.showErrorMessage('Current file is not a CHTL file');
        return;
    }

    // 保存文件
    document.save().then(() => {
        const filePath = document.fileName;
        const workspaceFolder = vscode.workspace.getWorkspaceFolder(document.uri);
        
        // 查找CHTL编译器
        let chtlPath = findCHTLCompiler(workspaceFolder);
        
        if (!chtlPath) {
            vscode.window.showErrorMessage('CHTL compiler not found. Please build the project first.');
            return;
        }

        // 构建编译命令
        let command = `"${chtlPath}" "${filePath}"`;
        if (inline) {
            command += ' --inline --default-struct';
        }

        // 执行编译
        vscode.window.showInformationMessage('Compiling CHTL file...');
        
        exec(command, (error, stdout, stderr) => {
            if (error) {
                vscode.window.showErrorMessage(`Compilation failed: ${stderr}`);
                return;
            }

            vscode.window.showInformationMessage(`✅ Compilation successful!\n${stdout}`);
            
            // 打开生成的HTML文件
            const baseName = path.basename(filePath, '.chtl');
            const dirName = path.dirname(filePath);
            const htmlPath = path.join(dirName, `${baseName}.html`);
            
            if (fs.existsSync(htmlPath)) {
                vscode.workspace.openTextDocument(htmlPath).then(doc => {
                    vscode.window.showTextDocument(doc, vscode.ViewColumn.Beside);
                });
            }
        });
    });
}

/**
 * 查找CHTL编译器
 */
function findCHTLCompiler(workspaceFolder) {
    if (!workspaceFolder) {
        return null;
    }

    const workspacePath = workspaceFolder.uri.fsPath;
    
    // 查找可能的编译器位置
    const possiblePaths = [
        path.join(workspacePath, 'build', 'chtl'),
        path.join(workspacePath, 'build', 'Debug', 'chtl'),
        path.join(workspacePath, 'build', 'Release', 'chtl'),
        path.join(workspacePath, 'build', 'chtl.exe'),
        path.join(workspacePath, 'build', 'Debug', 'chtl.exe'),
        path.join(workspacePath, 'build', 'Release', 'chtl.exe'),
    ];

    for (const p of possiblePaths) {
        if (fs.existsSync(p)) {
            return p;
        }
    }

    // 尝试在PATH中查找
    return 'chtl';
}

/**
 * 停用扩展
 */
function deactivate() {
    console.log('CHTL Language Support deactivated');
}

module.exports = {
    activate,
    deactivate
};
