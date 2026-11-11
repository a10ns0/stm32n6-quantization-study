from pathlib import Path
import re,json,hashlib,base64,ast,math,shutil,csv
import numpy as np
import pandas as pd
from lxml import html
from scipy.integrate import odeint
from dataclasses import dataclass
O=Path(__file__).resolve().parents[1]
R=O/'evidencia/resultados/originales'
T=O/'tablas/resultados';T.mkdir(parents=True,exist_ok=True)
F=O/'figuras/resultados';F.mkdir(parents=True,exist_ok=True)
E=O/'evidencia/resultados';E.mkdir(parents=True,exist_ok=True)
# Las fuentes originales se conservan sin alteraciones.
inventory=[]
for f in R.rglob('*'):
 if f.is_file():inventory.append({'archivo':str(f.relative_to(R)),'bytes':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
pd.DataFrame(inventory).to_csv(T/'inventario_fuentes.csv',index=False)
rows=[]
for f in R.glob('*/Validacion*/*.txt'):
 s=f.read_text();m=re.search(r'duration\s+:\s+([\d.]+) ms by sample \(([\d.]+)/([\d.]+)/([\d.]+)\)',s)
 if not m:continue
 row={'caso':f.parts[-3],'archivo':f.name,'media_ms':float(m[1]),'min_ms':float(m[2]),'max_ms':float(m[3]),'std_ms':float(m[4])}
 row['n']=int(re.search(r'nb sample\(s\)\s+:\s+(\d+)',s)[1])
 row['hash_modelo']=re.search(r'model_hash\s+:\s+(\S+)',s)[1]
 for name,pat in [('pesos_B',r'weights \(ro\)\s+:\s+([\d,]+) B'),('activaciones_B',r'activations \(rw\)\s+:\s+([\d,]+) B')]:
  vals=re.findall(pat,s);row[name+'_generate']=int(vals[0].replace(',',''));row[name+'_validate']=int(vals[-1].replace(',',''))
 cross=re.findall(r'X-cross #1\s+n.a.\s+([\d.]+)\s+([\d.]+)\s+([\d.]+)',s)[-1]
 row.update(dict(zip(['rmse_cross','mae_cross','l2r_cross'],map(float,cross))))
 hw=re.search(r'HW: ([\d.]+)%, SW: ([\d.]+)%, SW ctrl: ([\d.]+)%',s)
 for i,k in enumerate(['hw_pct','sw_pct','ctrl_pct']):row[k]=float(hw[i+1]) if hw else np.nan
 ac=re.findall(r'TARGET c-model #1\s+([\d.]+)%',s);row['acc_target']=float(ac[-1]) if ac else np.nan
 rows.append(row)
logs=pd.DataFrame(rows);logs.to_csv(T/'validacion_st.csv',index=False)
# Extract original HTML output figures and tables with output index provenance.
for f in R.glob('*/Simulacion/*.html'):
 d=html.parse(str(f));outputs=d.xpath('//*[contains(concat(" ",normalize-space(@class)," ")," jp-OutputArea-output ")]')
 short={'Pipeline_Radiografias':'rx_pipeline','comparacion_modelos_resumen_radiografia':'rx_comparacion','Notebook_MPC_CartPole_Baseline_Original_Autores':'mpc_baseline','Notebook_MPC_CartPole_NN_Float32':'mpc_exploratorio','Notebook_MPC_CartPole_Analisis_Dataset':'mpc_dataset'}[f.stem]
 for i,e in enumerate(outputs):
  for j,img in enumerate(e.xpath('.//img')):
   src=img.get('src','')
   if src.startswith('data:image/png;base64,'):(F/f'{short}_out{i}_{j}.png').write_bytes(base64.b64decode(src.split(',',1)[1]))
  for j,table in enumerate(e.xpath('.//table')):
   from io import StringIO
   df=pd.read_html(StringIO(html.tostring(table).decode()))[0];df.to_csv(T/f'{short}_out{i}_tabla{j}.csv',index=False)
# Radiography paired predictions, no model inference rerun.
P=R/'CasoRadiografia/Simulacion/resultados_comparacion_qat'
rx=pd.read_csv(P/'comparacion_final_modelos.csv');rx.to_csv(T/'radiografia_comparacion.csv',index=False)
ref=np.load(P/'predicciones_Float32.npz');paired=[]
for _,r in rx.iterrows():
 n=r.Modelo.replace(' ','_').replace('-','_');p=np.load(P/f'predicciones_{n}.npz')
 assert np.array_equal(ref['y_real'],p['y_real'])
 y=p['y_real'];pred=p['y_pred'];good=(pred==y);base=(ref['y_pred']==y)
 assert abs(good.mean()-r.Accuracy)<1e-12
 assert abs(np.sqrt(np.mean((p['salidas'].astype(float)-ref['salidas'])**2))-r.RMSE_salida_vs_Float32)<1e-7
 paired.append({'Modelo':r.Modelo,'ambos_correctos':int((good&base).sum()),'solo_variante_correcto':int((good&~base).sum()),'solo_base_correcto':int((~good&base).sum()),'ambos_incorrectos':int((~good&~base).sum()),'predicciones_distintas':int((pred!=ref['y_pred']).sum())})
pd.DataFrame(paired).to_csv(T/'radiografia_predicciones_pareadas.csv',index=False)
# HIL: load ONLY pure plant functions from supplied script, not UART main.
src=(R/'CasoMPC/HIL/hil_cartpole_uart.py').read_text();tree=ast.parse(src)
names={'PlantParameters','plant_derivative','integrate_plant'}
code=ast.Module(body=[n for n in tree.body if isinstance(n,(ast.FunctionDef,ast.ClassDef)) and n.name in names],type_ignores=[])
ns={'np':np,'math':math,'dataclass':dataclass,'odeint':odeint};exec(compile(code,'plant_functions','exec'),ns)
hil=[];reconstructed=[];series={}
for f in (R/'CasoMPC/HIL').glob('*.csv'):
 a=float(re.search(r'--alpha0 ([\d.-]+)',f.name)[1]);d=pd.read_csv(f)
 assert len(d)==100 and d.id.tolist()==list(range(100))
 assert np.allclose(d.t_s,np.arange(100)*.16)
 statecols=['y_m','alpha_cont_rad','y_dot_m_s','alpha_dot_rad_s'];X=d[statecols].to_numpy();u=d.u_v.to_numpy()
 pred=np.array([ns['integrate_plant'](X[i],u[i],.16,ns['PlantParameters']()) for i in range(100)])
 err=np.max(np.abs(pred[:-1]-X[1:]));assert err<1e-7,(a,err)
 xx=np.vstack([X,pred[-1]]);ea=np.arctan2(np.sin(xx[:,1]),np.cos(xx[:,1]));last=xx[-10:]
 ylast=np.abs(last[:,0]).max();alast=np.abs(ea[-10:]).max();success=bool(np.abs(xx[:,0]).max()<.4 and np.abs(u).max()<=9 and ylast<.1 and alast<.17)
 row={'alpha0':a,'n':len(d),'max_abs_y':np.abs(xx[:,0]).max(),'max_abs_u':np.abs(u).max(),'final10_max_y':ylast,'final10_max_ealpha':alast,'success':success,'mean_infer_us':d.inference_us.mean(),'max_infer_us':d.inference_us.max(),'p99_infer_us':d.inference_us.quantile(.99),'mean_rtt_ms':d.rtt_us.mean()/1000,'max_rtt_ms':d.rtt_us.max()/1000,'p99_rtt_ms':d.rtt_us.quantile(.99)/1000,'missed':int(d.deadline_missed.sum()),'max_step_replay_error':err,'saturated_steps':int((np.abs(u)>=9-1e-6).sum()),'y16':xx[-1,0],'alpha16':xx[-1,1]}
 text=next((R/'CasoMPC/HIL').glob(f'python*--alpha0 {a:.2f} *.txt')).read_text()
 assert ('ÉXITO: True' in text)==success
 row['late_pc_reported']=100 if 'Ciclos tardíos del PC (incluye gráfico): 100' in text else np.nan
 hil.append(row);reconstructed.append({'alpha0':a,'t_s':16,**dict(zip(statecols,xx[-1]))})
 series[a]={'df':d,'X':xx,'e':ea,'u':u,'success':success}
pd.DataFrame(hil).sort_values('alpha0').to_csv(T/'hil_resumen.csv',index=False)
pd.DataFrame(reconstructed).to_csv(T/'hil_estado16_reconstruido.csv',index=False)
# UART chest demonstration: ground truth from the file paths in the supplied log.
uart=next(R.glob('CasoRadiografia/Validacion*/Transmision*.txt')).read_text();ur=[]
for block in re.split(r'(?=\[\d+/16\])',uart):
 m=re.search(r'RESULT,id=(\d+),class=(\d+),label=(\w+),normal_milli=(\d+),pneumonia_milli=(\d+),latency_us=(\d+)',block)
 if m:ur.append(dict(id=int(m[1]),real=0 if '\\NORMAL\\' in block else 1,pred=int(m[2]),normal_milli=int(m[4]),pneumonia_milli=int(m[5]),latency_us=int(m[6])))
pd.DataFrame(ur).to_csv(T/'radiografia_uart16.csv',index=False)
print(logs[['archivo','media_ms','pesos_B_generate','activaciones_B_generate','rmse_cross']].to_string(index=False))
print(pd.DataFrame(hil).sort_values('alpha0').to_string(index=False));print(pd.DataFrame(paired).to_string(index=False))
print('UART',pd.DataFrame(ur).latency_us.describe().to_dict())
