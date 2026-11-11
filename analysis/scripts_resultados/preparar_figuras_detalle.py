from pathlib import Path
import fitz,numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
O=Path(__file__).resolve().parents[1];F=O/'figuras/resultados'
for name in ['rx_mixed_opt','mpc_mixed_opt']:
 src=fitz.open(F/(name+'.pdf'));s=src[0];w,h=s.rect.width,s.rect.height
 pix=s.get_pixmap(matrix=fitz.Matrix(2,2),alpha=False);a=np.frombuffer(pix.samples,dtype=np.uint8).reshape(pix.height,pix.width,3);count=(a.min(axis=2)<180).sum(axis=1);cuts=[0]
 for j in range(1,3):
  center=j*h/3;lo=int((center-45)*2);hi=int((center+45)*2);scores=np.convolve(count,np.ones(3)/3,mode='same')[lo:hi]+np.abs(np.arange(lo,hi)-center*2)*.004;cuts.append((lo+int(np.argmin(scores)))/2)
 cuts.append(h)
 for j in range(3):
  out=fitz.open();p=out.new_page(width=w,height=cuts[j+1]-cuts[j]);p.show_pdf_page(p.rect,src,0,clip=fitz.Rect(0,cuts[j],w,cuts[j+1]));out.save(F/(name+f'_tramo{j+1}.pdf'))
for group,seq in [(1,[(84,85,88),(91,92,95),(97,98,101)]),(2,[(103,104,107),(109,110,113),(115,116,119)])]:
 for typ,col in [('pos',0),('angulo',1),('u',2)]:
  fig,axs=plt.subplots(3,1,figsize=(8,9),layout='constrained')
  for ax,pair in zip(axs,seq):ax.imshow(plt.imread(F/f'mpc_baseline_out{pair[col]}_0.png'));ax.axis('off')
  fig.savefig(F/f'mpc_tray_{group}_{typ}.pdf',bbox_inches='tight');plt.close(fig)
