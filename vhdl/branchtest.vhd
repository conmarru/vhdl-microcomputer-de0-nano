library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity branchtest is
end entity branchtest;
architecture testbench of branchtest is

    component branch is
        port (
            condition : in signed(31 downto 0);
            cond_func : in signed(2 downto 0);
            do_branch : out std_logic
        );
    end component branch;

    signal condition : signed(31 downto 0);
    signal cond_func : signed(2 downto 0);
    signal do_branch : std_logic;

begin
    dlI : entity work.branch
        port map(condition, cond_func, do_branch);

    stiP : process is
    begin
        condition <= to_signed(0, 32);
        cond_func <= "100";
        wait for 20 ms;
        condition <= to_signed(1, 32);
        cond_func <= "100";
        wait for 20 ms;
        wait;
    end process stiP;
end architecture testbench;
