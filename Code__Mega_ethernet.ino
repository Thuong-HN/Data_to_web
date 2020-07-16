
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED  };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "mydatn.000webhostapp.com";    // name address for Google (using DNS)
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192,168, 1, 101); // IP của shield khi được cắm vào mạng
EthernetClient client;
//Motor
#define IN3  11
#define IN4 12
#define ENB 9 
#define Interrupt 2 
#define ChalB 5 
#define trig 7
#define echo 6
#define sensorCHE18 8
#define Light 10
const char* host = "mydatn.000webhostapp.com";
const char* uri = "/triet-encoder.php";
const int port = 80;
unsigned long timeSRF05; // biến đo thời gian
float distanceSRF05; 
String WarnLight;
//Motor
int But_Font,But_Back;
int Count = 0;
float Perimeter = 0.0;
float Diameter = 0.0;
float Rotation = 0.0;
float DistanceEncoder = 0.0;
char back;
String readback;   

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    Serial.println("..."); 
  }
  
  pinMode(trig,OUTPUT);   // chân trig sẽ phát tín hiệu
  pinMode(echo,INPUT);    // chân echo sẽ nhận tín hiệu
  pinMode(sensorCHE18,INPUT_PULLUP); // chân out của CHE18
  pinMode(ENB, OUTPUT); // chân ENB cầu H
  pinMode(IN3, OUTPUT); // Chân IN3 cầu H
  pinMode(IN4, OUTPUT); // Chân IN4 cầu H
  pinMode(Interrupt, INPUT_PULLUP); // Chân ngắt Mega2560 - Kênh A
  pinMode(ChalB, INPUT_PULLUP); // Chân kênh B
  attachInterrupt(0, pulse, FALLING); //Ngắt cạnh xuống
  Ethernet.begin(mac,ip);
  delay(200);
  // if you get a connection, report back via serial:
  Serial.println("IP của Ethernet shield:");
  Serial.println(Ethernet.localIP());
  
  //Serial.println("connecting...");
  
}
void loop() {
  // SRF05
  /* Phát xung từ chân trig */
    digitalWrite(trig,0);   // tắt chân trig
    delayMicroseconds(2);
    digitalWrite(trig,1);   // phát xung từ chân trig
    delayMicroseconds(5);   // xung có độ dài 5 microSeconds
    digitalWrite(trig,0);   // tắt chân trig
    /* Tính toán thời gian */
    // Đo độ rộng xung HIGH ở chân echo. 
    timeSRF05 = pulseIn(echo,HIGH);  
    // Tính khoảng cách đến vật.
    distanceSRF05 = int(timeSRF05/2/29.412);
    /* In kết quả ra Serial Monitor */
    //Serial.println("Giá trị SRF05:");
    //Serial.print(distanceSRF05);
    //Serial.println("cm");
    delay(500);
  //SRF05
  //CHE18
  int che18 = digitalRead(sensorCHE18);
  //Serial.println("Giá trị CHE18:");
  //Serial.println(che18); // 1 là trống, 0 là có
  delay(500);
  if(che18==0){
    digitalWrite(Light,1);
    WarnLight = "turnon";
  }
  else{
    digitalWrite(Light,0);
    WarnLight = "turnoff";
  }
  
    
  //CHE18
  //Motor
  
  //Encoder
  Rotation = Count/200.0;  // tinh ra số vòng quay được
  DistanceEncoder = Rotation * 0.79; // 1 vòng đi được 0.79cm
  //Serial.println("khoảng cách Encoder:");
  //Serial.println(DistanceEncoder);
  String DataRequest = "tag=ethernetup&email=datn@gmail.com&DistanceEncoder="+String(DistanceEncoder,2)+"&DistanceSRF05="+String(distanceSRF05,2)+"&StatusLight="+WarnLight;
  //Encoder
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.connect(server, 80)) {
    Serial.println("connecting to:");
    Serial.println(server);
   
    // Make a HTTP request:
    client.println("GET /triet-encoder.php HTTP/1.1");
    client.println("Host: mydatn.000webhostapp.com");
    client.println("Content-Type: application/x-www-form-urlencoded"); 
    client.print("Content-Length: "); 
    client.println("Connection: close");
    client.println(DataRequest.length()); 
    client.println(); 
    client.print(DataRequest); 
  }
  
  
  if (client.available()) { // nhận phản hồi từ server
     Serial.println("Nhan du lieu:");
     back = client.read();
     if (readback.length() < 100) {     //Lưu trữ các ký tự vào chuỗi
          readback += back;
          Serial.print(readback);
        }
    
    if(readback.indexOf("\"StatusButton\":\"thuan\"")!=-1)  //Nhấn nút quay tới
  {
    analogWrite(ENB, 160);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  if(readback.indexOf("\"StatusButton\":\"nghich\"")!=-1)  //Nhấn nút quay lui
  {
    analogWrite(ENB, 160);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  if(readback.indexOf("\"StatusButton\":\"dung\"")!=-1)  //Nhấn nút quay lui
  {
    analogWrite(ENB, 0);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
    // So sánh chuổi gửi về để điều khiển chiều động cơ
  }
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    //Serial.println();
    Serial.println("disconnecting.");
    client.stop();
//    //while (true);
    }
}

void pulse()
{
    if(digitalRead(5) == LOW)
      {Count++;}
    else
      {Count--;}
}

