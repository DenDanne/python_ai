from flask import Flask, request, jsonify
import json
import os
import time

print("Begin ")

app = Flask(__name__)
first_received_time = None  # Track the first received data time

def count_lines(filename):
    if os.path.exists(filename):
        with open(filename, "r") as file:
            return sum(1 for _ in file)
    return 0

@app.route('/api/data', methods=['POST'])
def receive_data():
    global first_received_time
    try:
        data = request.get_json()
        print("Received Data:", data)

        # Set first received time if it's None
        if first_received_time is None:
            first_received_time = time.time()

        # Save to sensor_data.txt
        with open("sensor_data.txt", "a") as file:
            file.write(str(data) + "\n")

        # Process data for reformat.txt
        if isinstance(data, list) and len(data) == 600:
            num_samples = 100
            num_sensors = 6
            
            # Reshape data into columns
            reformatted_data = [[str(data[col * num_samples + row]) for col in range(num_sensors)] for row in range(num_samples)]
            
            # Append to reformat.txt instead of overwriting
            with open("reformat.txt", "a") as file:
                for row in reformatted_data:
                    file.write(" ".join(row) + "\n")

        # Print the number of rows in both files
        sensor_data_lines = count_lines("sensor_data.txt")
        reformat_lines = count_lines("reformat.txt")
        elapsed_time = time.time() - first_received_time if first_received_time else 0.0
        
        print(f"Rows in sensor_data.txt: {sensor_data_lines}")
        print(f"Rows in reformat.txt: {reformat_lines}")
        print(f"Time since first value received: {elapsed_time:.1f} seconds")

        return jsonify({"status": "success", "message": "Data received and reformatted"}), 200
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)