LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;
ENTITY program_counter IS
  PORT (
    -- control
    reset : IN STD_LOGIC;
    clk : IN STD_LOGIC;
    -- in
    used_jmp : IN STD_LOGIC;
    used_branch : IN STD_LOGIC;
    next_pc_in : IN signed(31 DOWNTO 0);
    -- out
    pc : OUT signed(31 DOWNTO 0);
    next_pc_out : OUT signed(31 DOWNTO 0)
  );
END program_counter;

ARCHITECTURE behavior OF program_counter IS
  SIGNAL jmp_counter : unsigned(2 DOWNTO 0);
  SIGNAL pc_temp : signed(31 DOWNTO 0);
BEGIN
  PROCESS (clk, reset) IS
  BEGIN
    IF reset = '1' THEN
      pc <= (OTHERS => '0');
      next_pc_out <= to_signed(4, 32);
      jmp_counter <= to_unsigned(3, 3);
      pc_temp <= to_signed(1, 32);
    END IF;
    IF rising_edge(clk) THEN
      IF used_jmp = '1' OR used_branch = '1' THEN
        jmp_counter <= to_unsigned(3, 3);
        pc <= next_pc_in;
        pc_temp <= next_pc_in + to_signed(1, 32);
        next_pc_out <= next_pc_in + to_signed(4, 32);
      ELSIF jmp_counter > to_unsigned(0, 3) THEN
        jmp_counter <= jmp_counter - 1;
        pc <= pc_temp;
        next_pc_out <= pc_temp + to_signed(4, 32);
        pc_temp <= pc_temp + to_signed(1, 32);
      ELSE
        pc <= next_pc_in;
        next_pc_out <= next_pc_in + to_signed(4, 32);
      END IF;
    END IF;
  END PROCESS;
END behavior;