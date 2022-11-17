library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity BRANCH is
    port (
        condition : in signed(4 downto 0);
        cond_func : in signed(2 downto 0);
        offset : in signed(18 downto 0)
        output_address : out signed(31 downto 0); 
    );
end entity BRANCH;

architecture behavior of BRANCH is
begin
    branch_P: process (condition, cond_func) is
    begin
        case cond_func is
            when "" => output_address <= offset + ...;
        end case;
    end process branch_P;
end architecture behavior;
