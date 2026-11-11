"""Integra registros de octubre: sólo calcula sobre archivos, sin inferencia nueva."""
from pathlib import Path
import re,csv,json,hashlib,importlib.util
import numpy as np,pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from io import BytesIO
O=Path(__file__).resolve().parents[1];R=O/'evidencia/resultados/originales';T=O/'tablas/resultados';F=O/'figuras/resultados';E=O/'evidencia/resultados/extractos_revision'
# Reutilizar el formateador de tablas sin recrear las tablas anteriores.
s=(Path(__file__).parent/'tablas.py').read_text();ns={'__file__':str(Path(__file__).parent/'tablas.py')};exec(s.split("table('rx_metricas'")[0],ns);table=ns['table'];num=ns['num'];integer=ns['integer'];sci=ns['sci']
D=pd.read_csv(T/'validacion_st.csv');extra=[]
def metrics(p,board,case):
 s=p.read_text();m=re.search(r'duration\s+:\s+([\d.]+) ms by sample \(([\d.]+)/([\d.]+)/([\d.]+)\)',s)
 if not m:return None
 def cap(pat):
  v=re.search(pat,s);return v[1] if v else ''
 r=dict(board=board,caso=case,archivo=str(p.relative_to(R)),media_ms=float(m[1]),min_ms=float(m[2]),max_ms=float(m[3]),std_ms=float(m[4]),n=int(cap(r'nb sample\(s\)\s+:\s+(\d+)')),hash_modelo=cap(r'model_hash\s+:\s+(\S+)'),pesos_B_generate=int(cap(r'weights \(ro\)\s+:\s+([\d,]+) B').replace(',','')),activaciones_B_generate=int(cap(r'activations \(rw\)\s+:\s+([\d,]+) B').replace(',','')))
 r['rmse_cross']=float(cap(r'X-cross #1\s+n.a.\s+([\d.]+)'));r['acc_target']=float(cap(r'TARGET c-model #1\s+([\d.]+)%') or 'nan')
 cy=cap(r'CPU cycles\s+:\s+\[([\d,]+)\]');r['cycles']=int(cy.replace(',','')) if cy else np.nan
 r['MHz']=int(cap(r'device.desc\s+:.*?@(\d+)/') or 0);r['valinput']=cap(r'--valinput\s+(.*?)\s+--');r['runtime']=cap(r'runtime lib\s+:\s+([^\n]+)').strip()
 return r
for p in R.glob('NUCLEO-L476RG/*/logs/*.txt'):
 r=metrics(p,'L476',p.parts[-3])
 if r:extra.append(r)
L=pd.DataFrame(extra);L.to_csv(T/'validacion_nucleo.csv',index=False)
base=[]
for p in R.glob('*/Validacion*/*.txt'):
 r=metrics(p,'N6',p.parts[-3])
 if r:base.append(r)
A=pd.DataFrame(base+extra);A.to_csv(T/'validacion_ambas_placas.csv',index=False)
variants=[('CasoRadiografia','QAT INT8','0xa51e9164608a6653e553be25592d8fdc'),('CasoMPC','INT8 tensor','0xf450f1d3534fadc60c108e50d7d242e4'),('CasoMPC','INT8 canal','0xe39346fc01d92f7a41610e1a04331c83'),('CasoMPC','QAT INT8','0x70958991e43fd724db0aa997f0f0f59e'),('CasoMPC','INT16x8 importado','0xb91b9c411f03bad774e409bda903abe1'),('CasoMPC','Mixta','0xcee2ba2777f72e6840ad1f6493d834c7')]
comp=[]
for case,label,h in variants:
 l=L[(L.caso==case)&(L.hash_modelo==h)].iloc[0];m=A[(A.board=='N6')&(A.caso==case)&(A.hash_modelo==h)&A.archivo.str.lower().str.contains('cm55')].iloc[0];nn=A[(A.board=='N6')&(A.caso==case)&(A.hash_modelo==h)&A.archivo.str.lower().str.contains('npu')];n=nn.iloc[0] if len(nn) else None
 r={'caso':case,'modelo':label,'hash_modelo':h,'l4_ms':l.media_ms,'m55_ms':m.media_ms,'npu_ms':n.media_ms if n is not None else np.nan,'ratio_l4_m55':l.media_ms/m.media_ms,'ratio_l4_npu':l.media_ms/n.media_ms if n is not None else np.nan,'cycles_l4':l.cycles,'cycles_m55':m.cycles,'ratio_cycles':l.cycles/m.cycles,'rmse_l4':l.rmse_cross,'rmse_m55':m.rmse_cross,'rmse_npu':n.rmse_cross if n is not None else np.nan}
 assert l.MHz==80 and m.MHz==800
 comp.append(r)
C=pd.DataFrame(comp);C.to_csv(T/'comparacion_placas.csv',index=False)
table('nucleo_validacion','Validación aislada en la NUCLEO-L476RG.','tab:nucleo_validacion',['Caso / modelo','$n$','Media (ms)','Mín.--máx. (ms)','RMSE cruzado'],[[('RX / ' if r['caso']=='CasoRadiografia' else 'MPC / ')+r['modelo'],int(L[(L.caso==r['caso'])&(L.hash_modelo==r['hash_modelo'])].iloc[0]['n']),num(r['l4_ms'],3),num(L[L.hash_modelo==r['hash_modelo']].iloc[0].min_ms,3)+'--'+num(L[L.hash_modelo==r['hash_modelo']].iloc[0].max_ms,3),sci(r['rmse_l4'])] for r in comp],'registros ST de NUCLEO-L476RG; no son ensayos en lazo cerrado')
table('comparacion_placas','Latencias medias de modelos con igual hash informado por ST.','tab:comparacion_placas',['Caso / modelo','L476 (ms)','M55 (ms)','NPU (ms)','$t_{L4}/t_{M55}$','$t_{L4}/t_{NPU}$'],[[('RX / ' if r['caso']=='CasoRadiografia' else 'MPC / ')+r['modelo'],num(r['l4_ms'],3),num(r['m55_ms'],3),'ND' if pd.isna(r['npu_ms']) else num(r['npu_ms'],3),num(r['ratio_l4_m55'],2),'ND' if pd.isna(r['npu_ms']) else num(r['ratio_l4_npu'],2)] for r in comp],'medias registradas; razones calculadas con los valores redondeados. ND: sin registro')
table('comparacion_ciclos','Ciclos de CPU informados en las dos rutas sin NPU.','tab:comparacion_ciclos',['Caso / modelo','L476, 80 MHz','M55, 800 MHz','$C_{L4}/C_{M55}$'],[[('RX / ' if r['caso']=='CasoRadiografia' else 'MPC / ')+r['modelo'],integer(r['cycles_l4']),integer(r['cycles_m55']),num(r['ratio_cycles'],3)] for r in comp],'campo CPU cycles de cada perfil; no se estiman ciclos a partir de una frecuencia hipotética')
table('nucleo_memoria','Pesos y activaciones de las implementaciones validadas en NUCLEO-L476RG.','tab:nucleo_memoria',['Caso / modelo','Pesos (B)','Activaciones (B)'],[[('RX / ' if case=='CasoRadiografia' else 'MPC / ')+label,integer(L[L.hash_modelo==h].iloc[0].pesos_B_generate),integer(L[L.hash_modelo==h].iloc[0].activaciones_B_generate)] for case,label,h in variants],'primer resumen de generación de cada registro ST')
plt.rcParams.update({'font.family':'DejaVu Sans','font.size':10,'axes.spines.top':False,'axes.spines.right':False,'axes.grid':True,'grid.alpha':.18,'pdf.fonttype':42})
colors=['#ad7134','#285b83','#258477']
def save(fig,name):
 b=BytesIO();fig.savefig(b,format='pdf',bbox_inches='tight');(F/(name+'.pdf')).write_bytes(b.getvalue());fig.savefig(F/(name+'.png'),dpi=160,bbox_inches='tight');plt.close(fig)
# Memoria: cada categoría conserva su escala, evitando equiparar RO+RW con SRAM total.
for case,tag in [('CasoRadiografia','rx'),('CasoMPC','mpc')]:
 n=D[(D.caso==case)&D.archivo.str.lower().str.contains('npu')].copy()
 def lab(fn):
  f=fn.lower();return 'QAT INT8' if 'qat' in f else 'INT16x8 importado' if '16x8' in f else 'Mixta-5' if ('mix' in f and tag=='rx') else 'Mixta' if 'mix' in f else 'INT8 canal' if 'channel' in f else 'INT8 tensor' if 'tensor' in f else 'PTQ INT8'
 labels=[lab(fn) for fn in n.archivo]
 fig,axs=plt.subplots(1,2,figsize=(9,3.5),layout='constrained')
 for ax,col,title,c in zip(axs,['pesos_B_generate','activaciones_B_generate'],['Pesos de red, RO (KiB)','Activaciones, RW (KiB)'],colors[1:]):
  v=n[col].to_numpy()/1024;b=ax.barh(labels,v,color=c);ax.bar_label(b,labels=[f'{x:.2f}' for x in v],padding=3,fontsize=9);ax.set_xlabel(title);ax.invert_yaxis();ax.set_xlim(0,max(v)*1.28);ax.set_axisbelow(True)
 save(fig,tag+'_memoria_npu')
# Comparación temporal, con ausencia explícita en lugar de cero.
r=C.iloc[0];fig,ax=plt.subplots(figsize=(8,3.7),layout='constrained');v=[r.l4_ms,r.m55_ms,r.npu_ms];b=ax.barh(['L476 · Cortex-M4','N657 · Cortex-M55','N657 · Neural-ART'],v,color=colors);ax.set_xscale('log');ax.bar_label(b,labels=[f'{x:.3f} ms' for x in v],padding=5);ax.set_xlim(.1,800);ax.set_xlabel('Inferencia QAT de radiografías (ms, escala logarítmica)');ax.invert_yaxis();save(fig,'rx_comparacion_placas')
mp=C[C.caso=='CasoMPC'];fig,ax=plt.subplots(figsize=(9,4.5),layout='constrained');y=np.arange(len(mp))
for j,(col,label,color) in enumerate(zip(['l4_ms','m55_ms','npu_ms'],['L476 · M4','N657 · M55','N657 · NPU'],colors)):
 v=mp[col].to_numpy();b=ax.barh(y+(j-1)*.24,v,height=.22,label=label,color=color);ax.bar_label(b,labels=['' if np.isnan(x) else f'{x:.3f}' for x in v],fontsize=8,padding=3)
ax.set_yticks(y,mp.modelo);ax.invert_yaxis();ax.set_xlabel('Inferencia media MPC (ms)');ax.set_xlim(0,3.3);ax.legend(fontsize=8,loc='lower right');ax.text(.05,3+.24,'NPU: sin registro',fontsize=8,va='center');save(fig,'mpc_comparacion_placas')
# Comparación de memoria de MPC entre los tres destinos (pesos y espacio de trabajo).
fig,axs=plt.subplots(1,2,figsize=(10,5),layout='constrained');vlabels=[v[1] for v in variants[1:]]
for ax,col,title in zip(axs,['pesos_B_generate','activaciones_B_generate'],['Pesos de red, RO (KiB)','Activaciones, RW (KiB)']):
 for j,(route,color) in enumerate(zip(['L476','M55','NPU'],colors)):
  vals=[]
  for case,label,h in variants[1:]:
   ss=A[(A.caso==case)&(A.hash_modelo==h)];ss=ss[ss.board=='L476'] if route=='L476' else ss[(ss.board=='N6')&ss.archivo.str.lower().str.contains('cm55' if route=='M55' else 'npu')];vals.append(ss.iloc[0][col]/1024 if len(ss) else np.nan)
  ax.barh(np.arange(5)+(j-1)*.24,vals,height=.22,label=route,color=color)
 ax.set_yticks(np.arange(5),vlabels);ax.invert_yaxis();ax.set_xlabel(title);ax.set_axisbelow(True)
axs[1].legend(loc='lower right');save(fig,'mpc_memoria_placas')
# QAT confusiones lado a lado, sin inferir identidades de muestras no suministradas.
fig,axs=plt.subplots(1,3,figsize=(9,3.3),layout='constrained')
for ax,title,cm in zip(axs,['Referencia / M55 / L476','NPU','Cambio neto de errores'],[np.array([[2,2],[0,4]]),np.array([[1,3],[0,4]]),None]):
 if cm is None:
  ax.axis('off');ax.text(.02,.75,'Lote: 8 imágenes\n\nCPU: 6 aciertos (75 %)\nNPU: 5 aciertos (62,5 %)\n\nUn falso positivo adicional',fontsize=11,va='top');continue
 ax.imshow(cm,cmap='Blues',vmin=0,vmax=4);ax.set_title(title,fontsize=10);ax.set_xticks([0,1],['NORMAL','NEUMONÍA'],fontsize=8);ax.set_yticks([0,1],['NORMAL','NEUMONÍA'],fontsize=8);ax.grid(False);ax.set_xlabel('Predicción')
 for (i,j),v in np.ndenumerate(cm):ax.text(j,i,str(v),ha='center',va='center',color='white' if v>=3 else 'black',fontsize=14)
axs[0].set_ylabel('Clase real');save(fig,'rx_qat_validacion_confusion')
print(C.to_string(index=False))
