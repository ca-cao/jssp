import matplotlib.pyplot as plt
import networkx as nx
import numpy as np
import sys
from networkx.drawing.nx_pylab import draw_networkx as draw_net

# importar datos 
# los datos contienen
# nodo vecino peso mejor_nodo
gr = np.loadtxt(sys.argv[1]).astype(int)

# definir grafo
G = nx.Graph()
# agregar nodos
G.add_nodes_from(gr[:,0])

# agregar aristas
G.add_weighted_edges_from([(x[0],x[1],x[2]) for x in gr])
plt.subplot()
draw_net(G,with_labels=True)
plt.show()
