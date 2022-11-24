library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity aluTest is
end entity aluTest;


architecture testbench of aluTest is

    component alu is
        port (
            A, B: in signed(31 downto 0);
            opcode : in signed(4 downto 0);
            C: out signed(31 downto 0) );

    end component alu ;
    signal A, B: signed(31 downto 0);
    signal opcode : signed(4 downto 0);
    signal C: signed(31 downto 0);

    constant op_add : signed(4 downto 0) := "00000";

    constant op_and : signed(4 downto 0) := "00011";
    constant op_or  : signed(4 downto 0) := "00100";
    constant op_not : signed(4 downto 0) := "00101";

    constant op_shr : signed(4 downto 0) := "00001";
    constant op_shl : signed(4 downto 0) := "00010";

begin
    dlI: Alu
    port map(A, B, opcode, C);

    stiP: process is
    begin
        A <= (others => '0');
        B <= (others => '1');
        opcode <= op_add;
        wait for 20 ms;

        A <= to_signed(8, 32);
        B <= to_signed(7, 32);
        opcode <= op_add;
        wait for 20 ms;

        A <= to_signed(1, 32);
        B <= to_signed(0, 32);
        opcode <= op_and;
        wait for 20 ms;

        A <= to_signed(1, 32);
        B <= to_signed(1, 32);
        opcode <= op_and;
        wait for 20 ms;

        A <= to_signed(1, 32);
        B <= to_signed(0, 32);
        opcode <= op_or;
        wait for 20 ms;

        A <= to_signed(0, 32);
        B <= to_signed(0, 32);
        opcode <= op_or;
        wait for 20 ms;

        A <= to_signed(1, 32);
        opcode <= op_not;
        wait for 20 ms;
        
        A <= (others => '1');
        opcode <= op_shl;
        wait for 20 ms;

        A <= (others => '1');
        opcode <= op_shr;
        wait for 20 ms;
        wait;

    end process stiP;
end architecture testbench;
