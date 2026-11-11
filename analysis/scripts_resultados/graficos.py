from pathlib import Path
exec((Path(__file__).parent/'analizar.py').read_text().split("print(logs[")[0])
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
plt.rcParams.update({'font.family':'DejaVu Sans','font.size':10,'axes.spines.top':False,'axes.spines.right':False,'axes.grid':True,'grid.alpha':.2,'pdf.fonttype':42,'figure.constrained_layout.use':True})
C=['#1e4e79','#1c897f','#d68b24','#775999','#b24b44','#566574']
def save(fig,name):
 from io import BytesIO
 b=BytesIO();fig.savefig(b,format='pdf',bbox_inches='tight');(F/(name+'.pdf')).write_bytes(b.getvalue());fig.savefig(F/(name+'.png'),dpi=180,bbox_inches='tight');plt.close(fig)
labels=rx.Modelo.tolist()
fig,ax=plt.subplots(1,2,figsize=(9,3.7))
for i,(col,title) in enumerate([('Accuracy','Exactitud (%)'),('Especificidad','Especificidad (%)')]):
 vals=rx[col]*100;b=ax[i].barh(labels,vals,color=C);ax[i].set_xlim(0,105);ax[i].set_xlabel(title);ax[i].bar_label(b,fmt='%.2f',padding=3,fontsize=9);ax[i].invert_yaxis()
save(fig,'rx_metricas')
fig,axs=plt.subplots(2,3,figsize=(9,5.7))
for ax,(_,r) in zip(axs.flat,rx.iterrows()):
 cm=np.array([[r.TN,r.FP],[r.FN,r.TP]],int);ax.imshow(cm,cmap='Blues',vmin=0,vmax=390);ax.set_title(r.Modelo);ax.grid(False);ax.set_xticks([0,1],['NORMAL','NEUMONÍA'],fontsize=8);ax.set_yticks([0,1],['NORMAL','NEUMONÍA'],fontsize=8)
 for (i,j),v in np.ndenumerate(cm):ax.text(j,i,f'{v}\n({100*v/cm[i].sum():.1f}%)',ha='center',va='center',color='white' if v>200 else '#172c40',fontsize=10)
for ax in axs[-1]:ax.set_xlabel('Predicción')
for ax in axs[:,0]:ax.set_ylabel('Clase real')
save(fig,'rx_confusiones')
fig,axs=plt.subplots(1,2,figsize=(9,3.8));q=pd.read_csv(P/'historial_qat.csv');ep=q.epoca_indice_0+1
for a,cols,lab in [(axs[0],['loss','val_loss'],'Pérdida'),(axs[1],['accuracy','val_accuracy'],'Exactitud')]:
 for c,color in zip(cols,C):a.plot(ep,q[c],'.-',label='Ajuste' if c==cols[0] else 'Validación interna',color=color)
 a.axvline(4,color=C[2],ls='--',label='Pesos restaurados: época 4');a.set(xlabel='Época',ylabel=lab);a.xaxis.set_major_locator(MaxNLocator(integer=True))
axs[0].legend(fontsize=8);save(fig,'rx_qat')
fig,axs=plt.subplots(1,2,figsize=(9,3.8))
for ax,col,lab in zip(axs,['TFLite_KiB','Latencia_host_media_ms'],['Archivo TFLite (KiB)','Latencia media en PC (ms)']):
 b=ax.barh(labels,rx[col],color=C);ax.invert_yaxis();ax.set_xlabel(lab)
 if col.startswith('Latencia'):ax.set_xscale('log')
 ax.bar_label(b,fmt='%.2f',fontsize=8,padding=3);ax.margins(x=.25)
save(fig,'rx_recursos_host')
for case,prefix,keys,names in [('CasoRadiografia','rx',['PTQ_INT8','PTQ_INT16x8','PTQ_MIX','Clasificador_Neumonia'],['PTQ INT8','INT16x8 importado','Mixta-5','QAT INT8']),('CasoMPC','mpc',['float32','ptq_tensor','ptq_channel','qat_int8','ptq_int16x8','ptq_mixed'],['Float32','INT8 tensor','INT8 canal','QAT INT8','INT16x8 importado','Mixta'])]:
 df=logs[logs.caso==case];fig,ax=plt.subplots(figsize=(8,3.8));y=np.arange(len(keys))
 for j,(route,color) in enumerate([('cm55',C[0]),('npu',C[1])]):
  vals=[]
  for k in keys:
   rr=df[df.archivo.str.lower().str.startswith(k.lower())&df.archivo.str.lower().str.contains(route)]
   vals.append(rr.media_ms.iloc[0] if len(rr) else np.nan)
  b=ax.barh(y+(j-.5)*.33,vals,height=.32,color=color,label='Cortex-M55' if route=='cm55' else 'Ruta Neural-ART');ax.bar_label(b,fmt='%.3f',padding=2,fontsize=8)
 ax.set_yticks(y,names);ax.invert_yaxis();ax.set_xlabel('Inferencia media en placa (ms)');ax.legend(loc='lower right',fontsize=8)
 if case=='CasoRadiografia':ax.set_xscale('log');ax.set_xlim(.25,220)
 else:ax.set_xlim(0,.59)
 save(fig,prefix+'_latencia_placa')
df=logs[logs.hw_pct.notna()].copy();names=[]
for case,n in zip(df.caso,df.archivo):
 names.append(('RX ' if case=='CasoRadiografia' else 'MPC ')+('mixta' if 'mix' in n.lower() else 'INT16x8' if '16x8' in n else 'QAT' if 'qat' in n else 'INT8 canal' if 'channel' in n else 'INT8 tensor' if 'tensor' in n else 'INT8'))
fig,ax=plt.subplots(figsize=(8,4));left=np.zeros(len(df))
for col,colr,label in [('hw_pct',C[1],'HW'),('sw_pct',C[0],'SW'),('ctrl_pct',C[2],'Control SW')]:
 ax.barh(names,df[col],left=left,color=colr,label=label);left+=df[col]
ax.set(xlabel='Distribución temporal del perfil (%)',xlim=(0,100));ax.legend(loc='upper center',bbox_to_anchor=(.5,-.17),ncol=3);ax.invert_yaxis();save(fig,'perfil_hw_sw')
mpc=pd.read_csv(T/'mpc_baseline_out83_tabla0.csv');mpc.to_csv(T/'mpc_simulacion_resumen.csv',index=False)
fig,ax=plt.subplots(figsize=(8,3.8));ns=['Float32 Keras','INT8 tensor','INT8 canal','QAT INT8','INT16x8','Mixta'];v=mpc['Success_Rate_%'];b=ax.barh(ns,v,color=C);ax.bar_label(b,fmt='%.1f%%',padding=3);ax.set(xlabel='Éxito en 200 condiciones iniciales (%)',xlim=(0,115));ax.invert_yaxis();save(fig,'mpc_exito')
for j,idxs in enumerate([[(84,88),(91,95),(97,101)],[(103,107),(109,113),(115,119)]]):
 fig,axs=plt.subplots(3,2,figsize=(10,6.5),layout='constrained')
 for row,pair in enumerate(idxs):
  for col,idx in enumerate(pair):axs[row,col].imshow(plt.imread(F/f'mpc_baseline_out{idx}_0.png'));axs[row,col].axis('off')
 save(fig,'mpc_trayectorias_'+str(j+1))
# Show the selected quantization-layer diagnostic already present in the HTML.
rank=pd.read_csv(T/'mpc_baseline_out75_tabla0.csv');fig,ax=plt.subplots(figsize=(8,3.5));ax.barh(rank.layer,rank.rmse_scale,color=C[0]);ax.axvline(.32,color=C[2],ls='--',label='Umbral 0,32');ax.invert_yaxis();ax.set_xlabel('RMSE / escala de cuantización (máximo por capa)');ax.legend();save(fig,'mpc_sensibilidad')
angles=[.1,-.1,.3,-.3,1.,-1.,3.13,-3.13]
for var,title,lim,fn in [('y','Posición del carro (m)',.1,'hil_posiciones'),('e','Error angular (rad)',.17,'hil_angulos')]:
 fig,axs=plt.subplots(4,2,figsize=(8.5,9.2),sharex=True)
 for ax,a in zip(axs.flat,angles):
  s=series[a];t=np.arange(101)*.16;vals=s['X'][:,0] if var=='y' else s['e'];ax.plot(t,vals,color=C[0],lw=1);ax.axvspan(14.56,16,color=C[1],alpha=.1)
  for sign in [-1,1]:ax.axhline(sign*lim,color=C[2],ls='--',lw=.7)
  if var=='y':
   for sign in [-1,1]:ax.axhline(sign*.4,color=C[4],ls=':',lw=.7)
   ax.set_ylim(-.43,.43)
  else:ax.set_ylim(-3.4,3.4)
  ax.set_title(f'α₀ = {a:+.2f} rad · '+('cumple' if s['success'] else 'no cumple'),fontsize=9);ax.set_xlim(0,16)
 for ax in axs[-1]:ax.set_xlabel('Tiempo simulado (s)')
 for ax in axs[:,0]:ax.set_ylabel(title)
 save(fig,fn)
fig,axs=plt.subplots(3,2,figsize=(9,7),sharex=True)
for j,a in enumerate([3.13,-.3]):
 s=series[a];t=np.arange(101)*.16
 axs[0,j].plot(t,s['X'][:,1],color=C[0],label='Ángulo continuo');axs[0,j].plot(t,s['e'],color=C[1],label='Error angular');axs[0,j].set_title(f'α₀ = {a:+.2f} rad')
 axs[1,j].plot(t,s['X'][:,0],color=C[0]);axs[2,j].step(t,np.r_[s['u'],s['u'][-1]],where='post',color=C[0])
 for ax in axs[:,j]:ax.axvspan(14.56,16,color=C[1],alpha=.1)
 for v in [-.1,.1]:axs[1,j].axhline(v,color=C[2],ls='--',lw=.8)
 for v in [-9,9]:axs[2,j].axhline(v,color=C[2],ls='--',lw=.8)
 axs[2,j].set_xlabel('Tiempo simulado (s)')
axs[0,0].legend(fontsize=8);axs[0,0].set_ylabel('Ángulo (rad)');axs[1,0].set_ylabel('Posición (m)');axs[2,0].set_ylabel('Control retenido (V)');save(fig,'hil_detalles')
fig,axs=plt.subplots(1,2,figsize=(9,3.7))
for ax,field,mul,title in [(axs[0],'inference_us',1,'Inferencia (µs)'),(axs[1],'rtt_us',.001,'RTT UART (ms)')]:
 ax.boxplot([series[a]['df'][field]*mul for a in angles],tick_labels=[f'{a:+.2f}' for a in angles],showfliers=True,flierprops={'markersize':2});ax.set(xlabel='Ángulo inicial (rad)',ylabel=title);ax.tick_params(axis='x',rotation=45)
save(fig,'hil_tiempos')
fig,axs=plt.subplots(1,2,figsize=(8,3.4));ud=pd.DataFrame(ur);cm=np.array([[4,4],[0,8]]);axs[0].imshow(cm,cmap='Blues',vmin=0,vmax=8);axs[0].grid(False);axs[0].set_xticks([0,1],['NORMAL','NEUMONÍA']);axs[0].set_yticks([0,1],['NORMAL','NEUMONÍA']);axs[0].set(xlabel='Predicción',ylabel='Clase real')
for (i,j),v in np.ndenumerate(cm):axs[0].text(j,i,str(v),ha='center',va='center',color='white' if v>4 else 'black',fontsize=16)
axs[1].plot(ud.id,ud.latency_us/1000,'.-',color=C[0]);axs[1].set(xlabel='Imagen enviada',ylabel='Inferencia informada (ms)',ylim=(104.5,106.6));axs[1].set_xticks(range(1,17));axs[1].tick_params(axis='x',labelsize=8);save(fig,'rx_uart')
print('Gráficos generados')
