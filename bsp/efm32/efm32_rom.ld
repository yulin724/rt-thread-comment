/******************************************************************//**
 * @file 		efm32_rom.ld
 * @brief 	Linker script for EFM32 with GNU ld
 * 	COPYRIGHT (C) 2011, RT-Thread Development Team
 * @author 	Bernard, onelife
 * @version 	0.4 beta
 **********************************************************************
 * @section License
 * The license and distribution terms for this file may be found in the file LICENSE in this 
 * distribution or at http://www.rt-thread.org/license/LICENSE
 **********************************************************************
 * @section Change Logs
 * Date			Author		Notes
 * 2009-10-14 	Bernard 		first version
 * 2010-12-22	onelife		Modify for EFM32
 *********************************************************************/
OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(Reset_Handler)
SEARCH_DIR(.)
GROUP(-lgcc -lc -lcs3 -lcs3unhosted) 

MEMORY
{
	CODE (rx) : ORIGIN = 0x00000000, LENGTH = 128K
	DATA (rwx) : ORIGIN = 0x20000000, LENGTH = 16K
}

SECTIONS
{
	.text :
	{
		. = ALIGN(4);
		KEEP(*(.isr_vector))            /* Startup code */
		. = ALIGN(4);
		*(.text)						/* remaining code */
		*(.text.*)						/* remaining code */
		*(.rodata)						/* read-only data (constants) */
		*(.rodata*)
		*(.glue_7)
		*(.glue_7t)

		/* section information for finsh shell */
		. = ALIGN(4);
		__fsymtab_start = .;
		KEEP(*(FSymTab))
		__fsymtab_end = .;
		. = ALIGN(4);
		__vsymtab_start = .;
		KEEP(*(VSymTab))
		__vsymtab_end = .;
		. = ALIGN(4);

		. = ALIGN(4);
		_etext = .;

		/* This is used by the startup in order to initialize the .data secion */
		_sidata = _etext;
	} > CODE = 0

	/* .data section which is used for initialized data */

	.data : AT (_sidata)
	{
		. = ALIGN(4);
		/* This is used by the startup in order to initialize the .data secion */
		_sdata = . ;

		*(.data)
		*(.data.*)

		. = ALIGN(4);
		/* This is used by the startup in order to initialize the .data secion */
		_edata = . ;
	} >DATA

	__bss_start = .;
	.bss :
	{
		. = ALIGN(4);
		/* This is used by the startup in order to initialize the .bss secion */
		_sbss = .;

		*(.bss)
		*(COMMON)

		. = ALIGN(4);
		/* This is used by the startup in order to initialize the .bss secion */
		_ebss = . ;
		_estack = .;
		
		*(.bss.init)
	} > DATA
	__bss_end = .;
	_end = .;

	/* Stabs debugging sections.  */
	.stab          0 : { *(.stab) }
	.stabstr       0 : { *(.stabstr) }
	.stab.excl     0 : { *(.stab.excl) }
	.stab.exclstr  0 : { *(.stab.exclstr) }
	.stab.index    0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment       0 : { *(.comment) }
	
	/* DWARF debug sections.
	* Symbols in the DWARF debugging sections are relative to
	* the beginning of the section so we begin them at 0.
	*/
	/* DWARF 1 */
	.debug          0 : { *(.debug) }
	.line           0 : { *(.line) }
	/* GNU DWARF 1 extensions */
	.debug_srcinfo  0 : { *(.debug_srcinfo) }
	.debug_sfnames  0 : { *(.debug_sfnames) }
	/* DWARF 1.1 and DWARF 2 */
	.debug_aranges  0 : { *(.debug_aranges) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	/* DWARF 2 */
	.debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev   0 : { *(.debug_abbrev) }
	.debug_line     0 : { *(.debug_line) }
	.debug_frame    0 : { *(.debug_frame) }
	.debug_str      0 : { *(.debug_str) }
	.debug_loc      0 : { *(.debug_loc) }
	.debug_macinfo  0 : { *(.debug_macinfo) }
	/* DWARF 2.1 */
	.debug_ranges   0 : { *(.debug_ranges) }
	/* SGI/MIPS DWARF 2 extensions */
	.debug_weaknames 0 : { *(.debug_weaknames) }
	.debug_funcnames 0 : { *(.debug_funcnames) }
	.debug_typenames 0 : { *(.debug_typenames) }
	.debug_varnames  0 : { *(.debug_varnames) }

	.note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
	.ARM.attributes 0 : { KEEP (*(.ARM.attributes)) }
	/DISCARD/ : { *(.note.GNU-stack) }
}

