library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity BRANCH is
    port (
        condition, program_counter : in signed(31 downto 0);
        cond_func : in signed(2 downto 0);
        offset : in signed(18 downto 0);
        output_address : out signed(31 downto 0)
    );
end entity BRANCH;

architecture behavior of BRANCH is
constant ZERO : signed(31 downto 0) := (others => '0');
begin
    branch_P: process (condition, cond_func, offset) is
    begin
        case cond_func is
            when "100" => 
            if condition = ZERO then 
                output_address <= program_counter + offset;
            else
                output_address <= program_counter + 1;
            end if;
            when "010" =>
            if condition < ZERO then
                output_address <= program_counter + offset;
            else
                output_address <= program_counter + 1;
            end if;
            when "001" =>
            if condition > ZERO then
                output_address <= program_counter + offset;
            else
                output_address <= program_counter + 1;
            end if;
            when others => output_address <= program_counter + 1;
        end case;
    end process branch_P;
end architecture behavior;
