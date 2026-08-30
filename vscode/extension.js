const vscode = require('vscode');

let seTerminal;

const SE_COMPLETIONS = [
  // Core language.
  ['say', 'keyword', 'Output a value.'],
  ['ask', 'keyword', 'Read user input.'],
  ['make', 'keyword', 'Define a function.'],
  ['give', 'keyword', 'Return a value.'],
  ['if', 'keyword', 'Start a conditional branch.'],
  ['else', 'keyword', 'Start an alternative branch.'],
  ['repeat', 'keyword', 'Repeat a block a fixed number of times.'],
  ['for', 'keyword', 'Loop over values.'],
  ['while', 'keyword', 'Repeat while a condition is true.'],
  ['in', 'keyword', 'Used for iteration and membership.'],
  ['use', 'keyword', 'Import an SE module.'],
  ['type', 'keyword', 'Define a user type.'],
  ['match', 'keyword', 'Match a value.'],
  ['case', 'keyword', 'Define a match case.'],
  ['try', 'keyword', 'Handle a recoverable failure.'],
  ['fail', 'keyword', 'Create a failure.'],
  ['await', 'keyword', 'Wait for an asynchronous task.'],
  ['and', 'keyword', 'Logical AND.'],
  ['or', 'keyword', 'Logical OR.'],
  ['not', 'keyword', 'Logical NOT.'],
  ['true', 'value', 'Boolean true.'],
  ['false', 'value', 'Boolean false.'],

  // Common standard-library modules.
  ['http', 'module', 'HTTP client and server utilities.'],
  ['json', 'module', 'JSON parsing and serialization.'],
  ['db', 'module', 'Database utilities and adapters.'],
  ['math', 'module', 'Mathematics utilities.'],
  ['collections', 'module', 'Collection helpers.'],
  ['file', 'module', 'File utilities.'],
  ['path', 'module', 'Path utilities.'],
  ['time', 'module', 'Time utilities.'],
  ['os', 'module', 'Operating-system utilities.'],

  // SE Web contextual words.
  ['html', 'web', 'HTML structure inside an SE Web component.'],
  ['css', 'web', 'CSS styles inside an SE Web component.'],
  ['js', 'web', 'Browser behavior inside an SE Web component.'],
  ['style', 'web', 'Component styling.'],
  ['page', 'web', 'Define a web page.'],
  ['when', 'web', 'Handle an event.']
];

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

function completionKind(type) {
  switch (type) {
    case 'keyword':
      return vscode.CompletionItemKind.Keyword;
    case 'module':
      return vscode.CompletionItemKind.Module;
    case 'web':
      return vscode.CompletionItemKind.Property;
    case 'value':
      return vscode.CompletionItemKind.Value;
    default:
      return vscode.CompletionItemKind.Text;
  }
}

function getDocumentSymbols(document) {
  const symbols = new Map();

  for (let lineNumber = 0; lineNumber < document.lineCount; lineNumber++) {
    const line = document.lineAt(lineNumber).text;
    const code = line.split('#')[0];

    const functionMatch = code.match(/^\s*make\s+([A-Za-z_][A-Za-z0-9_]*)/);
    if (functionMatch) {
      symbols.set(functionMatch[1], {
        kind: vscode.CompletionItemKind.Function,
        detail: 'SE function'
      });
    }

    const typeMatch = code.match(/^\s*type\s+([A-Za-z_][A-Za-z0-9_]*)/);
    if (typeMatch) {
      symbols.set(typeMatch[1], {
        kind: vscode.CompletionItemKind.Class,
        detail: 'SE type'
      });
    }

    const variableMatch = code.match(/^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=/);
    if (variableMatch) {
      symbols.set(variableMatch[1], {
        kind: vscode.CompletionItemKind.Variable,
        detail: 'SE variable'
      });
    }
  }

  return symbols;
}

function createCompletionProvider() {
  return {
    provideCompletionItems(document) {
      const items = [];

      for (const [word, type, description] of SE_COMPLETIONS) {
        const item = new vscode.CompletionItem(word, completionKind(type));
        item.detail = `SE ${type}`;
        item.documentation = new vscode.MarkdownString(description);
        item.sortText = `0-${word}`;
        items.push(item);
      }

      for (const [name, info] of getDocumentSymbols(document)) {
        const item = new vscode.CompletionItem(name, info.kind);
        item.detail = info.detail;
        item.sortText = `1-${name}`;
        items.push(item);
      }

      return items;
    }
  };
}

function activate(context) {
  const completionProvider = vscode.languages.registerCompletionItemProvider(
    { language: 'se', scheme: 'file' },
    createCompletionProvider()
  );

  context.subscriptions.push(
    completionProvider,
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
