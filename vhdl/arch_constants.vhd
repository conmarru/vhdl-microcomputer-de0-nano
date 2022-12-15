library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

package instructions is
    constant NOP : signed(4 downto 0) := "00000";
    constant HALT : signed(4 downto 0) := "00001";

    constant MOVE : signed(4 downto 0) := "00010";
    constant LOADI : signed(4 downto 0) := "00011";
    constant ADD : signed(4 downto 0) := "00100";
    constant ADDI : signed(4 downto 0) := "00101";

    constant LOGIC : signed(4 downto 0) := "00100";

    constant JMP : signed(4 downto 0) := "00110";
    constant BRANCH : signed(4 downto 0) := "00111";

    constant LOAD : signed(4 downto 0) := "01000";
    constant STORE : signed(4 downto 0) := "01001";
end package instructions;