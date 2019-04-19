LED-tape based segment display
==============================

This is a work-in-progress repository containing hard- and software source files for an 8-segment LED display based on
cheap single-color LED tape. The display consists of one digit driver per display digit that receive power and
transceive data through a two-wire powerline bus. This bus can support 10-50 digits depending on the size of the display
(and thus current consumption). The bus is driven by a driver board that is in turn controlled through RS-485.

The hardware is mostly finalized and can be found in ``center`` (digit controller) and ``driver`` (bus driver). The
firmware is still WIP and can be found in ``center_fw`` (digit controller), ``driver_fw`` (bus driver) and
``common`` (common 8b10b implementation).
