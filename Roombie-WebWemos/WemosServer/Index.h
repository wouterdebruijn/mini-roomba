const char ROOT_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Roombie Control</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            margin: 0;
            padding: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .controls {
            margin: 20px 0;
        }

        button {
            padding: 10px 20px;
            font-size: 16px;
            margin: 10px;
            cursor: pointer;
            background-color: #1e1e1e;
            color: #ffffff;
            border: 1px solid #333;
            border-radius: 5px;
        }

        button:hover {
            background-color: #333;
        }

        .sensor-circle {
            position: relative;
            width: 200px;
            height: 200px;
            margin: 20px auto;
            border-radius: 50%;
            border: 2px solid #555;
            transform: rotate(-45deg);
        }

        .sensor-point {
            position: absolute;
            width: 20px;
            height: 20px;
            border-radius: 50%;
        }

        .sensor-point:nth-child(1) {
            top: 0;
            left: 50%;
            transform: translate(-50%, -50%);
        }

        .sensor-point:nth-child(2) {
            top: 50%;
            right: 0;
            transform: translate(50%, -50%);
        }

        .sensor-point:nth-child(3) {
            bottom: 0;
            left: 50%;
            transform: translate(-50%, 50%);
        }

        .sensor-point:nth-child(4) {
            top: 50%;
            left: 0;
            transform: translate(-50%, -50%);
        }
    </style>
</head>

<body>
    <h1>Roombie Control</h1>
    <div class="controls">
        <button onclick="sendCommand('c:2')">Toggle auto</button>
        <button onclick="sendCommand('c:3')">Toggle brushes</button>
    </div>
    <div class="sensor-circle">
        <div class="sensor-point" id="sensor1"></div>
        <div class="sensor-point" id="sensor2"></div>
        <div class="sensor-point" id="sensor3"></div>
        <div class="sensor-point" id="sensor4"></div>
    </div>

    <script>
        function sendCommand(command) {
            fetch('/api/command', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/text'
                },
                body: command
            }).then(response => {
                if (!response.ok) {
                    console.error('Failed to send command');
                }
            }).catch(error => console.error('Error:', error));
        }

        function updateSensors(sensorValues) {
            const maxSensorValue = 1024;
            sensorValues.forEach((value, index) => {
                const sensor = document.getElementById(`sensor${index + 1}`);
                const intensity = Math.floor((value / maxSensorValue) * 255);
                sensor.style.backgroundColor = `rgb(${intensity}, 0, ${255 - intensity})`;
            });
        }

        // Example: Simulate sensor updates
        setInterval(() => {
            const sensors = fetch('/api/sensors')
                .then(response => response.json())
                .then(data => {
                    if (data) {
                        console.log(data.map(sensor => sensor.value));
                        updateSensors(data.map(sensor => sensor.value));
                    }
                })
                .catch(error => console.error('Error fetching sensor data:', error));
        }, 1000);
    </script>
</body>

</html>
)=====";