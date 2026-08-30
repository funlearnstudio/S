from pathlib import Path

path = Path('src/main.cpp')
text = path.read_text()
old = 'SE 0.6.0-dev'
count = text.count(old)
if count != 3:
    raise SystemExit(f'expected 3 version strings, found {count}')
path.write_text(text.replace(old, 'SE 0.6.0'))
print('finalized SE 0.6.0 version strings')
