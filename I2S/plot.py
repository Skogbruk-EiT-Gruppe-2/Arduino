# Read the values from the "recorded-data.txt" file (comma-separated strings of integers) and plot them

import matplotlib.pyplot as plt
import numpy as np
import os

# Configuration parameters
SAMPLE_RATE = 16000  # Default sample rate - adjust based on your Arduino configuration
FILE_NAME = "data1.txt"

def plot_pcm_data(file_path=FILE_NAME, sample_rate=SAMPLE_RATE, outlier_threshold=1e18):
    try:
        # Check if file exists
        if not os.path.exists(file_path):
            print(f"Error: File '{file_path}' not found.")
            return
            
        # Read data from the file
        with open(file_path, 'r') as file:
            data_string = file.read().strip()
        
        # Parse comma-separated values
        data = np.array([int(val) for val in data_string.split(',') if val.strip()])
        
        # Filter out outliers
        original_count = len(data)
        mean_val = np.mean(np.abs(data))
        std_val = np.std(np.abs(data))
        threshold = mean_val + outlier_threshold * std_val
        
        # Create mask for values within threshold
        mask = np.abs(data) <= threshold
        filtered_data = data[mask]
        outliers_removed = original_count - len(filtered_data)
        
        # Calculate time axis (using filtered data)
        duration = len(filtered_data) / sample_rate
        time = np.linspace(0, duration, len(filtered_data))
        
        # Create the plot
        plt.figure(figsize=(12, 6))
        
        # Plot waveform
        plt.subplot(2, 1, 1)
        plt.plot(time, filtered_data, 'b-', linewidth=0.8)
        plt.title(f'PCM Audio Waveform (Outliers Removed)')
        plt.xlabel('Time (s)')
        plt.ylabel('Amplitude')
        plt.grid(True)
        
        # Add stats
        max_val = np.max(filtered_data)
        min_val = np.min(filtered_data)
        plt.figtext(0.02, 0.02, 
                  f'Samples: {len(filtered_data)}, Duration: {duration:.2f}s\n'
                  f'Min: {min_val}, Max: {max_val}\n'
                  f'Outliers removed: {outliers_removed} ({(outliers_removed/original_count)*100:.1f}%)', 
                  fontsize=9)
        
        # Plot frequency spectrum
        plt.subplot(2, 1, 2)
        spectrum = np.abs(np.fft.rfft(filtered_data))
        freq = np.fft.rfftfreq(len(filtered_data), 1/sample_rate)
        plt.plot(freq, spectrum, 'r-', linewidth=0.8)
        plt.title('Frequency Spectrum')
        plt.xlabel('Frequency (Hz)')
        plt.ylabel('Magnitude')
        plt.grid(True)
        
        plt.tight_layout()
        plt.show()
        
        print(f"Plotted {len(filtered_data)} PCM samples (removed {outliers_removed} outliers)")
        print(f"Audio duration: {duration:.2f} seconds")

    except Exception as e:
        print(f"Error occurred: {e}")

if __name__ == "__main__":
    import sys
    
    # Allow command-line specification of file, sample rate, and outlier threshold
    if len(sys.argv) > 1:
        file_path = sys.argv[1]
        rate = int(sys.argv[2]) if len(sys.argv) > 2 else SAMPLE_RATE
        threshold = float(sys.argv[3]) if len(sys.argv) > 3 else 3.0
        plot_pcm_data(file_path, rate, threshold)
    else:
        plot_pcm_data()
