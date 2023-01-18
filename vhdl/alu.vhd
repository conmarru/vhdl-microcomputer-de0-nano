library ieee;
library work;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.instructions.all;

entity alu is
    port (
    --clk : in std_logic;    
    A, B: in signed(31 downto 0);
    opcode : in signed(4 downto 0);
    func : in signed(4 downto 0);
    C: out signed(31 downto 0) 
    );
end entity alu ;

architecture behave of alu is

    constant func_add : signed(4 downto 0) := "00000";

    constant func_and : signed(4 downto 0) := "00011";
    constant func_or  : signed(4 downto 0) := "00100";
    constant func_not : signed(4 downto 0) := "00101";

    constant func_shr : signed(4 downto 0) := "00001";
    constant func_shl : signed(4 downto 0) := "00010";
    constant func_sr8 : signed(4 downto 0) := "00111";
    constant func_sl8 : signed(4 downto 0) := "01000";
    -- constant func_mul : signed(4 downto 0) := "00110"; zu anspruchsvoll / macht Probleme


begin
    proc: process (A, B, func, opcode) is
    begin
        --if rising_edge(clk) then
        case opcode
        
        case func is
        when  func_and    => C <= A and B;
        when  func_or     => C <= A or B;
        when  func_not    => C <= not A;
        when  func_add    => C <= A + B;
        --when  func_mul    => C <= A(15 downto 0) * B(15 downto 0); zu anspruchsvoll / macht Probleme
        when func_shr     => C <= A srl 1;
        when func_shl     => C <= A sll 1;
        when func_sr8     => C <= "00000000"& A(31 downto 8); --shr Byte;
        when func_sl8     => C <= A(23 downto 0) &"00000000"; --shl Byte;

        when others     => C <= (others=>'-');

        end case;
        end case;
        --end if;
    end process proc;
end architecture behave;