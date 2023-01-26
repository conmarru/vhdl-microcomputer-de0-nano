library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity BRANCH is
    port (
        condition : in signed(31 downto 0);
        cond_func : in signed(4 downto 0);
        do_branch : out std_logic
    );
end entity BRANCH;

architecture behavior of BRANCH is
constant ZERO : signed(31 downto 0) := (others => '0');
begin
    branch_P: process (condition, cond_func) is
    begin
        case cond_func is
            when "00100" => 
            if condition = ZERO then 
            do_branch <= '1';
            else
            do_branch <= '0';
            end if;
            when "00010" =>
            if condition < ZERO then
                do_branch <= '1';
            else
                do_branch <= '0';
            end if;
            when "00001" =>
            if condition > ZERO then
                do_branch <= '1';
            else
                do_branch <= '0';
            end if;
            when others => do_branch <= '0';
        end case;
    end process branch_P;
end architecture behavior;
