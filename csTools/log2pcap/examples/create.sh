#!/bin/sh

/opt/can-utils-2023.03/bin/cansend vcan0 123#
/opt/can-utils-2023.03/bin/cansend vcan0 123#0123456789abcdef
/opt/can-utils-2023.03/bin/cansend vcan0 123#0123456789abcdef_f

/opt/can-utils-2023.03/bin/cansend vcan0 00000123#
/opt/can-utils-2023.03/bin/cansend vcan0 00000123#0123456789abcdef
/opt/can-utils-2023.03/bin/cansend vcan0 00000123#0123456789abcdef_f

/opt/can-utils-2023.03/bin/cansend vcan0 123#R
/opt/can-utils-2023.03/bin/cansend vcan0 123#R8
/opt/can-utils-2023.03/bin/cansend vcan0 123#R8_f

/opt/can-utils-2023.03/bin/cansend vcan0 00000123#R
/opt/can-utils-2023.03/bin/cansend vcan0 00000123#R8
/opt/can-utils-2023.03/bin/cansend vcan0 00000123#R8_f

/opt/can-utils-2023.03/bin/cansend vcan0 123##0
/opt/can-utils-2023.03/bin/cansend vcan0 123##7
/opt/can-utils-2023.03/bin/cansend vcan0 123##0101112131415161728292a2b2c2d2e2f
/opt/can-utils-2023.03/bin/cansend vcan0 123##7101112131415161728292a2b2c2d2e2f

/opt/can-utils-2023.03/bin/cansend vcan0 00000123##0
/opt/can-utils-2023.03/bin/cansend vcan0 00000123##7
/opt/can-utils-2023.03/bin/cansend vcan0 00000123##0101112131415161728292a2b2c2d2e2f
/opt/can-utils-2023.03/bin/cansend vcan0 00000123##7101112131415161728292a2b2c2d2e2f
