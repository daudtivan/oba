#include <12F675.h>
#device adc=10
#use delay(clock=4000000)
#fuses NOMCLR, PUT, NOBROWNOUT, NOPROTECT, INTRC_IO, NOWDT
#include <regs_12F6xx.h>

#rom 0x3ff = {0x34b4}

//DECLARAÇÃO DE VARIÁVEIS

#define vbat pin_a0 // tensão da bateria no pino 0
#define vpla pin_a1 // tensão do painel no pino 1
#define gate pin_a2 // Carregador de REDE
#define leda pin_a5 // Carregar por painel solar
#define rssg pin_a4 // ?? O QUE É O RSSG?
#define BAT 0
#define OWN 1      //fonte energia propria
#define AUX 2      //fonte energia auxiliar

float vplaca=0, vbateria=0;   //voltagens
unsigned int src;   //fonte de energia ativa

void main()
{
//definicoes de hardware
 CMCON=0x07;
 ANSEL=0x03;
 setup_adc(ADC_CLOCK_INTERNAL);
 //setup_adc_ports(A0_A1);

//inicializa com fontes inativas
 output_high(leda);   //led amarelo ativado
 output_low(gate);   //auxiliar inativo
 output_low(rssg);   //fonte principal inativa

//ativa fonte propria
 delay_ms(2000);   //aguarda 2 seg
 output_low(leda);   //desliga led
 output_high(rssg);   //liga fonte propria
 src=OWN;      //indica fonte propria

//monitora e atua nos meios de carregamento
 while(TRUE)
{
   //verifica fonte propria
   set_adc_channel(OWN);          //ler da fonte propria
   delay_ms(1);
   vplaca=read_adc()*0.03;         //leitura
   delay_ms(1);

   if(vplaca>=11.5) {         //se painel ativo (>12V)
      if (src==AUX) {         //no carregador?
         output_low(gate);       // DESLIGA CARREGADOR
         output_low(leda);       //desliga led amarelo
         output_high(rssg);       // LIGA PAINEL SOLAR
         src = OWN;      //muda para fonte propria
      }
   }
   else {               //painel inativo
      //verifica bateria
      set_adc_channel(BAT);       //ler da bateria
      delay_ms(1);
      vbateria=read_adc()*0.03;   //leitura
      delay_ms(1);
      if (vbateria<=10.2) {       // bateria SEM carga (<11V)
         if (src==AUX) {       //ja na auxiliar?
            /*ativa modo emergencia, com sirene e led ativos
            possivel falta de energia na fonte auxiliar */
            output_high(leda);    //aciona led emergencia

         }
         else {
            output_high(gate);       //ativa auxiliar
            output_low(leda);       //desativa led amarelo
            output_low(rssg);       //desliga fonte externa
            src = AUX;      //indica fonte auxiliar
         }
      }
   }
  delay_ms(2000);               //pausa 2seg
}   //fim loop principal
 }
