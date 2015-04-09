void setup() {
  Serial1.begin(460800); // I am using Serial1 (pins 9,10).  If you are using USB serial monitor, switch to Serial
  Serial3.begin(1000000); // change this to 1000000 if you havent changed default baud
  Serial1.println("Hit Enter to send a command");
  pinMode(13,OUTPUT);
}

void loop() {
  while (Serial3.available()){
    Serial1.print(Serial3.read());
    Serial1.print("_");
  }
  while (Serial1.available()){
    if (Serial1.read()==13){
      prepareCommand();
    }
  }
}

void prepareCommand(void){
  Serial1.print("Enter device # or A for all:");
  while(!Serial1.available());
  uint8_t input = Serial1.read();
  uint8_t deviceNum = 255;
  if (input == 'A'){
    deviceNum = 254;
  }else if (input >='0' || input <='9'){
     deviceNum = input - '0'; 
  } else {
    Serial1.println("Invalid Input");
    return;
  }
  Serial1.print("\nSelect Instruction [P]ing,[M]ove,[R]ead,[W]rite:");
   while(!Serial1.available());
  switch(Serial1.read()){
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
   default: Serial1.println("Invalid Instruction");
   return;
  }
}

void readServo(uint8_t deviceNum){
    Serial1.print("\n Enter Address as 2 digits:");
  while(!Serial1.available());
  uint8_t input = Serial1.read()-'0';
    while(!Serial1.available());
  input = input*10 + Serial1.read()-'0';
  
   Serial1.print("\n Read [1] or [2] bytes:");
    while(!Serial1.available());
  uint8_t readCount = Serial1.read()-'0';
 
   uint8_t parms[] = {input,readCount};
   execute(deviceNum,2,2, parms );
}

void writeServo(uint8_t deviceNum){
  Serial1.print("\n Enter Address as 2 digits:");
  while(!Serial1.available());
  uint8_t addr = Serial1.read()-'0';
    while(!Serial1.available());
  addr = addr*10 + Serial1.read()-'0';
  Serial1.print("\n Write [1] or [2] bytes:");
      while(!Serial1.available());
  bool twobytes = Serial1.read() == '2';
  if (twobytes){
       Serial1.print("\n Enter Value as 5 digits:");
    while(!Serial1.available());
  uint16_t val = (Serial1.read()-'0')*10000;
      while(!Serial1.available());
  val += (Serial1.read()-'0')*1000;
      while(!Serial1.available());
  val += (Serial1.read()-'0')*100;
        while(!Serial1.available());
  val += (Serial1.read()-'0')*10;
        while(!Serial1.available());
  val += (Serial1.read()-'0');
    Serial1.print(addr);
    Serial1.print(",");
    Serial1.print(val);
    Serial1.print(" Hit space to execute, any other key to abort:");
  while(!Serial1.available());
  if (Serial1.read()!=' ')
  return;
   uint8_t parms[] = {addr,val%256,val>>8};
   execute(deviceNum,3,3, parms );
  }else{
   Serial1.print("\n Enter Value as 3 digits:");
    while(!Serial1.available());
  uint8_t val = (Serial1.read()-'0')*100;
      while(!Serial1.available());
  val += (Serial1.read()-'0')*10;
      while(!Serial1.available());
  val += Serial1.read()-'0';
  
      Serial1.print(addr);
    Serial1.print(",");
    Serial1.print(val);
    Serial1.print(" Hit space to execute, any other key to abort:");
  while(!Serial1.available());
  if (Serial1.read()!=' ')
  return;
 
   uint8_t parms[] = {addr,val};
   execute(deviceNum,3,2, parms );
  }
}


void moveServo(uint8_t deviceNum){
     Serial1.print("\n Choose position (Enter 4 digits(:");
  while(!Serial1.available());
  uint16_t input = (Serial1.read()-'0')*1000;
  while(!Serial1.available());
   input += (Serial1.read()-'0')*100;
    while(!Serial1.available());
   input += (Serial1.read()-'0')*10;
    while(!Serial1.available());
   input += Serial1.read()-'0';
   Serial1.print("Moving to ");
    Serial1.println(input);
   uint8_t parms[] = {30,input%256,input>>8};
   execute(deviceNum,3,3, parms );
}

void pingServo(uint8_t deviceNum){
  Serial1.println("Sending Ping");
  execute(deviceNum,1,0,0);
}

void execute(uint8_t deviceNum,uint8_t command,uint8_t parmCount, uint8_t parms[]){
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
}
