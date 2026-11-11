"""Auditoría estática reproducible; no ejecuta ni desensambla nuevos binarios."""
from pathlib import Path
import re,csv,hashlib
from collections import Counter
O=Path(__file__).resolve().parents[1];R=O/'evidencia/resultados/originales';T=O/'tablas/resultados';E=O/'evidencia/resultados/extractos_revision';E.mkdir(exist_ok=True)
D={};rows=[];matches=[]
def isq(x):return bool(re.search(r'\bq[0-7]\b',x['operandos']))
def cats(x):
 m=x['instruccion'];q=isq(x)
 return {'P':m.startswith('vctp.'),'L':m.startswith('vldrb') and q,'M':m=='vmlava.s8','R':m=='vaddva.s8','FQ':m in ['vmul.f32','vmult.f32','vadd.f32','vfma.f32'] and q,'FS':m in ['vmul.f32','vadd.f32','vfma.f32'] and bool(re.search(r'\bs\d+\b',x['operandos']))}
for p in sorted(R.glob('*/InstruccionesBinario/*.list')):
 fn='';section='';seq=[]
 for n,line in enumerate(p.read_text().splitlines(),1):
  m=re.match(r'Disassembly of section (.*):',line)
  if m:section=m[1]
  m=re.match(r'^([0-9a-f]+) <([^>]+)>:',line)
  if m:fn=m[2]
  m=re.match(r'^\s*([0-9a-f]+):\s+(?:[0-9a-f]{4,8}\s+)+\s*([a-z][a-z0-9.]*)\s*(.*)$',line)
  if m and section=='.text':
   x=dict(archivo=str(p.relative_to(R)),linea=n,direccion=m[1],funcion=fn,instruccion=m[2],operandos=m[3]);seq.append(x)
   if any(cats(x).values()) or isq(x):matches.append(x)
 D[p.name]=seq
 counts=Counter(k for x in seq for k,v in cats(x).items() if v)
 rows.append(dict(archivo=str(p.relative_to(R)),sha256=hashlib.sha256(p.read_bytes()).hexdigest(),**{k:counts[k] for k in ['P','L','M','R','FQ','FS']}))
for name,rr in [('instrucciones_presencia_conteos.csv',rows),('instrucciones_coincidencias.csv',matches)]:
 with (T/name).open('w',newline='') as f:w=csv.DictWriter(f,fieldnames=rr[0].keys());w.writeheader();w.writerows(rr)
rx=[('PTQ INT8','M55','PTQ_INT8_CM55'),('PTQ INT8','NPU','PTQ_INT8_NPU'),('INT16x8','M55',None),('INT16x8','NPU','PTQ_INT16x8_NPU'),('Mixta-5','M55','PTQ_MIX_INT8x32_SELECTIVE_CM55'),('Mixta-5','NPU','PTQ_MIX_INT8x32_SELECTIVE_NPU'),('QAT INT8','M55','QAT_CM55'),('QAT INT8','NPU','QAT_NPU')]
mpc=[('Float32','M55','Float32_CM55'),('INT8 tensor','M55','PTQ_int8_TENSOR_CM55'),('INT8 tensor','NPU','PTQ_int8_TENSOR_FULL_NPU'),('INT8 canal','M55','PTQ_int8_CHANNEL_CM55'),('INT8 canal','NPU','PTQ_int8_CHANNEL_NPU'),('QAT INT8','M55','QAT_INT8_FULL_CM55'),('QAT INT8','NPU','QAT_INT8_FULL_NPU'),('INT16x8','M55','PTQ_INT16x8_CM55'),('Mixta','M55','PTQ_MIXED_SELECTIVED_CM55'),('Mixta','NPU','PTQ_MIXED_SELECTIVED_NPU')]
for case,variants,folder in [('rx',rx,'CasoRadiografia'),('mpc',mpc,'CasoMPC')]:
 lines=[r'\begin{table}[H]\centering\small',r'\caption{Presencia de familias de instrucciones en los listados de '+('radiografías' if case=='rx' else 'MPC')+r'.}\label{tab:helium_'+case+'}',r'\begin{tabularx}{\linewidth}{Xlcccccc}\toprule',r'Modelo & Ruta & P & L & M & R & FQ & FS\\\midrule']
 for model,route,key in variants:
  r=next((r for r in rows if r['archivo']==folder+'/InstruccionesBinario/STM32N6570-DK_Appli_'+str(key)+'.list'),None)
  vals=['NE']*6 if r is None else ['Sí' if r[k]>0 else 'No' for k in ['P','L','M','R','FQ','FS']]
  lines.append(model+' & '+route+' & '+' & '.join(vals)+r'\\')
 lines += [r'\bottomrule\end{tabularx}',r'\par\smallskip\begin{minipage}{\linewidth}\footnotesize P: \texttt{vctp}; L: cargas \texttt{vldrb*} con Q; M: \texttt{vmlava.s8}; R: \texttt{vaddva.s8}; FQ: aritmética Float32 con Q; FS: aritmética Float32 con S. Sí/No indica presencia/ausencia en el listado completo, no frecuencia de ejecución. NE: no evaluable por ausencia del listado.\end{minipage}',r'\fuente{auditoría de las instrucciones desensambladas en los archivos originales}',r'\end{table}']
 (T/f'helium_{case}.tex').write_text('\n'.join(lines)+'\n')
# Sólo se transcriben instrucciones de direcciones existentes; las selecciones no contiguas se separan explícitamente.
specs=[
 ('rx_int8_conv','PTQ_INT8_CM55','st_sssa8_ch_nn_mat_mult_nt_t_s8','vmlava.s8',-4,12),
 ('rx_mixed_dense','PTQ_MIX_INT8x32_SELECTIVE_CM55','arm_nn_vec_mat_mult_t_per_ch_s8','vmlava.s8',-4,10),
 ('rx_mixed_float','PTQ_MIX_INT8x32_SELECTIVE_CM55','forward_lite_conv2d_if32of32wf32','vmul.f32',-2,9),
 ('rx_npu_convert','PTQ_INT8_NPU','lite_convert_is8of32','vcvt.f32.s32',-1,8),
 ('mpc_npu_float','PTQ_MIXED_SELECTIVED_NPU','forward_lite_conv2d_if32of32wf32_group','vmul.f32',-2,9)]
manifest=[]
for name,key,fn,mn,offset,count in specs:
 seq=[x for x in D['STM32N6570-DK_Appli_'+key+'.list'] if x['funcion']==fn]
 ix=next(i for i,x in enumerate(seq) if x['instruccion']==mn and (mn not in ['vmul.f32','vcvt.f32.s32'] or isq(x)))+offset
 chosen=seq[ix:ix+count]
 (E/(name+'.asm')).write_text('\n'.join(f"{x['direccion']}: {x['instruccion']:<13} {x['operandos'].split('@')[0].strip()}" for x in chosen)+'\n')
 manifest.append(dict(extracto=name,archivo=chosen[0]['archivo'],funcion=fn,linea_inicio=chosen[0]['linea'],linea_fin=chosen[-1]['linea'],direccion_inicio=chosen[0]['direccion'],direccion_fin=chosen[-1]['direccion']))
with (T/'instrucciones_extractos.csv').open('w',newline='') as f:w=csv.DictWriter(f,fieldnames=manifest[0].keys());w.writeheader();w.writerows(manifest)
print('17 listados STM32N6 auditados. Tablas y cinco extractos generados.')
