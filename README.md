# IoT-Based Food Wastage Monitoring and Fine System in Hostel Mess

This project is an IoT-based solution to monitor and reduce food wastage in hostel mess environments. The system uses a load cell to measure leftover food and a servo motor to control dustbin access. If the waste exceeds a set threshold (20 grams), a fine is calculated and displayed through a mobile app. Data is stored in Firebase for real-time tracking and analysis.

## 💡 Features
- Automatic dustbin lid control using servo motor
- Load sensor (HX711) for food waste measurement
- Fine calculation for food waste above 20 grams
- Real-time data storage and access via Firebase
- Mobile app built with MIT App Inventor for user interaction

## 🔧 Technologies Used
- NodeMCU (ESP8266)
- HX711 Load Cell + 5kg strain gauge
- Servo Motor
- Firebase Realtime Database
- MIT App Inventor (for mobile app)
- Arduino IDE (for microcontroller programming)

## 📲 How It Works
1. Student logs in via the mobile app.
2. Upon successful login, the dustbin lid opens automatically.
3. The student disposes of leftover food.
4. The load sensor measures the waste.
5. If waste > 20g, a fine of ₹10 per extra gram is calculated.
6. Waste and fine data are uploaded to Firebase and shown in the app.
7. The lid closes automatically after disposal.
