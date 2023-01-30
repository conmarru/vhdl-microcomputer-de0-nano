LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;
ENTITY reg_bank IS
  PORT (
    clk : IN STD_LOGIC;
    addr_a, addr_b, addr_dest : IN signed (4 DOWNTO 0);
    addr_write : IN signed(4 DOWNTO 0);
    write : IN STD_LOGIC;
    val_write : IN signed (31 DOWNTO 0);
    val_a, val_b, val_res : OUT signed (31 DOWNTO 0)
  );
END reg_bank;

ARCHITECTURE behavior OF reg_bank IS
  TYPE bankArr IS ARRAY(0 TO 31) OF signed(31 DOWNTO 0);
  SIGNAL bank : bankArr;
BEGIN
  val_a <= bank(to_integer(unsigned(addr_a)));
  val_b <= bank(to_integer(unsigned(addr_b)));
  val_res <= bank(to_integer(unsigned(addr_dest)));
  PROCESS (clk) IS
  BEGIN
    IF rising_edge(clk) THEN
      IF (write = '1') THEN
        bank(to_integer(unsigned(addr_write))) <= val_write;
      END IF;
    END IF;
  END PROCESS;
END behavior;