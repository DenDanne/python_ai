from flask import Flask, request, render_template_string, jsonify
import time
import threading
import tkinter as tk
from tkinter import ttk

app = Flask(__name__)

latest_prediction = {"time": "Waiting for data...", "text": "No prediction yet"}

def update_gui():
    """Updates the GUI with the latest prediction."""
    if gui_label_time and gui_label_style:
        gui_label_time.config(text=f"Received at: {latest_prediction['time']}")
        gui_label_style.config(text=latest_prediction['text'])

def start_gui():
    """Initializes the GUI."""
    global gui_label_time, gui_label_style, root

    root = tk.Tk()
    root.title("Predicted Ski Style")
    root.geometry("400x200")
    root.configure(bg="#1e1e1e")

    style = ttk.Style()
    style.configure("TLabel", foreground="white", background="#1e1e1e", font=("Arial", 12))
    style.configure("TFrame", background="#1e1e1e")

    frame = ttk.Frame(root)
    frame.pack(expand=True, fill="both")
    
    ttk.Label(frame, text="Predicted Ski Style:", font=("Arial", 14, "bold"), foreground="#00ffcc").pack(pady=10)
    
    gui_label_time = ttk.Label(frame, text="Waiting for data...", font=("Arial", 12))
    gui_label_time.pack(pady=5)
    
    gui_label_style = ttk.Label(frame, text="No prediction yet", font=("Arial", 16, "bold"), foreground="#00ffcc")
    gui_label_style.pack(pady=10)

    root.mainloop()

html_template = """
<!DOCTYPE html>
<html>
<head>
    <title>Predicted Skiing Style</title>
    <meta http-equiv="refresh" content="3">
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; background-color: #1e1e1e; color: white; }
        h1 { color: #00ffcc; }
        .container { display: flex; flex-direction: column; align-items: center; justify-content: center; height: 50vh; }
        .prediction-box { background-color: #333; padding: 20px; border-radius: 10px; box-shadow: 0px 0px 10px rgba(0, 255, 204, 0.5); width: 60%; }
        p { font-size: 20px; margin: 10px 0; }
        .time { color: #bbbbbb; font-size: 18px; }
    </style>
</head>
<body>
    <h1>Predicted Skiing Style</h1>
    <div class="container">
        <div class="prediction-box">
            <p class="time">Received at: {{ latest_prediction['time'] }}</p>
            <p><strong>{{ latest_prediction['text'] }}</strong></p>
        </div>
    </div>
</body>
</html>
"""

@app.route('/')
def home():
    return render_template_string(html_template, latest_prediction=latest_prediction)

@app.route('/api/data', methods=['POST'])
def receive_data():
    try:
        data = request.get_json()

        if not data or "prediction" not in data:
            return jsonify({"status": "error", "message": "Invalid data format. Expecting JSON with 'prediction' key."}), 400

        prediction_text = data["prediction"]
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")

        print(f"[{timestamp}] Predicted Skiing Style: {prediction_text}")

        latest_prediction["time"] = timestamp
        latest_prediction["text"] = prediction_text

        if 'root' in globals():
            root.after(0, update_gui)

        return jsonify({"status": "success", "message": "Prediction received"}), 200
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 400

if __name__ == '__main__':
    threading.Thread(target=start_gui, daemon=True).start()
    app.run(host='0.0.0.0', port=5000, debug=True)