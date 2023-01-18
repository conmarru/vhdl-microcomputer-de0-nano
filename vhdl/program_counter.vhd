library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


entity program_counter is
    Port (
        -- control
        reset : in std_logic;
        clk : in std_logic;
        -- in
        used_jmp : in std_logic;
        used_branch : in std_logic;
        halt : in std_logic;
        next_pc_in : in signed(31 downto 0);
        -- out
        pc : out signed(31 downto 0);
        next_pc_out : out signed(31 downto 0)
    );
end program_counter;

architecture behavior of program_counter is
    signal jmp_counter : unsigned(1 downto 0);
    signal pc_temp : signed(31 downto 0);
    begin
        process (clk, reset, halt) is 
        begin
        if halt = '1' then
            -- todo figure out what to do here
        end if;
        if reset = '1' then
            pc <= (others => '0');
            next_pc_out <= to_signed(4, 32);
            jmp_counter <= to_unsigned(0, 2);
        end if;
        if rising_edge(clk) then
          if used_jmp = '1' or used_branch = '1' then
            jmp_counter <= to_unsigned(3, 2);
            pc <= next_pc_in;
            pc_temp <= next_pc_in + to_signed(1, 32);
            next_pc_out <= next_pc_in + to_signed(4, 32);
          elsif jmp_counter > to_unsigned(0, 2) then
            jmp_counter <= jmp_counter - 1;
            pc <= pc_temp;
            next_pc_out <= pc_temp + to_signed(4, 32);
            pc_temp <= pc_temp + to_signed(1, 32);
          else
            pc <= next_pc_in;
            next_pc_out <= next_pc_in + to_signed(1, 32);
          end if;
        end if;
      end process;
    end behavior;
