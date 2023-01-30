LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY program_counter_test IS
END ENTITY program_counter_test;
ARCHITECTURE testbench OF program_counter_test IS

    COMPONENT program_counter IS
        PORT (
            -- control
            reset : IN STD_LOGIC;
            clk : IN STD_LOGIC;
            -- in
            used_jmp : IN STD_LOGIC;
            used_branch : IN STD_LOGIC;
            halt : IN STD_LOGIC;
            next_pc_in : IN signed(31 DOWNTO 0);
            -- out
            pc : OUT signed(31 DOWNTO 0);
            next_pc_out : OUT signed(31 DOWNTO 0)
        );
    END COMPONENT program_counter;

    SIGNAL reset : STD_LOGIC;
    SIGNAL clk : STD_LOGIC;
    SIGNAL used_jmp : STD_LOGIC;
    SIGNAL used_branch : STD_LOGIC;
    SIGNAL halt : STD_LOGIC;
    SIGNAL next_pc_in : signed(31 DOWNTO 0);
    SIGNAL pc : signed(31 DOWNTO 0);
    SIGNAL next_pc_out : signed(31 DOWNTO 0);

BEGIN
    dlI : ENTITY work.program_counter
        PORT MAP(reset, clk, used_jmp, used_branch, halt, next_pc_in, pc, next_pc_out);

    stiP : PROCESS IS
    BEGIN
        WAIT FOR 20 ms;
        clk <= '0';
        used_branch <= '0';
        used_jmp <= '0';
        reset <= '1';
        WAIT FOR 20 ms;
        reset <= '0';
        WAIT FOR 20 ms;
        next_pc_in <= to_signed(1, 32);
        clk <= '1';
        WAIT FOR 1 ms;
        clk <= '0';
        WAIT FOR 20 ms;
        next_pc_in <= to_signed(2, 32);
        clk <= '1';
        WAIT FOR 1 ms;
        clk <= '0';
        WAIT FOR 20 ms;
        next_pc_in <= to_signed(30, 32);
        used_jmp <= '1';
        clk <= '1';
        WAIT FOR 1 ms;
        clk <= '0';
        WAIT FOR 20 ms;
        next_pc_in <= to_signed(3, 32);
        used_jmp <= '0';
        clk <= '1';
        WAIT FOR 1 ms;
        clk <= '0';
        WAIT FOR 20 ms;
        next_pc_in <= to_signed(4, 32);
        clk <= '1';
        WAIT FOR 1 ms;
        clk <= '0';
        WAIT FOR 20 ms;
        next_pc_in <= to_signed(5, 32);
        clk <= '1';
        WAIT FOR 1 ms;
        clk <= '0';
        WAIT FOR 20 ms;
        next_pc_in <= to_signed(34, 32);
        clk <= '1';
        WAIT FOR 1 ms;
        clk <= '0';
        WAIT FOR 20 ms;
        WAIT;
    END PROCESS stiP;
END ARCHITECTURE testbench;