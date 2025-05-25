import subprocess
import re
import matplotlib.pyplot as plt
import os
import statistics

# Paramètres
image_file = "manet.jpg"
output_image = "result.png"
ocl_files = ["copyimageGa.cl", "copyimageGb.cl", "copyimageGc.cl", "copyimageGd.cl", "copyimageGe.cl"]
kernel_name = "copy_image"
sizes = list(range(0, 51))
repetitions = 3

# Dossier de sortie
output_dir = "execution_results"
os.makedirs(output_dir, exist_ok=True)

for ocl_file in ocl_files:
    base_name = os.path.splitext(os.path.basename(ocl_file))[0]
    print(f"\n========== OpenCL File: {ocl_file} ==========")
    avg_times = []

    for size in sizes:
        times = []
        print(f"  Taille {size} :")
        for i in range(repetitions):
            print(f"    → Essai {i+1}/{repetitions}")
            cmd = [
                "./build/imageCopyFilter",
                "--image", image_file,
                "--ocl", ocl_file,
                "--output", output_image,
                "--kernel", kernel_name,
                "--size", str(size)
            ]

            result = subprocess.run(cmd, capture_output=True, text=True)
            output = result.stdout + result.stderr

            match = re.search(r"Execution time:\s*([0-9.]+)", output)
            if match:
                t = float(match.group(1))
                print(f"      Temps : {t:.6f} s")
                times.append(t)
            else:
                print("      [Erreur] Temps non trouvé")

        if times:
            moyenne = min(times)
            avg_times.append(moyenne)
        else:
            avg_times.append(None)

    # Sauvegarde CSV
    valid_sizes = [s for s, t in zip(sizes, avg_times) if t is not None]
    valid_avg_times = [t for t in avg_times if t is not None]

    csv_filename = os.path.join(output_dir, f"execution_times_{base_name}.csv")
    with open(csv_filename, "w") as f:
        f.write("size,avg_time\n")
        for size, time in zip(valid_sizes, valid_avg_times):
            f.write(f"{size},{time}\n")
    print(f"[✓] Résultats sauvegardés dans {csv_filename}")
"""
    # Graphique
    plt.figure(figsize=(10, 6))
    plt.plot(valid_sizes, valid_avg_times, marker='o', linestyle='-')
    plt.xlabel("Filter size (N)")
    plt.ylabel("Average Execution time (s)")
    plt.title(f"Average Execution Time - {base_name}")
    plt.grid(True)
    plt.tight_layout()
    graph_path = os.path.join(output_dir, f"{base_name}_avg_graph.png")
    plt.savefig(graph_path)
    plt.close()
    print(f"[✓] Graphique sauvegardé dans {graph_path}")
"""
print("\nToutes les exécutions sont terminées avec le kernel unique `copy_image`.")
