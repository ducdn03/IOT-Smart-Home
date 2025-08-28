# 🏠 IoT Smart Home – Hệ Thống Giám Sát & Điều Khiển Thiết Bị

Dự án môn **"Các giải pháp và hệ thống IoT tiên tiến"**: Xây dựng hệ thống giám sát môi trường và điều khiển thiết bị điện trong nhà dựa trên nền tảng IoT.

---

## 🎯 Mục Tiêu

- Đo lường: **nhiệt độ**, **độ ẩm**, **ánh sáng** (DHT11, LDR)
- Điều khiển từ xa: **đèn**, **quạt**, **điều hòa** (relay)
- Kiểm soát truy cập: **thẻ RFID**
- Giao tiếp & xử lý dữ liệu: **MQTT**
- Giao diện web: hiển thị & điều khiển thiết bị
- Lưu trữ lịch sử: **CSDL MySQL**

---

## 🏗 Kiến Trúc Hệ Thống

- **Thiết bị nhúng (ESP8266):** Thu thập dữ liệu, nhận lệnh điều khiển
- **Broker MQTT (Mosquitto):** Trung gian truyền dữ liệu
- **Backend (NodeJS – ExpressJS):** Xử lý API, kết nối MQTT & MySQL
- **Frontend (HTML/CSS/JS):** Dashboard hiển thị & điều khiển
- **Cơ sở dữ liệu (MySQL):** Lưu thông số cảm biến & lịch sử thao tác

---
---

## ⚙️ Hướng Dẫn Triển Khai

### 1. Phần Cứng

- ESP8266 NodeMCU
- DHT11 (nhiệt độ, độ ẩm)
- LDR (ánh sáng)
- RFID MFRC522
- Servo SG90
- Relay (quạt/đèn)
- Đèn LED, Quạt 12V

### 2. Cài Đặt Mosquitto

```bash
sudo apt install mosquitto mosquitto-clients
# Chỉnh sửa mqtt/mosquitto.conf
# Tạo mqtt/passwd với user: manh, password: 123 (đã mã hóa)
```

### 3. Khởi Chạy Backend

```bash
cd backend
npm install
npm start
```

Tạo file `.env`:

```env
DB_HOST=localhost
DB_PORT=3306
DB_USER=root
DB_PASSWORD=yourpassword
DB_NAME=smart_home
```

### 4. Lập Trình ESP8266

- Sử dụng Arduino IDE, thêm thư viện: `ESP8266WiFi`, `PubSubClient`, `DHT`, `MFRC522`, `Servo`
- Nạp file `smart_home.ino` trong thư mục `embedded/`

---

## 🌐 Giao Diện Người Dùng

- **Dashboard:** Hiển thị nhiệt độ, độ ẩm, ánh sáng theo thời gian thực (biểu đồ)
- **Sensor Data:** Bảng dữ liệu cảm biến, hỗ trợ tìm kiếm, phân trang
- **Action History:** Lịch sử bật/tắt thiết bị
- **Điều khiển:** Trực tiếp từ web

---

## 🗄 Cơ Sở Dữ Liệu

- **Devices:** Lưu giá trị cảm biến
- **Actions:** Lưu lịch sử điều khiển

```sql
CREATE TABLE Devices (
    id INT AUTO_INCREMENT PRIMARY KEY,
    temperature FLOAT,
    humidity FLOAT,
    light INT,
    time DATETIME
);
```

---

## 🔒 Bảo Mật

- MQTT yêu cầu xác thực (user/password)
- Không cho phép kết nối ẩn danh
- Khuyến nghị nâng cấp TLS/SSL khi triển khai thực tế

---

## 👥 Nhóm Thực Hiện

- Nguyễn Tiến Thiệu – B21DCVT412
- Dương Ngọc Đức – B21DCVT132
- Lê Đức Mạnh – B21DCVT284
- Phạm Văn An – B21DCVT052

**GVHD:** Cô Nguyễn Thị Thu Hằng  
**Học viện Công nghệ Bưu chính Viễn thông**

---

## 📎 Liên Kết

- 📘 [Báo cáo đầy đủ: BTL_Nhóm 9.docx]
- 💻 [Repo GitHub](https://github.com/ducdn03/IOT-Smart-Home)

---

## 📄 License

This project is for educational purposes only.

---

