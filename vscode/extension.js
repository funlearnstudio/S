const vscode = require('vscode');
function run(command) {
  const editor = vscode.window.activeTextEditor;
  if (!editor || editor.document.languageId !== 'se') {
    vscode.window.showErrorMessage('Open an SE .se file first.');
    return;
  }
  editor.document.save();
  const terminal = vscode.window.createTerminal('SE');
  terminal.show();
  const file = editor.document.uri.fsPath.replace(/"/g, '\\"');
  terminal.sendText(`se ${command} "${file}"`);
}
function activate(context) {
  context.subscriptions.push(vscode.commands.registerCommand('se.run', () => run('run')));
  context.subscriptions.push(vscode.commands.registerCommand('se.check', () => run('check')));
  context.subscriptions.push(vscode.commands.registerCommand('se.build', () => run('build')));
}
function deactivate() {}
module.exports = { activate, deactivate };
