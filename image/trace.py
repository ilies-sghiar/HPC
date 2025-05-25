import pandas as pd
import matplotlib.pyplot as plt

# Liste des fichiers CSV
files = [f'execution_results/execution_times_copyimageG{letter}.csv' for letter in ['a', 'b', 'c', 'd', 'e']]
labels = ['A', 'B', 'C', 'D', 'E']

# Charger toutes les données
data_list = [pd.read_csv(f) for f in files]

# Tracer le graphe avec les 5 courbes
plt.figure()
for df, label in zip(data_list, labels):
    plt.plot(df['size'], df['avg_time'], label=f'Courbe {label}')
plt.xlabel('Taille (size)')
plt.ylabel('Temps d\'exécution (time)')
plt.ylim(0, 0.175)
plt.title('Temps d\'exécution en fonction de la taille du filtre')
plt.legend()
plt.grid(True, which="both", ls="--", lw=0.5)
plt.savefig('graph_all.png')
plt.show()
plt.close()