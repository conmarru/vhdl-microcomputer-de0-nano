library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
entity JMP is
    port (
        output_address : out signed(31 downto 0); 
        input_address : in signed(31 downto 0)
    );
end entity JMP;

architecture behavior of JMP is
begin
    jmp_p: process(input_address) is
    begin 
            output_address <= input_address;
    end process jmp_p;
end architecture behavior;
