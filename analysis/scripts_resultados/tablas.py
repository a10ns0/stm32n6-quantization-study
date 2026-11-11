from pathlib import Path
import pandas as pd,numpy as np
O=Path(__file__).resolve().parents[1];T=O/'tablas/resultados';D=pd.read_csv(T/'validacion_st.csv');rx=pd.read_csv(T/'radiografia_comparacion.csv');H=pd.read_csv(T/'hil_resumen.csv')
def num(x,d=2):return f'{x:.{d}f}'.replace('.',',')
def integer(x):return f'{int(x):,}'.replace(',',r'\,')
def sci(x):
 if x==0:return '0'
 e=int(np.floor(np.log10(abs(x))));return '$'+num(x/10**e,3)+r'\times10^{'+str(e)+'}$'
def table(name,caption,label,headers,rows,source,cols=None):
 cols=cols or 'Y'+'r'*(len(headers)-1)
 s='\\begin{table}[H]\n\\centering\\small\n\\caption{'+caption+'}\\label{'+label+'}\n\\begin{tabularx}{\\linewidth}{'+cols+'}\\toprule\n'
 s+=' & '.join(headers)+r'\\\midrule'+'\n'
 s+='\n'.join(' & '.join(map(str,r))+r'\\' for r in rows)
 s+='\n\\bottomrule\\end{tabularx}\n\\fuente{'+source+'}\n\\end{table}\n';(T/(name+'.tex')).write_text(s)
table('rx_metricas','Desempeño de clasificación sobre 624 radiografías. Valores en porcentaje.','tab:xrayres',['Variante','Exact.','Balanceada','Sensib.','Especif.','$F_1$'],[[r.Modelo,*[num(r[c]*100) for c in ['Accuracy','Balanced_accuracy','Recall_sensibilidad','Especificidad','F1']]] for _,r in rx.iterrows()],r'\texttt{comparacion\_final\_modelos.csv}; comprobación con las predicciones almacenadas')
table('rx_recursos','Tamaño y latencia de los modelos evaluados en el computador.','tab:rx_recursos',['Variante','Archivo (B)','Red. (\%)','Media (ms)','P95 (ms)','RMSE salida'],[[r.Modelo,integer(r.TFLite_bytes),num(r['Compresion_%']),num(r.Latencia_host_media_ms,3),num(r.Latencia_host_p95_ms,3),num(r.RMSE_salida_vs_Float32,6)] for _,r in rx.iterrows()],r'\texttt{comparacion\_final\_modelos.csv}; reducción respecto de 95\,652 B')
p=pd.read_csv(T/'radiografia_predicciones_pareadas.csv').iloc[1:]
table('rx_pareadas','Cambios de decisión respecto de Float32 sobre las mismas imágenes.','tab:rx_pareadas',['Variante','Corrige','Introduce error','Decisiones distintas'],[[r.Modelo,r.solo_variante_correcto,r.solo_base_correcto,r.predicciones_distintas] for _,r in p.iterrows()],r'cálculo propio a partir de \texttt{predicciones\_*.npz}')
rxnames={'PTQ_INT8_CM55_log.txt':'INT8 / M55','PTQ_INT8_NPU.txt':'INT8 / Neural-ART','PTQ_INT16x8_CM55_log.txt':'INT16x8 / M55','PTQ_INT16x8_NPU_log.txt':'INT16x8 / Neural-ART','PTQ_MIX_INT8x32_SELECTIVE_CM55_log.txt':'Mixta-5 / M55','PTQ_MIX_INT8x32_SELECTIVE_NPU.txt':'Mixta-5 / Neural-ART'}
mpcnames={'float32_cm55.txt':'Float32 / M55','ptq_tensor_cm55.txt':'INT8 tensor / M55','ptq_tensor_npu.txt':'INT8 tensor / NPU','ptq_channel_cm55.txt':'INT8 canal / M55','ptq_channel_npu.txt':'INT8 canal / NPU','qat_int8_cm55.txt':'QAT / M55','qat_int8_npu.txt':'QAT / NPU','ptq_int16x8_cm55.txt':'INT16x8 / M55','ptq_mixed_selectived_cm55.txt':'Mixta / M55','ptq_mixed_selectived_npu.txt':'Mixta / NPU'}
rxnames.update({'Clasificador_Neumonia_run-9_logs_QAT_CM55.txt':'QAT / M55','Clasificador_Neumonia_run-8_logs_QAT_NPU.txt':'QAT / Neural-ART'})
for tag,names in [('rx',rxnames),('mpc',mpcnames)]:
 rows=[];mem=[]
 for fn,n in names.items():
  r=D[D.archivo==fn].iloc[0]
  row=[n,num(r.media_ms,3),num(r.min_ms,3)+'--'+num(r.max_ms,3),sci(r.rmse_cross)]
  if tag=='rx':row.append(num(r.acc_target,1))
  rows.append(row);mem.append([n,integer(r.pesos_B_generate),integer(r.activaciones_B_generate)])
 table(tag+'_placa','Validación de '+('radiografías sobre ocho entradas' if tag=='rx' else 'MPC sobre 200 entradas')+' en la STM32N6570-DK.','tab:'+tag+'_placa',['Variante / ruta','Media (ms)','Mín.--máx. (ms)','RMSE cruzado']+(['Exact. (\%)'] if tag=='rx' else []),rows,'registros de validación ST incluidos en el paquete experimental')
 table(tag+'_memoria','Memoria de red informada durante la generación para '+('radiografías' if tag=='rx' else 'MPC')+'.','tab:'+tag+'_memoria',['Variante / ruta','Pesos (B)','Activaciones (B)'],mem,r'primer resumen de \texttt{generate}; incluye búferes de entrada/salida en activaciones')
mpc=pd.read_csv(T/'mpc_simulacion_resumen.csv');names=['Float32 Keras','PTQ INT8 tensor','PTQ INT8 canal','QAT INT8','PTQ INT16x8','Mixta selectiva'];ex=[199,74,50,65,198,156]
table('mpc_sim','Resultados en lazo cerrado sobre 200 condiciones iniciales.','tab:mpcres',['Variante','Éxitos','Fallos','Éxito (\%)','Archivo (B)','Formato'],[[names[i],ex[i],200-ex[i],num(r['Success_Rate_%'],1),integer(r['Tamaño_bytes']),'.keras' if i==0 else '.tflite'] for i,(_,r) in enumerate(mpc.iterrows())],r'exportación HTML del estudio MPC de referencia; el conteo Keras procede de su evaluación explícita',cols='Yrrrrl')
table('hil_funcional','Verificación funcional de las ocho condiciones iniciales HIL.','tab:hilres',[r'$\alpha_0$ (rad)',r'$\max |y|$ (m)',r'$\max |u|$ (V)',r'$\max_{10}|y|$ (m)',r'$\max_{10}|e_\alpha|$ (rad)','Cumple'],[[num(r.alpha0,2),num(r.max_abs_y,4),num(r.max_abs_u,3),num(r.final10_max_y,4),num(r.final10_max_ealpha,4),'Sí' if r.success else 'No'] for _,r in H.iterrows()],r'CSV HIL; ventana final de 14,56 a 16,00 s, con estado terminal reconstruido',cols='Yrrrrl')
table('hil_tiempos','Resumen temporal por condición inicial HIL: 100 intercambios por ensayo.','tab:hil_tiempos',[r'$\alpha_0$ (rad)','Infer. media ($\mu$s)','Infer. máx. ($\mu$s)','RTT medio (ms)','RTT máx. (ms)'],[[num(r.alpha0,2),num(r.mean_infer_us,2),integer(r.max_infer_us),num(r.mean_rtt_ms,4),num(r.max_rtt_ms,4)] for _,r in H.iterrows()],'cálculo propio a partir de las 800 filas de los CSV HIL')
names=['RX INT8','RX INT16x8 importado','RX mixta-5','MPC INT8 tensor','MPC INT8 canal','MPC QAT','MPC mixta'];keys=['PTQ_INT8_NPU.txt','PTQ_INT16x8_NPU_log.txt','PTQ_MIX_INT8x32_SELECTIVE_NPU.txt','ptq_tensor_npu.txt','ptq_channel_npu.txt','qat_int8_npu.txt','ptq_mixed_selectived_npu.txt']
names.append('RX QAT');keys.append('Clasificador_Neumonia_run-8_logs_QAT_NPU.txt')
table('perfil','Distribución temporal informada en las ejecuciones Neural-ART.','tab:perfil',['Modelo','HW (\%)','SW (\%)','Control SW (\%)'],[[n,*[num(D[D.archivo==k].iloc[0][c],1) for c in ['hw_pct','sw_pct','ctrl_pct']]] for n,k in zip(names,keys)],'resumen del perfil ST; pequeñas diferencias respecto de 100\% se deben al redondeo')
print('Tablas listas')
