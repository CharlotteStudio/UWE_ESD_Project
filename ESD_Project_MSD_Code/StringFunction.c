void ParseString(const char *input, int *output) {
    int value = 0;
    int index = 0;
    while (*input != '\0')
    {
        if (*input == ',')
        {
            output[index++] = value;
            value = 0;
        } else if (*input >= '0' && *input <= '9')
        {
            value = value * 10 + (*input - '0');
        }
        input++;
    }
    output[index] = value;
}

void intToStr(int num, char* str) {
    int i = 0;
    int isNegative = 0;
    
    // if negative
    if (num < 0) {
        isNegative = 1;
        num = -num;
    }
    
    // if 0
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    while (num != 0) {
        int rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }
    
    // if negative
    if (isNegative) {
        str[i++] = '-';
    }
    
    // end
    str[i] = '\0';
    
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void CreateAlarmCommand(int hours, int minutes, char* output) {
    int pos = 0;
    
    output[pos++] = '3';
    output[pos++] = ',';
    
    // add hours
    intToStr(hours, &output[pos]);
    while(output[pos] != '\0') pos++;

    output[pos++] = ',';
    
    // add minutes
    intToStr(minutes, &output[pos]);
}