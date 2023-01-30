LIBRARY IEEE;
LIBRARY work;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;
USE work.instructions.ALL;

ENTITY pre_fetch_reg IS
    PORT (
        clk : IN STD_LOGIC;
        pc_in, next_pc_in, instruction : IN signed(31 DOWNTO 0);
        reset : IN STD_LOGIC;
        pc_out, next_pc_out : OUT signed(31 DOWNTO 0);
        opcode_out, addr_a_out, addr_b_out, addr_dest_out, func_out : OUT signed(4 DOWNTO 0);
        imm_out : OUT signed(31 DOWNTO 0);
        use_imm_out : OUT STD_LOGIC;
        use_jmp_out : OUT STD_LOGIC;
        reg_write_out : OUT STD_LOGIC;
        memory_read_out, memory_write_out : OUT STD_LOGIC;
        halt_out : OUT STD_LOGIC
    );
END ENTITY pre_fetch_reg;

ARCHITECTURE behavior OF pre_fetch_reg IS
BEGIN
    pre_fetch_P : PROCESS (clk, reset) IS
    BEGIN
        IF reset = '1' THEN
            pc_out <= (OTHERS => '0');
            next_pc_out <= to_signed(3, 32);
            addr_a_out <= (OTHERS => '0');
            addr_b_out <= (OTHERS => '0');
            opcode_out <= (OTHERS => '0');
            addr_dest_out <= (OTHERS => '0');
            func_out <= (OTHERS => '0');
            imm_out <= (OTHERS => '0');
            use_imm_out <= '0';
            use_jmp_out <= '0';
            reg_write_out <= '0';
            memory_read_out <= '0';
            memory_write_out <= '0';
            halt_out <= '0';
        END IF;

        IF rising_edge(clk) THEN
            pc_out <= pc_in;
            next_pc_out <= next_pc_in;
            opcode_out <= instruction(31 DOWNTO 27);
            addr_dest_out <= instruction(26 DOWNTO 22);
            addr_a_out <= instruction(21 DOWNTO 17);
            addr_b_out <= instruction(16 DOWNTO 12);
            func_out <= instruction(4 DOWNTO 0);
            -- Instruction format is slightly different when branching
            IF instruction(31 DOWNTO 27) = BRANCH THEN
                imm_out <= (31 DOWNTO 16 => instruction(21)) & instruction(20 DOWNTO 5);
            ELSE
                imm_out <= (31 DOWNTO 16 => instruction(16)) & instruction(15 DOWNTO 0);
            END IF;
            IF instruction(31 DOWNTO 27) = BRANCH OR instruction(31 DOWNTO 27) = LOADI OR instruction(31 DOWNTO 27) = ADDI THEN
                use_imm_out <= '1';
            ELSE
                use_imm_out <= '0';
            END IF;
            IF instruction(31 DOWNTO 27) = JMP THEN
                use_jmp_out <= '1';
            ELSE
                use_jmp_out <= '0';
            END IF;
            IF instruction(31 DOWNTO 27) = NOP OR instruction(31 DOWNTO 27) = HALT OR instruction(31 DOWNTO 27) = JMP OR instruction(31 DOWNTO 27) = BRANCH OR instruction(31 DOWNTO 27) = STORE THEN
                reg_write_out <= '0';
            ELSE
                reg_write_out <= '1';
            END IF;
            IF instruction(31 DOWNTO 27) = LOAD THEN
                memory_read_out <= '1';
            ELSE
                memory_read_out <= '0';
            END IF;
            IF instruction(31 DOWNTO 27) = STORE THEN
                memory_write_out <= '1';
            ELSE
                memory_write_out <= '0';
            END IF;
            IF instruction(31 DOWNTO 27) = HALT THEN
                halt_out <= '1';
            ELSE
                halt_out <= '0';
            END IF;
        END IF;
    END PROCESS pre_fetch_P;
END ARCHITECTURE behavior;