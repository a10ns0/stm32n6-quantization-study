from pathlib import Path
import re,csv,hashlib
O=Path(__file__).resolve().parents[1];R=O/'evidencia/resultados/originales';T=O/'tablas/resultados';E=O/'evidencia/resultados/extractos_revision'
D={};rows=[];match=[]
for p in sorted(R.rglob('*.list')):
 fn='';sect='';seq=[]
 for n,line in enumerate(p.read_text(errors='replace').splitlines(),1):
  m=re.match(r'Disassembly of section (.*):',line)
  if m:sect=m[1]
  m=re.match(r'^([0-9a-f]+) <([^>]+)>:',line)
  if m:fn=m[2]
  m=re.match(r'^\s*([0-9a-f]+):\s+(?:[0-9a-f]{4,8}\s+)+\s*([a-z][a-z0-9.]*)\s*(.*)$',line)
  if m and sect=='.text':seq.append(dict(archivo=str(p.relative_to(R)),linea=n,direccion=m[1],funcion=fn,instruccion=m[2],operandos=m[3]))
 key=str(p.relative_to(R));D[key]=seq
 def flags(x):
  m=x['instruccion'];a=x['operandos'];return [bool(re.search(r'\bq[0-7]\b',a)),m.startswith(('vctp','vpst','vpt')),m=='smlad',m in ['sxtb16','sxtab16'],m=='qadd16',bool(re.search(r'\bs\d+\b',a)) and m in ['vmul.f32','vfma.f32','vmla.f32'],m in ['mla','mul','mul.w','muls','mls']]
 counts=[sum(flags(x)[i] for x in seq) for i in range(7)]
 rows.append(dict(archivo=key,sha256=hashlib.sha256(p.read_bytes()).hexdigest(),**dict(zip(['Q','pred_MVE','SMLAD','SXTB16_SXTAB16','QADD16','FP_scalar','MUL_MLA_scalar'],counts))))
 match.extend(x for x in seq if any(flags(x)))
for name,rr in [('auditoria_23_listados.csv',rows),('instrucciones_23_coincidencias.csv',match)]:
 with (T/name).open('w',newline='') as f:w=csv.DictWriter(f,fieldnames=rr[0].keys());w.writeheader();w.writerows(rr)
keys=[('RX QAT','NUCLEO-L476RG/CasoRadiografia/InstruccionesBinario/NUCLEO-L476RG_QAT.list'),('MPC INT8 tensor','NUCLEO-L476RG/CasoMPC/InstruccionesBinario/NUCLEO-L476RG_PTQ_PER_TENSOR.list'),('MPC INT8 canal','NUCLEO-L476RG/CasoMPC/InstruccionesBinario/NUCLEO-L476RG_PTQ_PER_CHANNEL.list'),('MPC QAT','NUCLEO-L476RG/CasoMPC/InstruccionesBinario/NUCLEO-L476RG_QAT.list'),('MPC INT16x8','NUCLEO-L476RG/CasoMPC/InstruccionesBinario/NUCLEO-L476RG_PTQ_16x8.list'),('MPC mixta','NUCLEO-L476RG/CasoMPC/InstruccionesBinario/NUCLEO-L476RG_MIXED_SELECTIVED.list')]
s=[r'\begin{table}[H]\centering\small',r'\caption{Auditoría de los seis listados NUCLEO-L476RG.}\label{tab:list_nucleo}',r'\begin{tabularx}{\linewidth}{Xccccc}\toprule',r'Modelo & Q/MVE & SMLAD & SXTB16 & FP con S & MUL/MLA\\\midrule']
for name,key in keys:
 r=next(x for x in rows if x['archivo']==key);assert r['Q']==r['pred_MVE']==0
 vals=['No']+['Sí' if r[k] else 'No' for k in ['SMLAD','SXTB16_SXTAB16','FP_scalar','MUL_MLA_scalar']];s.append(name+' & '+' & '.join(vals)+r'\\')
s.extend([r'\bottomrule\end{tabularx}',r'\par\smallskip\begin{minipage}{\linewidth}\footnotesize Q/MVE: operandos Q y predicación vectorial; SXTB16 incluye SXTAB16; FP con S: aritmética flotante escalar. SMLAD es producto-acumulación dual DSP, no Helium. La presencia se refiere al ejecutable completo.\end{minipage}',r'\fuente{auditoría estática de los ejecutables; coordenadas y conteos conservados en CSV}',r'\end{table}']);(T/'helium_nucleo.tex').write_text('\n'.join(s))
spec=[('nucleo_rx_dsp',keys[0][1],'st_sssa8_ch_nn_mat_mult_kernel_opt','smlad'),('nucleo_mpc_dsp',keys[1][1],'st_sssa8_fully_connected','smlad'),('nucleo_mpc_float',keys[4][1],'forward_lite_dense_if32of32wf32','vfma.f32'),('n6_qat_mve','CasoRadiografia/InstruccionesBinario/STM32N6570-DK_Appli_QAT_CM55.list','st_sssa8_ch_nn_mat_mult_nt_t_s8','vmlava.s8')]
manifest=[]
for name,key,fn,mn in spec:
 seq=[x for x in D[key] if x['funcion']==fn];ix=next(i for i,x in enumerate(seq) if x['instruccion']==mn);chosen=seq[max(0,ix-3):ix+7]
 (E/(name+'.asm')).write_text('\n'.join(f"{x['direccion']}: {x['instruccion']:<12} {x['operandos'].split('@')[0].strip()}" for x in chosen)+'\n')
 manifest.append(dict(extracto=name,archivo=key,funcion=fn,linea_inicio=chosen[0]['linea'],linea_fin=chosen[-1]['linea']))
with (T/'extractos_placas.csv').open('w',newline='') as f:w=csv.DictWriter(f,fieldnames=manifest[0].keys());w.writeheader();w.writerows(manifest)
print('Listados revisados:',len(rows));print('\n'.join(str(x) for x in manifest))
