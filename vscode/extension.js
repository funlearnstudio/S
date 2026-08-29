const vscode = require('vscode');

let seTerminal;

function quote(value) {
  return `"${String(value).replace(/"/g, '\\"')}"`;
}

function terminalForRun() {
  const config = vscode.workspace.getConfiguration('se');
  const reuse = config.get('reuseTerminal', true);
  if (reuse) {
    if (!seTerminal || seTerminal.exitStatus) {
      seTerminal = vscode.window.createTerminal('SE');
    }
    return seTerminal;
  }
  return vscode.window.createTerminal('SE');
}

async function run(command) {
  const editor = vscode.window.activeTextEditor;
  if (!editor || editor.document.languageId !== 'se') {
    vscode.window.showErrorMessage('Open an SE .se file first.');
    return;
  }

  if (editor.document.isUntitled) {
    vscode.window.showErrorMessage('Save the SE file before running it.');
    return;
  }

  if (editor.document.isDirty) {
    const saved = await editor.document.save();
    if (!saved) {
      vscode.window.showErrorMessage('Could not save the SE file.');
      return;
    }
  }

  const config = vscode.workspace.getConfiguration('se');
  const executable = config.get('executablePath', 'se');
  const file = editor.document.uri.fsPath;
  const cwd = vscode.workspace.getWorkspaceFolder(editor.document.uri)?.uri.fsPath
    || require('path').dirname(file);

  const terminal = terminalForRun();
  terminal.show(true);
  terminal.sendText(`cd ${quote(cwd)} && ${quote(executable)} ${command} ${quote(file)}`, true);
}

function activate(context) {
  context.subscriptions.push(
    vscode.commands.registerCommand('se.run', () => run('run')),
    vscode.commands.registerCommand('se.check', () => run('check')),
    vscode.commands.registerCommand('se.build', () => run('build')),
    vscode.window.onDidCloseTerminal((terminal) => {
      if (terminal === seTerminal) seTerminal = undefined;
    })
  );
}

function deactivate() {
  if (seTerminal) seTerminal.dispose();
}

module.exports = { activate, deactivate };
