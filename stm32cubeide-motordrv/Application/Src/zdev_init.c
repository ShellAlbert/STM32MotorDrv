/*
 * filename:zdev_init.c
 * versin:0.1.0
 * author:1811543668@qq.com
 * date: 2020/3/16
 */

#include <zadc_handle.h>
#include <zdev_init.h>
#include <zdistance_handle.h>
#include <zmodbus_rtu_handle.h>
#include "lens_task.h"
#include "drv_uart.h"
#include "2d_bracket.h"
#include "protect_structure.h"
#include "m62429.h"

struct lens lens;
struct bracket bracket;
struct protect_structure protect_structure;

void device_init(void)
{
  zsy_ADCInit();
  zsy_ModBusInit();
  zsy_DistanceInit();
  
  /******************************************************************************************/
 //  easer_flash();

 //  writer_u w;
 //  uint32_t res = 0;
 //  for(uint8_t i=0;i<50;i++)
 //  {
 //	 w.data[0] = '0'+i;
 //	 w.data[1] = '1'+i;
 //	 write_word_to_flash(w);
 //
 //	 usart1_printf("Addr: %x  %x\n",0x0801E000,read_word_from_flash(0x0801E000));
 //	 usart1_printf("Addr: %x  %x\n",0x0801E000+4,read_word_from_flash(0x0801E000+4));
 //	 usart1_printf("Addr: %x  %x\n",0x0801E000+8,read_word_from_flash(0x0801E000+8));
 //	 usart1_printf("Addr: %x  %x\n",0x0801E000+12,read_word_from_flash(0x0801E000+12));
 //	 usart1_printf("Addr: %x  %x\n",0x0801E000+16,read_word_from_flash(0x0801E000+16));
 //	 usart1_printf("Addr: %x  %x\n",0x0801E000+20,read_word_from_flash(0x0801E000+20));
 //
 //	 res = find_used_entry();
 //	 usart1_printf("%x\n",res);
 //  }

   /******************************************************************************************/

  lens_pid_register(&lens,"lens");
  bracket_register(&bracket,"bracket");
  protect_structure_register(&protect_structure,"protect_structure");
  m62429_init();
//  stop_distance_measure();
}
