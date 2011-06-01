/*
 * File      : lcd.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2011, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-03-05     lgnq         ZYMG12864C3 LCD driver
 */

#include <rtthread.h>
#include "lcd.h"
#include "font.h"

static rt_uint8_t gui_disp_buf[GUI_LCM_YMAX/8][GUI_LCM_XMAX];
const unsigned char BIT_MASK[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
/* simple font: ' ', '0'~'9','a'~'z','A'~'Z' */
extern const unsigned char  FONTTYPE8_8[][8];

rt_uint32_t x;
rt_uint32_t y;

struct rtgui_lcd_device
{
    struct rt_device parent;
    rt_uint16_t width;
    rt_uint16_t height;
    void* hw_framebuffer;
};
static struct rtgui_lcd_device *lcd = RT_NULL;

void Power_Delay(void)
{
    rt_uint32_t i=0x4ffff;
    while(i--)
        ;//asm("nop");
}

void Delay(void)
{
    rt_uint8_t i=0x8;
    while(i--)
        ;//asm("nop");
}

void Reset_Delay(void)
{
    rt_uint8_t i=0xff;
    while(i--)
        ;//asm("nop");
}

void LCD_WriteCmd(unsigned char command)
{
    rt_uint8_t i;

    LCD_PS_LOW();
    LCD_CS_LOW();
    LCD_CD_LOW();
    for(i=0; i<8; i++)
    {
        if (command & (0x80 >> i))
            LCD_DATA_HIGH();
        else
            LCD_DATA_LOW();
            
        LCD_CLK_LOW();
        Delay();
        LCD_CLK_HIGH();
        Delay();
    }
    LCD_CS_HIGH();
}

void LCD_WriteData(unsigned char data)
{
    rt_uint8_t i;

    LCD_PS_LOW();
    LCD_CS_LOW();
    LCD_CD_HIGH();
    for(i=0; i<8; i++)
    {
        if (data & (0x80 >> i))
            LCD_DATA_HIGH();
        else
            LCD_DATA_LOW();
        
        LCD_CLK_LOW();
        Delay();
        LCD_CLK_HIGH();
        Delay();
    }
    LCD_CS_HIGH();
}

#ifdef RT_USING_RTGUI
#include <rtgui/driver.h>
#include <rtgui/color.h>

static void rt_hw_lcd_update(rtgui_rect_t *rect)
{
    rt_uint8_t i,j = GUI_LCM_XMAX;
    rt_uint8_t* p = (rt_uint8_t*)gui_disp_buf;  
    
    for (i=0; i<GUI_LCM_PAGE; i++)
    { 
        LCD_WriteCmd(Set_Page_Addr_0|i);    
        LCD_WriteCmd(Set_ColH_Addr_0);        
        LCD_WriteCmd(Set_ColL_Addr_0);
        j = GUI_LCM_XMAX;
        while (j--)
        {
            LCD_WriteData(*p++);
            Delay();
        }
    }
}

static rt_uint8_t * rt_hw_lcd_get_framebuffer(void)
{
    return(rt_uint8_t *)gui_disp_buf;
}

static void rt_hw_lcd_set_pixel(rtgui_color_t *c, rt_base_t x, rt_base_t y)
{
    rt_uint8_t page;
    page = y/8;

    if (*c == black)
        gui_disp_buf[page][x] |= 1<<(y%8);
    else 
        if (*c == white)
            gui_disp_buf[page][x] &= ~(1<<(y%8));
}

static void rt_hw_lcd_get_pixel(rtgui_color_t *c, rt_base_t x, rt_base_t y)
{
    rt_uint8_t page;
    page = y/8;
    
    if (gui_disp_buf[page][x] & (1<<(y%8)))
        *c = black;
    else
        *c = white;
}

static void rt_hw_lcd_draw_hline(rtgui_color_t *c, rt_base_t x1, rt_base_t x2, rt_base_t y)
{
    rt_uint8_t page;
	rt_uint8_t i;
    page = y/8;
  
    for (i=x1; i<x2; i++)
    {
        if (*c == black)
            gui_disp_buf[page][i] |= 1<<(y%8);
        else 
            if (*c == white)
                gui_disp_buf[page][i] &= ~(1<<(y%8));      
    }
}

static void rt_hw_lcd_draw_vline(rtgui_color_t *c, rt_base_t x, rt_base_t y1, rt_base_t y2)
{
    rt_uint8_t y;

    for (y = y1; y < y2; y ++)
    {
        rt_hw_lcd_set_pixel(c, x, y);
    }
}

static void rt_hw_lcd_draw_raw_hline(rt_uint8_t *pixels, rt_base_t x1, rt_base_t x2, rt_base_t y)
{
    rt_uint8_t coll; 
	rt_uint8_t colh; 
	rt_uint8_t page;
	rt_uint8_t i;

    page = y/8;
  
    for (i=x1; i<x2; i++)
    {
        gui_disp_buf[page][i] |= 1<<(y%8);
        coll = i & 0x0f;
        colh = i >> 4;
        LCD_WriteCmd(Set_Page_Addr_0 | page);
        LCD_WriteCmd(Set_ColH_Addr_0 | colh);
        LCD_WriteCmd(Set_ColL_Addr_0 | coll);
        LCD_WriteData(gui_disp_buf[page][i]);
    }
}

struct rtgui_graphic_driver _rtgui_lcd_driver =
{
    "lcd",
    1,
    LCD_WIDTH,
    LCD_HEIGHT,
    rt_hw_lcd_update,
    rt_hw_lcd_get_framebuffer,
    rt_hw_lcd_set_pixel,
    rt_hw_lcd_get_pixel,
    rt_hw_lcd_draw_hline,
    rt_hw_lcd_draw_vline,
    rt_hw_lcd_draw_raw_hline
};
#endif

void lcd_io_init()
{
    /* Release the analog input function*/
    FM3_GPIO->ADE =0x03;
    /*Select CPIO function*/
    LCD_CS_PFR &= ~LCD_CS;
    /*Make pin output*/
    LCD_CS_DDR |= LCD_CS;
    /*Select CPIO function*/
    LCD_CD_PFR &= ~LCD_CD;
    /*Make pin output*/
    LCD_CD_DDR |= LCD_CD;
    /*Select CPIO function*/
    LCD_PS_PFR &= ~LCD_PS;
    /*Make pin output*/
    LCD_PS_DDR |= LCD_PS;    
    /*Select CPIO function*/
    LCD_CLK_PFR &= ~LCD_CLK;
    /*Make pin output*/
    LCD_CLK_DDR |= LCD_CLK;
    /*Select CPIO function*/
    LCD_DATA_PFR &= ~LCD_DATA;
    /*Make pin output*/
    LCD_DATA_DDR |= LCD_DATA;
}

/* RT-Thread Device Interface */
static rt_err_t rt_lcd_init (rt_device_t dev)
{
    lcd_io_init();
    
    Power_Delay();
    LCD_WriteCmd(Display_Off);
    Reset_Delay();
    // Resetting circuit
    LCD_WriteCmd(Reset_LCD);
    Reset_Delay();
    // LCD bias setting
    LCD_WriteCmd(Set_LCD_Bias_9);
    Reset_Delay();
    // ADC selection: display from left to right
    LCD_WriteCmd(Set_ADC_Normal);        
    Reset_Delay();
    // Common output state selection: display from up to down
    LCD_WriteCmd(COM_Scan_Dir_Reverse);
    Reset_Delay();
      
    LCD_WriteCmd(Power_Booster_On);
    Power_Delay(); // 50ms requried
    LCD_WriteCmd(Power_Regulator_On);
    Power_Delay(); // 50ms
    LCD_WriteCmd(Power_Follower_On);
    Power_Delay(); // 50ms
      
    // Setting the built-in resistance radio for regulation of the V0 voltage
    // Electronic volume control
    // Power control setting
    LCD_WriteCmd(Set_ElecVol_Reg|0x05);
    Delay();
    LCD_WriteCmd(Set_ElecVol_Mode);
    Delay();
    LCD_WriteCmd(Set_ElecVol_Reg);
    Delay();
    //  LCD_Clear();
    Delay();
    LCD_WriteCmd(Set_Page_Addr_0);
    Delay();
    LCD_WriteCmd(Set_ColH_Addr_0);
    Delay();
    LCD_WriteCmd(Set_ColL_Addr_0);
    Delay();
    LCD_WriteCmd(Display_On);
    Delay();
      
    LCD_WriteCmd(Display_All_On);
    Delay();
    LCD_WriteCmd(Display_Off);
    Delay();
    LCD_WriteCmd(Display_On);
    Delay();
    LCD_WriteCmd(Display_All_Normal);
    Delay();
    
    return RT_EOK;
}

/*******************************************************************************
* Function Name  : LCD_FillAll
* Description    : Fill the whole LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_FillAll(unsigned char*	buffer)
{
  unsigned char i,j = GUI_LCM_XMAX;
  unsigned char* p = buffer;  
	
  for (i=0; i<GUI_LCM_PAGE; i++)
  { 
    LCD_WriteCmd(Set_Page_Addr_0|i);	
    LCD_WriteCmd(Set_ColH_Addr_0);		
    LCD_WriteCmd(Set_ColL_Addr_0);
    j = GUI_LCM_XMAX;
    while (j--)
    {
      LCD_WriteData(*p++);
      Delay();
    }
  }
}

/*******************************************************************************
* Function Name  : LCD_ClearSCR
* Description    : clean screen
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_ClearSCR(void)
{
  unsigned char i, j;

  for(i=0; i<GUI_LCM_PAGE; i++)
  { 
    for(j = 0; j < GUI_LCM_XMAX; j++) 
      gui_disp_buf[i][j] = 0;
  }
  LCD_FillAll((unsigned char*)gui_disp_buf);
}

/****************************************************************************
* Function Name  : LCD_UpdatePoint
* Description    : refresh the point in screen 
* Input          : x      X-coordinate
                   y      Y-coordinate
* Output         : None
* Return         : None
****************************************************************************/

void  LCD_UpdatePoint(unsigned int x, unsigned int y)
{
  unsigned char coll, colh, page;
  page = y / 8;
  coll = x & 0x0f;
  colh = x >> 4;
	
  LCD_WriteCmd(Set_Page_Addr_0 | page);	        // page no.
  LCD_WriteCmd(Set_ColH_Addr_0 | colh);		// fixed col first addr
  LCD_WriteCmd(Set_ColL_Addr_0 | coll);
  LCD_WriteData(gui_disp_buf[page][x]);
}

/****************************************************************************
* Function Name  : LCD_PutChar
* Description    : output a char to screen 
                  (the char only can be ' ','0'~'9','A'~'Z','a'~'z')
* Input          : x      X-coordinate
                   y      Y-coordinate
                   ch     character
* Output         : None
* Return         : 1    Success
                   0    Fail
****************************************************************************/
unsigned char  LCD_PutChar(unsigned long x, unsigned long y, unsigned char ch)
{  
   unsigned char data;
   unsigned char i, j;

   if( x >=(GUI_LCM_XMAX-8) ) return(0);
   if( y >=(GUI_LCM_YMAX-8) ) return(0);
   
   if(ch == 0x20)
     ch -= 0x20;
     else if((ch >= 0x30)&&(ch <= 0x39))
       ch -= 0x2f;
       else if((ch >= 0x41)&&(ch <= 0x5a))
         ch -= 0x36;
         else if((ch >= 0x61)&&(ch <= 0x7a))
          ch -= 0x3C;
          else
            return(0);
    
   for(i = 0; i < 8; i++)
   {  
      data = FONTTYPE8_8[ch][i];
      
      for(j = 0; j < 8; j++)
      {  
         if( (data&BIT_MASK[j]) == 0)
           gui_disp_buf[y / 8][x] &= (~(0x01 << ( y % 8)));
         else  
           gui_disp_buf[y / 8][x] |= (0x01 <<( y % 8));
         LCD_UpdatePoint(x, y);
         x ++;
      }
      x -= 8;								
      y++;									
   }
   
   return(1);
}

/****************************************************************************
* Function Name  : LCD_PutString
* Description    : output string to screen 
* Input          : x      X-coordinate
                   y      Y-coordinate
                  str     pointer to string
* Output         : None
* Return         : None
****************************************************************************/
void  LCD_PutString(unsigned long x, unsigned long y, char *str)
{  
  while(1)
  {  
    if( (*str)=='\0' ) break;
    if( LCD_PutChar(x, y, *str++) == 0 ) break;
    x += 6;								
  }
}

static rt_err_t rt_lcd_control (rt_device_t dev, rt_uint8_t cmd, void *args)
{
    switch (cmd)
    {
        case RT_DEVICE_CTRL_LCD_GET_WIDTH:
            *((rt_uint16_t*)args) = lcd->width;
            break;
        case RT_DEVICE_CTRL_LCD_GET_HEIGHT:
            *((rt_uint16_t*)args) = lcd->height;
            break;
        case RT_DEVICE_CTRL_LCD_DISPLAY_ON:
            LCD_WriteCmd(Display_On);
            break;
        case RT_DEVICE_CTRL_LCD_DISPLAY_OFF:
            LCD_WriteCmd(Display_Off);
            break;        
        case RT_DEVICE_CTRL_LCD_PUT_STRING:
            LCD_PutString(x, y, (char*)args);
            break;
        case RT_DEVICE_CTRL_LCD_CLEAR_SCR:
            LCD_ClearSCR();
            break;
    }
    return RT_EOK;
}

static rt_err_t rt_lcd_open(rt_device_t dev, rt_uint16_t oflag)
{    
    RT_ASSERT(dev != RT_NULL);
    
    return RT_EOK;
}

static rt_err_t rt_lcd_close(rt_device_t dev)
{    
    RT_ASSERT(dev != RT_NULL);

    return RT_EOK;
}

void rt_hw_lcd_init(void)
{
    lcd = (struct rtgui_lcd_device*)rt_malloc(sizeof(struct rtgui_lcd_device));
    if (lcd == RT_NULL) 
        return; /* no memory yet */

    /* init device structure */
    lcd->parent.type = RT_Device_Class_Unknown;
    lcd->parent.init = rt_lcd_init;
    lcd->parent.open = rt_lcd_open;
    lcd->parent.close = rt_lcd_close;
    lcd->parent.control = rt_lcd_control;
    lcd->parent.user_data = RT_NULL;
    lcd->width = LCD_WIDTH;
    lcd->height = LCD_HEIGHT;
    lcd->hw_framebuffer = (void*)gui_disp_buf;
    
    /* register touch device to RT-Thread */
    rt_device_register(&(lcd->parent), "lcd", RT_DEVICE_FLAG_RDWR);

#ifdef RT_USING_RTGUI
    /* add lcd driver into graphic driver */
    rtgui_graphic_driver_add(&_rtgui_lcd_driver);
#endif
}
