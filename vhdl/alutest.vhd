LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY aluTest IS
END ENTITY aluTest;
ARCHITECTURE testbench OF aluTest IS

    COMPONENT alu IS
        PORT (
            A, B : IN signed(31 DOWNTO 0);
            opcode : IN signed(4 DOWNTO 0);
            C : OUT signed(31 DOWNTO 0));

    END COMPONENT alu;
    SIGNAL A, B : signed(31 DOWNTO 0);
    SIGNAL opcode : signed(4 DOWNTO 0);
    SIGNAL C : signed(31 DOWNTO 0);

    CONSTANT op_add : signed(4 DOWNTO 0) := "00000";
    CONSTANT op_mul : signed(4 DOWNTO 0) := "00110";

    CONSTANT op_and : signed(4 DOWNTO 0) := "00011";
    CONSTANT op_or : signed(4 DOWNTO 0) := "00100";
    CONSTANT op_not : signed(4 DOWNTO 0) := "00101";

    CONSTANT op_shr : signed(4 DOWNTO 0) := "00001";
    CONSTANT op_shl : signed(4 DOWNTO 0) := "00010";
    CONSTANT op_sr8 : signed(4 DOWNTO 0) := "00111";
    CONSTANT op_sl8 : signed(4 DOWNTO 0) := "01000";

BEGIN
    dlI : ENTITY work.alu
        PORT MAP(A, B, opcode, C);

    stiP : PROCESS IS
    BEGIN
        A <= (0 => '1', OTHERS => '0');
        B <= (OTHERS => '1');
        opcode <= op_add;
        WAIT FOR 20 ms;

        A <= to_signed(0, 32);
        B <= to_signed(0, 32);
        opcode <= op_and;
        WAIT FOR 20 ms;

        A <= to_signed(0, 32);
        B <= to_signed(1, 32);
        opcode <= op_and;
        WAIT FOR 20 ms;

        A <= to_signed(1, 32);
        B <= to_signed(0, 32);
        opcode <= op_and;
        WAIT FOR 20 ms;

        A <= to_signed(1, 32);
        B <= to_signed(1, 32);
        opcode <= op_and;
        WAIT FOR 20 ms;

        A <= to_signed(1, 32);
        B <= to_signed(0, 32);
        opcode <= op_or;
        WAIT FOR 20 ms;

        A <= to_signed(0, 32);
        B <= to_signed(0, 32);
        opcode <= op_or;
        WAIT FOR 20 ms;

        A <= to_signed(1, 32);
        B <= to_signed(1, 32);
        opcode <= op_or;
        WAIT FOR 20 ms;

        A <= to_signed(255, 32);
        opcode <= op_not;
        WAIT FOR 20 ms;

        A <= to_signed(255, 32);
        opcode <= op_not;
        WAIT FOR 20 ms;

        A <= (OTHERS => '1');
        opcode <= op_not;
        WAIT FOR 20 ms;

        A <= (OTHERS => '0');
        opcode <= op_not;
        WAIT FOR 20 ms;

        A <= (OTHERS => '1');
        opcode <= op_shl;
        WAIT FOR 20 ms;

        A <= (OTHERS => '1');
        opcode <= op_shr;
        WAIT FOR 20 ms;

        -- A <= to_signed(2, 32);
        -- B <= to_signed(2**15, 32);
        -- opcode <= op_mul;
        -- wait for 20 ms;

        -- A <= to_signed(2, 32);
        -- B <= to_signed(8, 32);
        -- opcode <= op_mul;
        -- wait for 20 ms;

        A <= (OTHERS => '1');
        opcode <= op_sr8;
        WAIT FOR 20 ms;

        A <= (OTHERS => '1');
        opcode <= op_sl8;
        WAIT FOR 20 ms;
        WAIT;

    END PROCESS stiP;
END ARCHITECTURE testbench;