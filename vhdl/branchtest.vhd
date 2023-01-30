LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY branchtest IS
END ENTITY branchtest;
ARCHITECTURE testbench OF branchtest IS

    COMPONENT branch IS
        PORT (
            condition : IN signed(31 DOWNTO 0);
            cond_func : IN signed(2 DOWNTO 0);
            do_branch : OUT STD_LOGIC
        );
    END COMPONENT branch;

    SIGNAL condition : signed(31 DOWNTO 0);
    SIGNAL cond_func : signed(2 DOWNTO 0);
    SIGNAL do_branch : STD_LOGIC;

BEGIN
    dlI : ENTITY work.branch
        PORT MAP(condition, cond_func, do_branch);

    stiP : PROCESS IS
    BEGIN
        condition <= to_signed(0, 32);
        cond_func <= "100";
        WAIT FOR 20 ms;
        condition <= to_signed(1, 32);
        cond_func <= "100";
        WAIT FOR 20 ms;
        WAIT;
    END PROCESS stiP;
END ARCHITECTURE testbench;