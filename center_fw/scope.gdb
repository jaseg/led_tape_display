target remote localhost:3334
set pagination off
file main.elf
load

break gdb_dump
command 1
    dump binary value /tmp/scope_dump.bin adc_buf
    continue
end

continue
