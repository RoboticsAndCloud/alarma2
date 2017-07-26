#define MAX_CMD_BUF_LEN 20
uint8_t cmd_rcv_buffer[MAX_CMD_BUF_LEN + 1];
uint8_t cmd_rcv_buffer_cnt = 0;


#define CMD_HELP '?'
#define CMD_LIGHTS 'L'
#define CMD_BEEP 'B'
#define CMD_MELODY 'M'
#define CMD_ALARM 'A'
#define CMD_STOP 'S'
#define CMD_TEXT 'T'


bool cmd_parse(Stream& io);
int cmd_parse_dec(unsigned char pos);


void cmd_add_char(Stream& io)
{
    char ch = io.read();
    if(ch < ' ' || ch > 'z')
    {
        if(cmd_rcv_buffer_cnt != 0)
        {
            cmd_rcv_buffer[cmd_rcv_buffer_cnt] = 0;
            bool ok = cmd_parse(io);
            cmd_rcv_buffer_cnt = 0;
            io.println(ok ? F("ok") : F("err"));
        }
    }
    else
    {
        if(cmd_rcv_buffer_cnt < MAX_CMD_BUF_LEN)
        {
            cmd_rcv_buffer[cmd_rcv_buffer_cnt++] = ch;
        }
    }
}


bool cmd_parse(Stream& io)
{
    int int_param;
  
/*
    switch(cmd_rcv_buffer[0])
    {
    case CMD_LIGHTS:
        if(cmd_rcv_buffer_cnt == 4)
        {
            bool r = (cmd_rcv_buffer[1]=='1' ? true : false);
            bool g = (cmd_rcv_buffer[2]=='1' ? true : false);
            bool b = (cmd_rcv_buffer[3]=='1' ? true : false);
            led_set_all(r, g, b);
        }
        return true;
      
    case CMD_BEEP:
        sound_beep();
        return true;
      
    case CMD_MELODY:
        int_param = cmd_parse_dec(1);
        if(int_param != -1) {
            return sound_melody(int_param);
        }
        return false;
      
    case CMD_STOP:
        led_set_all(false, false, false);
        return true;

    case CMD_TEXT:
//FIXME      oled_set_text((const char*)&bt_rcv_buffer[1]);
        return true;
      
    case CMD_HELP:
        io.println(F("B M[NUM] L[01][01][01] T[text]"));
        return true;
    }
*/
    return false;
}


int cmd_parse_dec(unsigned char pos)
{
  int num = 0;

  // find the end of the number. endpos points after the end.
  unsigned char endpos = pos;
  for(; endpos<cmd_rcv_buffer_cnt; ++endpos) {
    if(cmd_rcv_buffer[endpos] < '0' || cmd_rcv_buffer[endpos] > '9') {
      break;
    }
  }
  if(endpos==pos) {
    return -1;
  }
  
  Serial.print(F("endpos: "));
  Serial.println(endpos);

  int factor = 1;
  for(; endpos != pos; --endpos) {
    num += (cmd_rcv_buffer[endpos-1] - '0') * factor;
    factor *= 10;
  }

  Serial.print(F("bt-dec-num: "));
  Serial.print(num);
  return num;
}
