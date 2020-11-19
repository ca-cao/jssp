# random graph
import plotly.graph_objects as go
import networkx as nx
import matplotlib.pyplot as plt

G = nx.Graph()
G.add_nodes_from([i for i in range(10)])

G.add_weighted_edges_from([(i,(j+1)%10,.1) for i ,j in zip(range(10),range(10))])
G.add_weighted_edges_from([(i,(j+3)%10,100) for i ,j in zip(range(10),range(10))])

plt.subplot()
nx.draw(G,with_labels=True)
plt.show()
