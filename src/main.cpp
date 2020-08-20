#include <Arduino.h>


const int dht_pin = 7;
const int length_dht_read = 40;
const int first_byte_start = 0;
const int second_byte_start = 8;
const int third_byte_start = 16;
const int fourth_byte_start = 24;
const int fifth_byte_start = 32;

uint8_t x = 1;
uint8_t i;
uint8_t data[length_dht_read];
uint8_t bytes_read = 0;

uint8_t high_humid;
uint8_t low_humid;
uint8_t high_temp;
uint8_t low_temp;
uint8_t read_check_sum;
uint8_t true_check_sum;
uint8_t failed_checksum_counter = 0;
int uint8_place_counter;

void initialize_reading();

int power_exponent(int base, int exponent)
{
    int result = 1;
    int i;

    for (i = 0; i < (exponent); i++)
    {
        result = result * base;
    }
    return result;

}


void setup() 
{
  Serial.begin(9600);
  DDRD = B10000000; //Set DDRD pin 7 (PD7) as an output
  PORTD = (1 << dht_pin) | PORTD; // pull DDRD7 high
  _delay_ms(1500);  //the DHT11 wants atleast 1000ms to settle after start up
}

void loop()
{
  
    initialize_reading();
    
    
    for (i = 0; i < length_dht_read; i++)
    {
      Serial.print(data[i]);
    }

    Serial.print("\n");

    Serial.print("Temperatur: ");
    Serial.print(high_temp);
    Serial.print(".");
    Serial.print(low_temp);
    Serial.print(" C");
    Serial.print("\n");

    Serial.print("Humidity: ");
    Serial.print(high_humid);
    Serial.print(".");
    Serial.print(low_humid);
    Serial.print(" %");
    Serial.print("\n");
     


    if(true_check_sum == read_check_sum)
      {
        Serial.print("Check sum adds up, successful read!\n");
        failed_checksum_counter = 0;
      }
    else
    {
      Serial.print("Check sum is incorrect\n");
      failed_checksum_counter++;
    }

    _delay_ms(5000);
    
}


  

void initialize_reading()
{
    PORTD = (0 << dht_pin) & PORTD; //pull PD7 low and wait >18ms to initialize read
    _delay_ms(30);
    PORTD = (1 << dht_pin) | PORTD; // pull PD7 high again and wait for DHT response (20-40us)
    _delay_us(20);
    //PORTD = (0 << dht_pin) & PORTD; // disconnect internal pull-up resistor
    DDRD = (0 << dht_pin) & PORTD; // configure PD7 as input
    
    while((PIND >> dht_pin) == 1); // wait for the 80us low pulse
    _delay_us(1);
    while((PIND >> dht_pin) == 0); // in the 80us low pulse
    _delay_us(1);
    while((PIND >> dht_pin) == 1); // in the 80us high pulse before the data
    
    for(i = 0; i < 40; i++)     // 40-bit stream from DHT11, if pulse > 35us its a 1
    {
      while((PIND >> dht_pin) == 0);
      _delay_us(35);
      if((PIND >> dht_pin) == 0)
      {
        data[i] = 0;
      }
      else if((PIND >> dht_pin) == 1)
      {
        data[i] = 1;
        while((PIND >> dht_pin) == 1);
      }
    }

    for(i = first_byte_start; i < second_byte_start; i++) // first byte translated to a unit8
  {
    uint8_place_counter = 7 - i;
    if(data[i] == 1)
    {
    high_humid += power_exponent(2, uint8_place_counter);
    }
  }

  
  for(i = second_byte_start; i < third_byte_start; i++) // second byte translated to a unit8
  {

    if(data[i] == 1)
    {
    low_humid += power_exponent(2, (7-(i - second_byte_start)) );
    }
  }


  for(i = third_byte_start; i < fourth_byte_start; i++) // third byte translated to a unit8
  {
    if(data[i] == 1)
    {
    high_temp += power_exponent(2, (7-(i - third_byte_start)));
    }
  }

  for(i = fourth_byte_start; i < fifth_byte_start; i++) // fourth byte translated to a unit8
  {
    if(data[i] == 1)
    {
    low_temp += power_exponent(2, (7-(i - fourth_byte_start)));
    }
  }

  for(i = fifth_byte_start; i < length_dht_read; i++) // fifth byte translated to a unit8
  {
    if(data[i] == 1)
    {
    read_check_sum += power_exponent(2, (7-(i - fifth_byte_start)));
    }
  }

  true_check_sum = high_humid + low_humid + high_temp + low_temp;


}
