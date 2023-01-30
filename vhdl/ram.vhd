LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;
USE STD.TEXTIO.ALL;

-- yoinked and adapted from lothar-miller.de
ENTITY RAM IS
  PORT (
    clk : IN STD_LOGIC;
    reset : IN STD_LOGIC;
    Addr : IN signed (31 DOWNTO 0);
    write_in : IN STD_LOGIC;
    Din : IN signed (31 DOWNTO 0);
    Dout : OUT signed (31 DOWNTO 0)
  );
END RAM;

ARCHITECTURE BlockRAM OF RAM IS
  TYPE speicher IS ARRAY(0 TO 16368) OF signed(31 DOWNTO 0);
  SIGNAL memory : speicher;

  FILE data_in : TEXT;
BEGIN
  behav : PROCESS (clk, Addr, reset) IS
    VARIABLE cur_read : line;
    VARIABLE int : INTEGER;
    VARIABLE x : INTEGER := 0;
    VARIABLE good : BOOLEAN;
  BEGIN
    IF reset = '1' THEN
      file_open(data_in, "instructions.bin", read_mode);
      WHILE NOT endfile (data_in) LOOP
        readline(data_in, cur_read);
        read(cur_read, int);
        REPORT INTEGER'image(int);
        memory(x) <= to_signed(int, 32);
        x := x + 1;
      END LOOP;
      file_close(data_in);
    END IF;
    REPORT to_string(Addr);
    IF rising_edge(clk) THEN
      IF (write_in = '1') THEN
        REPORT "Trying write";
        IF to_integer(Addr) < 16368 THEN
          memory(to_integer(Addr)) <= Din;
        ELSE
          REPORT "Out of bounds memory read";
        END IF;
      END IF;
    END IF;
    IF to_integer(Addr) < 16368 AND to_integer(Addr) >= 0 THEN
      Dout <= memory(to_integer(Addr));
    ELSE
      REPORT "Out of bounds memory read";
    END IF;
  END PROCESS;
END BlockRAM;