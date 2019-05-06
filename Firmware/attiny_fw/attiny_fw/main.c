/*
 * attiny_fw.c
 *
 * Created: 5/5/2019 5:54:51 AM
 * Author : Ronald Arthur
 */

#include <avr/io.h>
#include <util/delay.h>

#define N_1    (_BV(CS00))
#define N_8    (_BV(CS01))
#define N_64   (_BV(CS01)|_BV(CS00))
#define N_256  (_BV(CS02))
#define N_1024 (_BV(CS02)|_BV(CS00))

const int sPWMArray2[] = {25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 225, 200, 175, 150, 125, 100, 75, 50, 25};
/*
int ADC_Read = 0;
*/

static void pwm_init(void) {
	DDRB |= _BV(PB0); // set PWM pin as OUTPUT
	TCCR0A |= _BV(WGM01)|_BV(WGM00); // set timer mode to FAST PWM
	TCCR0A |= _BV(COM0A1); // connect PWM signal to pin (AC0A => PB0)
}

/*
When timer is set to Fast PWM Mode, the frequency can be
calculated using equation: F = F_CPU / (N * 256)
Example - Possible frequencies (@1.2MHz):
 -> F(N_1)    = 4.687kHz
 -> F(N_8)    = 585Hz
 -> F(N_64)   = 73Hz
 -> F(N_256)  = 18Hz
 -> F(N_1024) = 4Hz
*/
static void pwm_set_frequency(uint32_t N) {
	TCCR0B = (TCCR0B & ~((1 << CS02)|(1 << CS01)|(1 << CS00))) | N; // set prescaler
}

static void pwm_set_duty(uint8_t duty) {
	OCR0A = duty; // set the OCRnx
}

static void pwm_stop(void) {
	TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00)); // stop the timer
}

int main(void) {
	/* setup */
	DDRB |= _BV(PB1); // set pin as OUTPUT
	DDRB |= _BV(PB2); // set pin as OUTPUT

	DDRB &= ~_BV(PB4); // set pin as INPUT

	/*// AREF = AVcc
	ADMUX = (1 << REFS0);

	// ADC Enable and prescaler of 128
	ADCSRA = (1 << ADEN)|(1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);*/

	pwm_init();
	pwm_set_frequency(N_8);

	/* loop */
	while (1) {
		// Generate first 1/2 Cycle
		PORTB |= _BV(PB1);
		PORTB &= ~_BV(PB2);

		for(int i=0; i<20; i++) {
			pwm_set_duty(sPWMArray2[i]);
			_delay_us(500);
		}

		// Generate second 1/2 Cycle
		PORTB &= ~_BV(PB1);
		PORTB |= _BV(PB2);
		for(int i=0; i<20; i++) {
			pwm_set_duty(sPWMArray2[i]);
			_delay_us(500);
		}

		/*ADCSRA |= _BV(ADSC);
		ADMUX = (ADMUX & 0xF8) | 2;
		ADCSRA |= (1 << ADSC);

		while((ADCSRA & (1 << ADSC)));
		ADC_Read = (ADCH << 8) | ADCL;*/
	}

	pwm_stop();
	return -1;
}

