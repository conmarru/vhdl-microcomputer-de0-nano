LIBRARY ieee;
LIBRARY work;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
USE work.instructions.ALL;

ENTITY alu IS
    PORT (
        --clk : in std_logic;    
        A, B : IN signed(31 DOWNTO 0);
        opcode : IN signed(4 DOWNTO 0);
        func : IN signed(4 DOWNTO 0);
        C : OUT signed(31 DOWNTO 0)
    );
END ENTITY alu;

ARCHITECTURE behave OF alu IS

    CONSTANT func_add : signed(4 DOWNTO 0) := "00000";

    CONSTANT func_and : signed(4 DOWNTO 0) := "00011";
    CONSTANT func_or : signed(4 DOWNTO 0) := "00100";
    CONSTANT func_not : signed(4 DOWNTO 0) := "00101";

    CONSTANT func_shr : signed(4 DOWNTO 0) := "00001";
    CONSTANT func_shl : signed(4 DOWNTO 0) := "00010";
    CONSTANT func_sr8 : signed(4 DOWNTO 0) := "00111";
    CONSTANT func_sl8 : signed(4 DOWNTO 0) := "01000";
    -- constant func_mul : signed(4 downto 0) := "00110"; zu anspruchsvoll / macht Probleme
BEGIN
    proc : PROCESS (A, B, func, opcode) IS
    BEGIN
        --if rising_edge(clk) then

        IF opcode = BRANCH OR opcode = ADDI THEN
            C <= A + B;
        ELSIF opcode = MOVE OR opcode = LOAD OR opcode = STORE THEN
            C <= A;
        ELSIF opcode = LOADI THEN
            C <= B;
        ELSE
            CASE func IS
                WHEN func_and => C <= A AND B;
                WHEN func_or => C <= A OR B;
                WHEN func_not => C <= NOT A;
                WHEN func_add => C <= A + B;
                WHEN func_shr => C <= A SRL 1;
                WHEN func_shl => C <= A SLL 1;
                WHEN func_sr8 => C <= "00000000" & A(31 DOWNTO 8); --shr Byte;
                WHEN func_sl8 => C <= A(23 DOWNTO 0) & "00000000"; --shl Byte;
                WHEN OTHERS => C <= (OTHERS => '-');
            END CASE;
        END IF;
    END PROCESS proc;
END ARCHITECTURE behave;