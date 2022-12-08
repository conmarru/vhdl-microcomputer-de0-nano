library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity alu is
    port (A, B: in signed(31 downto 0);
    opcode : in signed(4 downto 0);
    C: out signed(31 downto 0) );
end entity alu ;

architecture behave of alu is

    constant op_add : signed(4 downto 0) := "00000";

    constant op_and : signed(4 downto 0) := "00011";
    constant op_or  : signed(4 downto 0) := "00100";
    constant op_not : signed(4 downto 0) := "00101";

    constant op_shr : signed(4 downto 0) := "00001";
    constant op_shl : signed(4 downto 0) := "00010";
    constant op_sr8 : signed(4 downto 0) := "00111";
    constant op_sl8 : signed(4 downto 0) := "01000";
    -- constant op_mul : signed(4 downto 0) := "00110"; zu anspruchsvoll / macht Probleme


begin
    proc: process (A, B, opcode) is
    begin
        case opcode is
        when  op_and    => C <= A and B;
        when  op_or     => C <= A or B;
        when  op_not    => C <= not A;
        when  op_add    => C <= A + B;
        --when  op_mul    => C <= A(15 downto 0) * B(15 downto 0); zu anspruchsvoll / macht Probleme
        when op_shr     => C <= A srl 1;
        when op_shl     => C <= A sll 1;
        when op_sr8     => C <= "00000000"& A(31 downto 8); --shr Byte;
        when op_sl8     => C <= A(23 downto 0) &"00000000"; --shl Byte;

        when others     => C <= (others=>'-');

        end case;
    end process proc;
end architecture behave;