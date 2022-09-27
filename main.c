/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#include "TM4C123GH6PM.h"
#include "stdint.h"
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
volatile static uint32_t adcResult = 0;

//Function for interrupt from ADC
void ADC1SS3_Handler (void)
{
  adcResult = ADC1 ->SSFIFO3;  //read adc coversion result from SS3 FIFO
  ADC1->ISC = (1<<3); 
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/



int main()
{
  //Initialization sequence for the ADC is as follows:
  
  //1. Enable the ADC clock using the RCGCADC register (see page 352).
  SYSCTL->RCGCADC =(1<<1);
  GPIOF->PUR |= 0x10;        // Enable Pull Up resistor PF4
  //2. Enable the clock to the appropriate GPIO modules via the RCGCGPIO register (see page 340).
  //To find out which GPIO ports to enable, refer to “Signal Description” on page 801.
  SYSCTL->RCGCGPIO = (1<<4)|(1<<5); // Port E will be used for Input, Port F will be used for LED PF3 and switch PF4        
  GPIOE->DIR &= ~(1<<1); //changing port E pin 2 to input  PE1
  
  
  // Configure the LED Pin of Port F
  GPIOF->DIR |= 0x08;     //using switch 1 as input and led green as output
  GPIOF->DEN = 0xFF;      //port F as digital


  
  //3. Set the GPIO AFSEL bits for the ADC input pins (see page 671). To determine which GPIOs to
  //configure, see Table 23-4 on page 1344.
  GPIOE->AFSEL = (1<<1);    //PE1(alternate function select) 
  
  //4. Configure the AINx signals to be analog inputs by clearing the corresponding DEN bit in the
  //GPIO Digital Enable (GPIODEN) register (see page 682).
  GPIOE->DEN &= ~(1<<1);
  
  //5. Disable the analog isolation circuit for all ADC input pins that are to be used by writing a 1 to
  //the appropriate bits of the GPIOAMSEL register (see page 687) in the associated GPIO block.
  GPIOE->AMSEL = (1<<1);
  
  //6. If required by the application, reconfigure the sample sequencer priorities in the ADCSSPRI
  //register. The default configuration has Sample Sequencer 0 with the highest priority and Sample
  //Sequencer 3 as the lowest priority.
  
  /*----------------------------------------------------------------------------*/
  /*----------------------------------------------------------------------------*/
  
  
  /*----------------------------------------------------------------------------*/
  /*----------------------------------------------------------------------------*/
  //Sample Sequencer configuration should be as follows:
  
  
  //1. Ensure that the sample sequencer is disabled by clearing the corresponding ASENn bit in the
  //ADCACTSS register. Programming of the sample sequencers is allowed without having them
  //enabled. Disabling the sequencer during programming prevents erroneous execution if a trigger
  //event were to occur during the configuration process.
  ADC1->ACTSS &= ~(1<<3);
  
  //2. Configure the trigger event for the sample sequencer in the ADCEMUX register.
  ADC1->EMUX = (0x4 <<12);  //  OR  ADC1->EMUX=0x4000
  //***************************************************************************interrup part******************************************************************//
  
  GPIOF->ADCCTL = (1<<4); //to configure source of ac trigger
  GPIOF->IM = (1<<4);
  
  //**********************************************************************************************************************************************************//
  
  //3. When using a PWM generator as the trigger source, use the ADC Trigger Source Select
  //(ADCTSSEL) register to specify in which PWM module the generator is located. The default
  //register reset selects PWM module 0 for all generators.
  // Not Needed
  
  //4. For each sample in the sample sequence, configure the corresponding input source in the
  //ADCSSMUXn register.
  ADC1->SSMUX3 = 2; //1st sample input select( a value of 0x2 indicates the input is AIN2 that is PE1 pg.801)
  
  //5. For each sample in the sample sequence, configure the sample control bits in the corresponding
  //nibble in the ADCSSCTLn register. When programming the last nibble, ensure that the END bit
  //is set. Failure to set the END bit causes unpredictable behavior.
  ADC1->SSCTL3 = 0x6;  //no TS0 D0, yes IE0 END0
  
  //6. If interrupts are to be used, set the corresponding MASK bit in the ADCIM register.
  ADC1->IM = (1<<3);
  
  //7. Enable the sample sequencer logic by setting the corresponding ASENn bit in the ADCACTSS
  //register.
  ADC1->ACTSS |= (1<<3);
  ADC1->ISC = (1<<3);    //status and clear
  
  NVIC_EnableIRQ(ADC1SS3_IRQn);
  
  while (1)
{
    if (adcResult<1861 && adcResult>1620)   //voltage between 1.3v and 1.5v
    {
      GPIOF->DATA |= (1<<3);
    }
    else
    {
      GPIOF->DATA &= ~(1<<3);
    }
  }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/