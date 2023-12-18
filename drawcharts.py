import pandas as pd
import matplotlib.pyplot as plt
import os.path

script_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(script_dir)

# Output directory for saving figures
output_dir = os.path.join(script_dir, "figures")
os.makedirs(output_dir, exist_ok=True)

# File paths
mpi_files = {
    "MPI (4) processes": "./Times/MPI_times(4).csv",
    "MPI (6) processes": "./Times/MPI_times(6).csv",
    "MPI (8) processes": "./Times/MPI_times(8).csv"
}

sas_files = {
    "Shared Address Space (4) threads": "./Times/SAS_times(4).csv",
    "Shared Address Space (50) threads": "./Times/SAS_times(50).csv",
    "Shared Address Space (100) threads": "./Times/SAS_times(100).csv",
    "Shared Address Space (500) threads": "./Times/SAS_times(500).csv"
}

sequential_file = "./Times/sequential_times.csv"
# Load data
mpi_data = {key: pd.read_csv(file_path) for key, file_path in mpi_files.items()}
sas_data = {key: pd.read_csv(file_path) for key, file_path in sas_files.items()}
sequential_data = pd.read_csv(sequential_file)

def save_line_chart(data_dict, title_prefix, output_dir):
    """Function to save line charts as figures for each configuration"""
    for key, data in data_dict.items():
        plt.figure(figsize=(12, 6))
        plt.plot(data["Encoder Times"], label="Encoder Times")
        plt.plot(data["Decoder Times"], label="Decoder Times")
        plt.plot(data["Key Generation Times"], label="Key Generation Times")
        plt.title(f"{title_prefix} Configuration: {key}")
        plt.xlabel("Number of characters")
        plt.ylabel("Time")
        plt.legend()
        plt.grid(True)
        fig_name = f"{title_prefix}_{key}.png"
        fig_path = os.path.join(output_dir, fig_name)
        plt.savefig(fig_path, bbox_inches='tight')
        plt.close()

# Plotting line charts for MPI, SAS, and Sequential data
print("MPI Data Line Charts")
save_line_chart(mpi_data, "MPI", output_dir)

print("SAS Data Line Charts")
save_line_chart(sas_data, "SAS", output_dir)

# Plotting for sequential data
plt.figure(figsize=(12, 6))
plt.plot(sequential_data["Encoder Times"], label="Encoder Times")
plt.plot(sequential_data["Decoder Times"], label="Decoder Times")
plt.plot(sequential_data["Key Generation Times"], label="Key Generation Times")
plt.title("Sequential Configuration")
plt.xlabel("Number of characters")
plt.ylabel("Time")
plt.legend()
plt.grid(True)
fig_name = "sequential.png"
fig_path = os.path.join(output_dir, fig_name)
plt.savefig(fig_path, bbox_inches='tight')
plt.close()
