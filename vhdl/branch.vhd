LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
USE work.instructions.ALL;

ENTITY BRANCH IS
    PORT (
        condition : IN signed(31 DOWNTO 0);
        opcode : IN signed(4 DOWNTO 0);
        cond_func : IN signed(4 DOWNTO 0);
        do_branch : OUT STD_LOGIC
    );
END ENTITY BRANCH;

ARCHITECTURE behavior OF BRANCH IS
    CONSTANT ZERO : signed(31 DOWNTO 0) := (OTHERS => '0');
BEGIN
    branch_P : PROCESS (condition, cond_func, opcode) IS
    BEGIN
        IF opcode = work.instructions.BRANCH THEN
            CASE cond_func IS
                WHEN "00100" =>
                    IF condition = ZERO THEN
                        do_branch <= '1';
                    ELSE
                        do_branch <= '0';
                    END IF;
                WHEN "00010" =>
                    IF condition < ZERO THEN
                        do_branch <= '1';
                    ELSE
                        do_branch <= '0';
                    END IF;
                WHEN "00001" =>
                    IF condition > ZERO THEN
                        do_branch <= '1';
                    ELSE
                        do_branch <= '0';
                    END IF;
                WHEN OTHERS => do_branch <= '0';
            END CASE;
        ELSE
            do_branch <= '0';
        END IF;
    END PROCESS branch_P;
END ARCHITECTURE behavior;