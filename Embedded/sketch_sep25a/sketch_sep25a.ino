void setup() {
  Serial.begin(115200);
  pinMode(A0,INPUT);

}

void loop() {
  int data [10];
  int minVal = 100000;
  int maxVal = 0;
  for (int i = 0 ; i < 10 ; i++){
    data[i] = (analogRead(A0));
    if (data[i] < minVal) 
      minVal = data[i];
    if (data[i] > maxVal)
      maxVal = data[i];
    delay(100);
  }
  int count = 0;
  int avg = 0;
  for (int i = 0; i < 10 ; i ++){
    if (data[i]>(minVal+maxVal)/2){
      continue;
    }
    count++;
    avg += data[i];
  }
  avg = avg / count;
  Serial.println(data[9]);
  //Serial.println(avg);
}
