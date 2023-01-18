library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity program_counter_test is
end entity program_counter_test;


architecture testbench of program_counter_test is

    component program_counter is
    port (
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
    end component program_counter;

        signal reset :  std_logic;
        signal clk : std_logic;
        signal used_jmp : std_logic;
        signal used_branch : std_logic;
        signal halt : std_logic;
        signal next_pc_in : signed(31 downto 0);
        signal pc : signed(31 downto 0);
        signal next_pc_out : signed(31 downto 0);

begin
  dlI: entity work.program_counter
    port map(reset, clk, used_jmp, used_branch, halt, next_pc_in, pc, next_pc_out);

    stiP: process is
    begin
        wait for 20 ms;
        clk <= '0';
        used_branch <= '0';
        used_jmp <= '0';
        reset <= '1';
        wait for 20 ms;
        reset <= '0';
        wait for 20 ms;
        next_pc_in <= to_signed(1, 32);
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 20 ms;
        next_pc_in <= to_signed(2, 32);
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 20 ms;
        next_pc_in <= to_signed(30, 32);
        used_jmp <= '1';
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 20 ms;
        next_pc_in <= to_signed(3, 32);
        used_jmp <= '0';
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 20 ms;
        next_pc_in <= to_signed(4, 32);
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 20 ms;
        next_pc_in <= to_signed(5, 32);
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 20 ms;
        next_pc_in <= to_signed(34, 32);
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 20 ms;
        wait;
    end process stiP;
end architecture testbench;
