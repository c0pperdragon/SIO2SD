void setup() {
//    Serial1.begin(19200, SERIAL_8N1);  
}

void loop() {
    delay(500);
    Serial1.begin(19200, SERIAL_8N1);  
    delay(500);
    Serial1.end();
        
//    Serial1.print("Hi");
//    Serial1.flush();
//    digitalWrite(1,HIGH);
    
}

