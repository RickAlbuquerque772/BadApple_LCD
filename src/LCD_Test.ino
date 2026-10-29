#define LCD_ON                              // Habilitar LCD

#ifdef LCD_ON                               // Se habilitar LCD com interface 4 bits

#include <LiquidCrystal.h>
//#include "frames8x4_test.h"
#include "pixels.h"                  
#define LCD_RS GPIO_NUM_13
#define LCD_EN GPIO_NUM_12

#define LCD_D4 GPIO_NUM_14
#define LCD_D5 GPIO_NUM_27
#define LCD_D6 GPIO_NUM_26
#define LCD_D7 GPIO_NUM_25

#define DISP_CLEAR 0x01
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

const uint8_t row_offset[] = {0x04, 0x44, 0x14, 0x54}; // LCD 16x4 (atenção aos hexadecimais) mas quero usar 8x4

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7); 
#endif

struct st_reg{
  uint8_t ch[8][8];
};
struct st_reg reg[4];
uint32_t frame_inicial = 0; // Indice Importante: pular 2 frames -> colocar valor par
uint32_t frame_final = 6570; // Indice Max é 6570
uint32_t frame_atual = frame_inicial;
uint32_t pixel_atual = 0;
uint32_t posicao_pixel = 0;
uint8_t regiao = 0;
unsigned long tempo_anterior, tempo_atual, intervalo_tempo;

void introducao()
{
  lcd.setCursor(4,0);
  lcd.print("Tocando:");
  lcd.setCursor(3,2);
  lcd.print("BAD APPLE!!");
  delay(1000);
  lcd.setCursor(6,3);
  lcd.print("Em 3");
  delay(1000);
  lcd.setCursor(6,3);
  lcd.print("Em 2");
  delay(1000);
  lcd.setCursor(6,3);
  lcd.print("Em 1");
  delay(1000);
  LCD_CLEAR();
}

void meu_teste()
{
  for(int k = 0; k < 4; k++)
  {
    for(int i = 0; i < 8; i++)
    {
      for(int j = 0; j < 8; j++)
      {
        reg[k].ch[i][j] = 0b11111;
      }
    }
  }
}

void gera_frame()
{
  uint16_t k = posicao_pixel / (320); // 320 = (40*8)
  uint16_t i = (posicao_pixel % 40) / 5;
  uint16_t j = (posicao_pixel / 40) % 8;
  uint16_t shift = (posicao_pixel % 40) % 5;

  reg[k].ch[i][j] ^= (16 >> shift);
}

void avanca_frame()
{
  uint16_t quantidade_local = quantidade[frame_atual];
  int16_t vet_local; // Vai receber valores negativos -> usar signed
  while( quantidade_local > 0)
  {
    vet_local = vet0[pixel_atual];
    if( vet_local >= 0 )
    {
      posicao_pixel = vet_local;
      pixel_atual++;
      gera_frame();
      quantidade_local--;
    }
    else
    {
      while( vet_local < 0)
      {
        posicao_pixel++;
        vet_local++;
        gera_frame();
        quantidade_local--;
      }
      pixel_atual++;
    }
  }
  frame_atual += inc;
}

void setup() {
  
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_EN, OUTPUT);
  pinMode(LCD_D4, OUTPUT);
  pinMode(LCD_D5, OUTPUT);
  pinMode(LCD_D6, OUTPUT);
  pinMode(LCD_D7, OUTPUT);
  pinMode(2, OUTPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 4);


  //avanca_frame();
  introducao();
  tempo_anterior = esp_timer_get_time();
  
}

void loop() {
  uint8_t rowdata[] = {0, 1, 2, 3, 4, 5, 6, 7};
  uint8_t intervalo = 8; // Tempo maior ajuda a diminuir ghosting, mas reduz avanço de frame
                          // Cada +1 em intervalo adiciona 4ms no processamento 
  for(uint8_t i = 0; i < 8; i++)
  {
    DEF_CHAR(i, reg[regiao].ch[i] );
  }
  LCD_CLEAR();
  command(LCD_SETDDRAMADDR | (row_offset[regiao]) );
  data(rowdata, 8);
  vTaskDelay(intervalo);
  regiao++;
  regiao = regiao % 4;
  
  
  //if(minha_contagem >= 32)
  tempo_atual = esp_timer_get_time();
  intervalo_tempo = tempo_atual - tempo_anterior;
  printf("Intervalo de tempo: %lu %lu %lu\n", tempo_atual, tempo_anterior, intervalo_tempo);
  if(intervalo_tempo >= 65000)
  {
    if(frame_atual >= frame_final)
    {
      ESP.restart();
    }
    avanca_frame();
    tempo_anterior = esp_timer_get_time();
  }
  
}

void DEF_CHAR(uint8_t local, uint8_t* meu_char) // define 1 carácter especial
{                                              // trocar para uint8_t* meu_char quando receber vetor de carácteres
  command(LCD_SETCGRAMADDR | local << 3);
  data(meu_char, 8); // cada carácter tem 8 linhas
}

void LCD_CLEAR()
{
  command(DISP_CLEAR);
  esp_rom_delay_us(2000);
}

void command(uint8_t value)
{
  digitalWrite(LCD_RS, LOW);
  write4bits(value);
}

void data(uint8_t* dado, uint8_t size)
{
  digitalWrite(LCD_RS, HIGH);
  for(uint8_t i = 0; i < 8; i++)
  {
    write4bits(dado[i]);
  }
}

void write4bits(uint8_t value) // modo 4 bits
{
  uint8_t aux = value >> 4;
  digitalWrite(LCD_D4, (aux >> 0) & 0x01 );
  digitalWrite(LCD_D5, (aux >> 1) & 0x01 );
  digitalWrite(LCD_D6, (aux >> 2) & 0x01 );
  digitalWrite(LCD_D7, (aux >> 3) & 0x01 );
  pulseEnable();
  digitalWrite(LCD_D4, (value >> 0) & 0x01 );
  digitalWrite(LCD_D5, (value >> 1) & 0x01 );
  digitalWrite(LCD_D6, (value >> 2) & 0x01 );
  digitalWrite(LCD_D7, (value >> 3) & 0x01 );
  pulseEnable();
}

void pulseEnable()
{
  digitalWrite(LCD_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(LCD_EN, HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD_EN, LOW);
  delayMicroseconds(30); //Aumentar se der pau
}

