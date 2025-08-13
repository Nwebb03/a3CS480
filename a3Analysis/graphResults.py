import numpy as np
import matplotlib.pyplot as plt

# Load data (skip header)
data = np.genfromtxt("results.csv", delimiter=",", skip_header=1)

# Extract columns
request_id = data[:, 0]
first_fit_frag = data[:, 1]
best_fit_frag = data[:, 2]
first_fit_nodes = data[:, 3]
best_fit_nodes = data[:, 4]

# -------- Graph 1: Fragments vs RequestID --------
plt.figure(figsize=(10, 5))
plt.plot(request_id, first_fit_frag, label="First Fit Fragments", color="blue")
plt.plot(request_id, best_fit_frag, label="Best Fit Fragments", color="green")
plt.xlabel("Request ID")
plt.ylabel("Fragments")
plt.title("First vs Best Fit Fragments")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("fragments_vs_request_id.png", dpi=300)
# -------- Graph 2: Nodes Traversed vs RequestID --------
plt.figure(figsize=(10, 5))
plt.plot(request_id, first_fit_nodes, label="First Fit Nodes Traversed", color="red")
plt.plot(request_id, best_fit_nodes, label="Best Fit Nodes Traversed", color="purple")
plt.xlabel("Request ID")
plt.ylabel("Nodes Traversed")
plt.title("First vs Best Fit Nodes Traversed")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("nodes_traversed.png", dpi=300)

# -------- Graph 3: Cumulative Sum (Integral) of Fragments vs RequestID --------
cumulative_sum_fragments_first = np.cumsum(first_fit_frag)  # Running total
cumulative_sum_fragments_best = np.cumsum(best_fit_frag)  # Running total
plt.figure(figsize=(10, 5))
plt.plot(request_id, cumulative_sum_fragments_first, label="Cumulative First Fit Fragments", color="orange")
plt.plot(request_id, cumulative_sum_fragments_best, label="Cumulative Best Fit Fragments", color="cyan")
plt.xlabel("Request ID")
plt.ylabel("Cumulative Fragments")
plt.title("Cumulative Sum of Fragments vs Request ID")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("cumulative_fragments.png", dpi=300)
