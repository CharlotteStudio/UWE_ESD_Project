String SerialToStringUntil(char c) {
  while (Serial.available()) {
    return Serial.readStringUntil(c);
  }
  return "";
}

String* SplitString(String str, char c, int size){
  int count = 0;
  String* strings = new String[size];

  while (str.length() > 0)
  {
    int index = str.indexOf(c);
    if (index == -1)
    {
      strings[count++] = str;
      break;
    }
    else
    {
      strings[count++] = str.substring(0, index);
      str = str.substring(index + 1);
    }
  }
  return strings;
}