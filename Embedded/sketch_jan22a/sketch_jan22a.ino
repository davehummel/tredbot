void setup() {
  Serial1.begin(3500000);

}
byte bytes[4];
void loop() {
  if (Serial1.readBytes(bytes,4)){
    Serial1.write(bytes,4);
    Serial1.readBytes(bytes,4);
  }
}
