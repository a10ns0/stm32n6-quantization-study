"""Validate manifest, notebook JSON and protocol shape without executing notebooks."""
from pathlib import Path
import ast,csv,hashlib,json,struct,re
ROOT=Path(__file__).resolve().parents[1]
errors=[]
for row in csv.DictReader((ROOT/'audit/notebooks.csv').open(encoding='utf8')):
 p=ROOT/row['path']
 if hashlib.sha256(p.read_bytes()).hexdigest()!=row['sha256']:errors.append(str(p))
 nb=json.loads(p.read_text(encoding='utf8'))
 if nb.get('nbformat')!=4:errors.append('nbformat: '+str(p))
for p in list((ROOT/'analysis/scripts_resultados').glob('*.py'))+list((ROOT/'cases').rglob('*.py')):
 try:ast.parse(p.read_text(encoding='utf-8-sig'))
 except SyntaxError as e:errors.append(str(e))
assert struct.calcsize('<4sI4f')==24
assert struct.calcsize('<4sIffIIi')==28
packet=struct.pack('<4sI4f',b'MPC1',7,0.,3.13,0.,0.)
assert struct.unpack('<4sI4f',packet)[:2]==(b'MPC1',7)
for p in [ROOT/'README.md',*list((ROOT/'docs').glob('*.md')),*list((ROOT/'references').glob('*.md')),*list((ROOT/'cases').glob('*/README.md'))]:
 for dest in re.findall(r'\]\(([^)]+)\)',p.read_text(encoding='utf8')):
  if '://' in dest or dest.startswith('#'):continue
  if not (p.parent/dest.split('#')[0]).exists():errors.append(f'Link: {p.relative_to(ROOT)} -> {dest}')
if errors:raise SystemExit('\n'.join(errors))
print('OK: notebooks intactos, sintaxis Python, enlaces locales y tamaños de protocolo.')
print('PENDIENTE: ejecución neuronal, hardware, equivalencia firmware HIL y derechos de redistribución.')
