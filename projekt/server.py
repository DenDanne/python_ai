from flask import Flask, request, jsonify
import json
import os
import time

print("Begin ")

app = Flask(__name__)
first_received_time = None  # Track the first received data time
last_received_time = None  # Track the last received data time
current_timestamp = None  # Track the current timestamped filename


def generate_timestamp():
    return time.strftime("%Y%m%d_%H%M%S")


def count_lines(filename):
    if os.path.exists(filename):
        with open(filename, "r") as file:
            return sum(1 for _ in file)
    return 0


@app.route('/api/data', methods=['POST'])
def receive_data():
    global first_received_time, last_received_time, current_timestamp

    try:
        data = request.get_json()
        print("Received Data:", data)

        current_time = time.time()

        # Set first received time if it's None
        if first_received_time is None:
            first_received_time = current_time

        # If more than 5 seconds have passed since the last message, create new files
        if last_received_time is None or (current_time - last_received_time) > 10:
            current_timestamp = generate_timestamp()  # Generate new timestamp
            sensor_filename = f"sensor_data_{current_timestamp}.txt"
            reformat_filename = f"reformat_{current_timestamp}.txt"
        else:
            # Continue using the same filename
            sensor_filename = f"sensor_data_{current_timestamp}.txt"
            reformat_filename = f"reformat_{current_timestamp}.txt"

        last_received_time = current_time  # Update last received time

        # Save to new sensor_data file
        with open(sensor_filename, "a") as file:
            file.write(str(data) + "\n")

        # Process data for reformat file
        if isinstance(data, list) and len(data) == 900:
            num_samples = 150
            num_sensors = 6

            # Reshape data into columns
            reformatted_data = [[str(data[col * num_samples + row]) for col in range(num_sensors)] for row in range(num_samples)]

            # Append to reformat file
            with open(reformat_filename, "a") as file:
                for row in reformatted_data:
                    file.write(" ".join(row) + "\n")

        # Print the number of rows in both files
        sensor_data_lines = count_lines(sensor_filename)
        reformat_lines = count_lines(reformat_filename)
        elapsed_time = current_time - first_received_time if first_received_time else 0.0

        print(f"Rows in {sensor_filename}: {sensor_data_lines}")
        print(f"Rows in {reformat_filename}: {reformat_lines}")
        print(f"Time since first value received: {elapsed_time:.1f} seconds")

        return jsonify({"status": "success", "message": "Data received and reformatted"}), 200

    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 400


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)


#NN87UMFB3F
#3UK7KYDZP5
#ANM8CVMMYX
#HZ3FAP4P3F
#PR6338JT4Y
#RZC5WEZ287
#W6ADBTFRSJ
#4NGKJFHPXR
#QJ35YYWAPE
#T747CQ9WF2