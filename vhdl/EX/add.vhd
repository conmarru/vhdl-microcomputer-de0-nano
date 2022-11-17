library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity ADD is
    port (
        a, b : in signed(31 downto 0);
        result : out signed(31 downto 0); 
    );
end entity ADD;

architecture behavior of ADD is
begin
    add_P: process (a, b) is
    begin
        result <= a + b;
    end process add_P;
end architecture behavior;
