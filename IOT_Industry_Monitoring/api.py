
from flask import Flask, request, jsonify, render_template
import mysql.connector

app = Flask(__name__)

def get_db_connection():
    return mysql.connector.connect(
        host="localhost",
        user="root",
        password="",
        database="iot_monitoring"
    )

@app.route("/")
def home():
    return render_template("index.html")

app.route("/api/sensors", methods=["POST"])
def receive_sensor_data():

    try:
        data = request.get_json()

        if not data:
            return jsonify({"error": "No JSON data received"}), 400

        temperature = data["temperature"]
        humidity = data["humidity"]
        vibration = data["vibration"]
        gas = data["gas"]
        pressure = data["pressure"]
        rotation = data["rotation"]

        db = get_db_connection()
        cursor = db.cursor()

        sql = """
        INSERT INTO sensor_data
        (temperature, humidity, vibration, gas, pressure, rotation)
        VALUES (%s, %s, %s, %s, %s, %s)
        """

        values = (
            temperature,
            humidity,
            vibration,
            gas,
            pressure,
            rotation
        )

        cursor.execute(sql, values)
        db.commit()

        cursor.close()
        db.close()

        return jsonify({
            "status": "success",
            "message": "Sensor data stored successfully"
        }), 201

    except Exception as e:
        print("Database/API Error:", e)
        return jsonify({"error": str(e)}), 500

@app.route("/api/sensors", methods=["GET"])
def get_sensor_data():

    try:
        db = get_db_connection()
        cursor = db.cursor(dictionary=True)

        cursor.execute("""
            SELECT * FROM sensor_data
            ORDER BY id DESC
            LIMIT 20
        """)

        records = cursor.fetchall()

        cursor.close()
        db.close()

        return jsonify(records)

    except Exception as e:
        return jsonify({"error": str(e)}), 500


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=False)
