void setup() {
  pinMode(26,OUTPUT);
  digitalWrite(26,HIGH);
  Serial.begin(400000); // I am using Serial1 (pins 9,10).  If you are using USB serial monitor, switch to Serial
  Serial3.begin(1000000); // change this to 1000000 if you havent changed default baud
  delay(4000);
  Serial.println("Hit Space to send a command");

  pinMode(6,OUTPUT);
  digitalWrite(6,LOW);
}

void loop() {

  while (Serial3.available()){
    Serial.print(Serial3.read(),HEX);
    Serial.print("_");
  }

  while (Serial.available()){
    if (Serial.read()==' '){
      prepareCommand();
    }
  }
}

void prepareCommand(void){
  Serial.print("Enter device # or A for all:");
  while(!Serial.available());
  uint8_t input = Serial.read();
  uint8_t deviceNum = 255;
  if (input == 'A'){
    deviceNum = 254;
  }else if (input >='0' || input <='9'){
     deviceNum = input - '0'; 
  } else {
    Serial.println("Invalid Input");
    return;
  }
  Serial.print("\nSelect Instruction [P]ing,[M]ove,[R]ead,[W]rite:");
   while(!Serial.available());
  switch(Serial.read()){
   case 'P': pingServo(deviceNum);
   break;
   case 'M': moveServo(deviceNum);
   break;
   case 'R':  readServo(deviceNum);
   break;
   case 'W':writeServo(deviceNum);
   break;
//   case 'S':// syncWriteServo(deviceNum);
//   break;
   default: Serial.println("Invalid Instruction");
   return;
  }
}

void readServo(uint8_t deviceNum){
    Serial.print("\n Enter Address as 2 digits:");
  while(!Serial.available());
  uint8_t input = Serial.read()-'0';
    while(!Serial.available());
  input = input*10 + Serial.read()-'0';
  
   Serial.print("\n Read [1] or [2] bytes:");
    while(!Serial.available());
  uint8_t readCount = Serial.read()-'0';
 
   uint8_t parms[] = {input,readCount};
   execute(deviceNum,2,2, parms );
}

void writeServo(uint8_t deviceNum){
  Serial.print("\n Enter Address as 2 digits:");
  while(!Serial.available());
  uint8_t addr = Serial.read()-'0';
    while(!Serial.available());
  addr = addr*10 + Serial.read()-'0';
  Serial.print("\n Write [1] or [2] bytes:");
      while(!Serial.available());
  bool twobytes = Serial.read() == '2';
  if (twobytes){
       Serial.print("\n Enter Value as 5 digits:");
    while(!Serial.available());
  uint16_t val = (Serial.read()-'0')*10000;
      while(!Serial.available());
  val += (Serial.read()-'0')*1000;
      while(!Serial.available());
  val += (Serial.read()-'0')*100;
        while(!Serial.available());
  val += (Serial.read()-'0')*10;
        while(!Serial.available());
  val += (Serial.read()-'0');
    Serial.print(addr);
    Serial.print(",");
    Serial.print(val);
    Serial.print(" Hit space to execute, any other key to abort:");
  while(!Serial.available());
  if (Serial.read()!=' ')
  return;
   uint8_t parms[] = {addr,val%256,val>>8};
   execute(deviceNum,3,3, parms );
  }else{
   Serial.print("\n Enter Value as 3 digits:");
    while(!Serial.available());
  uint8_t val = (Serial.read()-'0')*100;
      while(!Serial.available());
  val += (Serial.read()-'0')*10;
      while(!Serial.available());
  val += Serial.read()-'0';
  
      Serial.print(addr);
    Serial.print(",");
    Serial.print(val);
    Serial.print(" Hit space to execute, any other key to abort:");
  while(!Serial.available());
  if (Serial.read()!=' ')
  return;
 
   uint8_t parms[] = {addr,val};
   execute(deviceNum,3,2, parms );
  }
}


void moveServo(uint8_t deviceNum){
     Serial.print("\n Choose position (Enter 4 digits(:");
  while(!Serial.available());
  uint16_t input = (Serial.read()-'0')*1000;
  while(!Serial.available());
   input += (Serial.read()-'0')*100;
    while(!Serial.available());
   input += (Serial.read()-'0')*10;
    while(!Serial.available());
   input += Serial.read()-'0';
   Serial.print("Moving to ");
    Serial.println(input);
   uint8_t parms[] = {30,input%256,input>>8};
   execute(deviceNum,3,3, parms );
}

void pingServo(uint8_t deviceNum){
  Serial.println("Sending Ping");

  execute(deviceNum,1,0,0);
}

void execute(uint8_t deviceNum,uint8_t command,uint8_t parmCount, uint8_t parms[]){
    digitalWrite(6,HIGH);
  uint16_t checksum = 0;
  Serial3.write(0xff);
  Serial3.write(0xff);
  Serial3.write(deviceNum);
  checksum+=deviceNum;
  Serial3.write(parmCount+2);
    checksum+=parmCount+2;
  Serial3.write(command);
    checksum+=command;
  for (uint8_t i = 0 ; i < parmCount ; i++){
    Serial3.write(parms[i]);
      checksum+=parms[i];
  }
  Serial3.write((uint8_t)(255-(checksum%256)));
  Serial3.flush();
    digitalWrite(6,LOW);
}
