/* Simple linker script for the JOS kernel.
   See the GNU ld 'info' manual ("info ld") to learn the syntax. */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{
	/* Link the kernel at this address: "." means the current address */
        /* Must be equal to KERNLINK */
	. = 0x80100000;

	.text : AT(0x100000) {
		*(.text .stub .text.* .gnu.linkonce.t.*)
	}

	PROVIDE(etext = .);	/* Define the 'etext' symbol to this value */

	.rodata : {
		*(.rodata .rodata.* .gnu.linkonce.r.*)
	}

	. = ALIGN(0x1000);
	PROVIDE(sprof = .);
	.prof : {
		*(.prof)
	}
	PROVIDE(eprof = .);

        .init_array : {
                PROVIDE_HIDDEN(__init_array_start = .);
                KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
                KEEP(*(.init_array))
                KEEP(*(.ctors));
                PROVIDE_HIDDEN(__init_array_end = .);
        }

	.percpuinit_array : {
		PROVIDE_HIDDEN(__percpuinit_array_start = .);
		KEEP(*(.percpuinit_array))
		PROVIDE_HIDDEN(__percpuinit_array_end = .);
	}

	/* Include debugging information in kernel memory */
	.stab : {
		PROVIDE(__STAB_BEGIN__ = .);
		*(.stab);
		PROVIDE(__STAB_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}

	.stabstr : {
		PROVIDE(__STABSTR_BEGIN__ = .);
		*(.stabstr);
		PROVIDE(__STABSTR_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}

	/* Adjust the address for the data segment to the next page */
	. = ALIGN(0x1000);

	/* Conventionally, Unix linkers provide pseudo-symbols
	 * etext, edata, and end, at the end of the text, data, and bss.
	 * For the kernel mapping, we need the address at the beginning
	 * of the data section, but that's not one of the conventional
	 * symbols, because the convention started before there was a
	 * read-only rodata section between text and data. */
	PROVIDE(data = .);

	/* The data segment */
	.data : {
		*(.data .data.* .gnu.linkonce.d.*)
        *(.got) *(.igot)
        *(.got.plt)  *(.igot.plt)
        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.eh_frame_end)
        *(.gcc_except_table .gcc_except_table.*)
	}

	PROVIDE(edata = .);

	.bss : {
		*(.bss)
	}

	PROVIDE(end = .);

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack)
	}

	/*wrapping symbols: the generated symbol changes with the directory.*/
	PROVIDE(_binary_entryother_start = _binary___build_kern_entryother_start);
	PROVIDE(_binary_entryother_size = _binary___build_kern_entryother_size);
	PROVIDE(_binary_initcode_start = _binary___build_kern_initcode_start);
	PROVIDE(_binary_initcode_size = _binary___build_kern_initcode_size);
}
