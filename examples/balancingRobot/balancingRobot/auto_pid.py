import serial
import time
import threading
import tkinter as tk
from tkinter import ttk

class BalancingRobotApp:
    def __init__(self, root, ser):
        self.root = root
        self.ser = ser
        self.running = True
        self.bridge_state = False  # False = OFF, True = ON
        self.testing = False  # Flag for automatic testing
        self.best_combinations = []  # Store the best 3 combinations

        # GUI Layout
        self.root.title("Balancing Robot Control")
        self.root.geometry("400x500")

        # Angle Display
        self.angle_label = ttk.Label(root, text="Angle: 0.0", font=("Arial", 16))
        self.angle_label.pack(pady=20)

        # Current PID Values Display
        self.pid_label = ttk.Label(root, text="kp: 0.0, ki: 0.0, kd: 0.0, alpha: 0.0", font=("Arial", 12))
        self.pid_label.pack(pady=10)

        # Best Combinations Display
        self.best_label = ttk.Label(root, text="Best Combinations:\n1. -\n2. -\n3. -", font=("Arial", 12))
        self.best_label.pack(pady=20)

        # Button to toggle Multi-Halfbridge
        self.bridge_button = tk.Button(root, text="Turn Multi-Halfbridge ON", bg="red", fg="white", command=self.toggle_bridge)
        self.bridge_button.pack(pady=10)

        # Button to start automatic testing
        self.test_button = tk.Button(root, text="Start Automatic Testing", bg="blue", fg="white", command=self.start_testing)
        self.test_button.pack(pady=10)

        # Exit Button
        self.exit_button = ttk.Button(root, text="Exit", command=self.exit_app)
        self.exit_button.pack(pady=10)

        # Start a thread to read angles
        self.read_thread = threading.Thread(target=self.read_angles, daemon=True)
        self.read_thread.start()

    def read_angles(self):
        """Read and update angle values continuously from the Arduino."""
        while self.running:
            if self.ser.in_waiting > 0:  # Check if data is available from Arduino
                response = self.ser.readline().decode('utf-8', errors='ignore').strip()
                try:
                    # Remove the "Angle: " prefix if it exists
                    if response.startswith("Angle:"):
                        response = response.replace("Angle:", "").strip()
                    
                    # Convert the cleaned response to a float
                    angle = float(response)
                    self.update_angle_label(angle)
                except ValueError:
                    print(f"Invalid response: {response}")
            time.sleep(0.1)  # Small delay to avoid overwhelming the serial port

    def update_angle_label(self, angle):
        """Update the angle label in the GUI."""
        self.angle_label.config(text=f"Angle: {angle}")

    def update_pid_label(self, kp, ki, kd, alpha):
        """Update the PID values label in the GUI."""
        self.pid_label.config(text=f"kp: {kp}, ki: {ki}, kd: {kd}, alpha: {alpha}")

    def update_best_combinations(self, kp, ki, kd, alpha, error):
        """Update the best combinations in the GUI."""
        # Add the new combination with its error
        self.best_combinations.append((kp, ki, kd, alpha, error))
        # Sort by error (ascending) and keep only the top 3
        self.best_combinations = sorted(self.best_combinations, key=lambda x: x[4])[:3]
        # Update the label
        best_text = "Best Combinations:\n"
        for i, (kp, ki, kd, alpha, error) in enumerate(self.best_combinations, start=1):
            best_text += f"{i}. kp: {kp}, ki: {ki}, kd: {kd}, alpha: {alpha}, error: {error:.2f}\n"
        self.best_label.config(text=best_text)

    def toggle_bridge(self):
        """Toggle the Multi-Halfbridge ON/OFF."""
        if self.bridge_state:
            self.send_command("bridge off")
            self.bridge_state = False
            self.bridge_button.config(text="Turn Multi-Halfbridge ON", bg="red", fg="white")
        else:
            self.send_command("bridge on")
            self.bridge_state = True
            self.bridge_button.config(text="Turn Multi-Halfbridge OFF", bg="green", fg="white")

    def start_testing(self):
        """Start the automatic testing process."""
        if not self.testing:
            self.testing = True
            self.test_button.config(text="Stop Automatic Testing", bg="orange")
            self.test_thread = threading.Thread(target=self.run_testing, daemon=True)
            self.test_thread.start()
        else:
            self.testing = False
            self.test_button.config(text="Start Automatic Testing", bg="blue")

    def run_testing(self):
        """Run the automatic testing process."""
        kp, ki, kd, alpha = 1.0, 0.5, 0.1, 0.9  # Example initial values
        while self.testing:
            # Simulate an error value (replace this with actual feedback from Arduino)
            error = abs(alpha - 0.7) + abs(kp - 1.2) + abs(ki - 0.6) + abs(kd - 0.2)

            # Reset PID and send the current PID values to the Arduino
            self.reset_pid()
            self.send_command(f"kp {kp}")
            self.send_command(f"ki {ki}")
            self.send_command(f"kd {kd}")
            self.send_command(f"alpha {alpha}")

            # Update the GUI with the current PID values
            self.update_pid_label(kp, ki, kd, alpha)

            # Update the best combinations
            self.update_best_combinations(kp, ki, kd, alpha, error)

            # Simulate testing logic (e.g., adjust values based on feedback)
            kp += 0.1
            ki += 0.05
            kd += 0.01
            alpha -= 0.01

            # Stop condition for testing (example: alpha reaches 0.5)
            if alpha <= 0.5:
                self.testing = False
                self.test_button.config(text="Start Automatic Testing", bg="blue")
                break

            time.sleep(1)  # Wait before the next iteration

    def send_command(self, command):
        """Send a command to the Arduino."""
        self.ser.write(f"{command}\n".encode('utf-8'))
        time.sleep(0.1)  # Allow Arduino to process the command
        if self.ser.in_waiting > 0:
            response = self.ser.readline().decode('utf-8').strip()
            print(f"Arduino responded: {response}")

    def reset_pid(self):
        """Send a command to reset the PID state on the Arduino."""
        self.send_command("reset_pid")

    def exit_app(self):
        """Exit the application."""
        self.running = False
        self.testing = False
        self.root.destroy()

def main():
    # Configure the serial connection
    arduino_port = "COM63"  # Replace with your Arduino's COM port
    baud_rate = 9600        # Match the baud rate with the Arduino sketch
    timeout = 1             # Timeout in seconds

    try:
        # Open the serial connection
        with serial.Serial(arduino_port, baud_rate, timeout=timeout) as ser:
            print(f"Connected to Arduino on {arduino_port} at {baud_rate} baud.")
            
            # Initialize the GUI
            root = tk.Tk()
            app = BalancingRobotApp(root, ser)
            root.mainloop()

    except serial.SerialException as e:
        print(f"Error: {e}")
    except KeyboardInterrupt:
        print("\nProgram interrupted by user.")

if __name__ == "__main__":
    main()