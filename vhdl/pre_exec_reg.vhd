LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;
USE work.instructions.ALL;

ENTITY pre_exec_reg IS
    PORT (
        -- control
        reset : IN STD_LOGIC;
        clk : IN STD_LOGIC;
        -- in
        pc_in, next_pc_in, val_a_in, val_b_in, val_d_in : IN signed(31 DOWNTO 0);
        opcode_in, dest_addr_in, func_in : IN signed(4 DOWNTO 0);
        imm_in : IN signed(31 DOWNTO 0);
        use_imm_in : IN STD_LOGIC;
        use_jmp_in : IN STD_LOGIC;
        reg_write_in : IN STD_LOGIC;
        memory_read_in, memory_write_in : IN STD_LOGIC;
        halt_in : IN STD_LOGIC;
        -- out
        next_pc_out, val_a_out, val_b_out, val_d_out : OUT signed(31 DOWNTO 0);
        opcode_out, dest_addr_out, func_out : OUT signed(4 DOWNTO 0);
        imm_out : OUT signed(31 DOWNTO 0);
        use_imm_out : OUT STD_LOGIC;
        use_jmp_out : OUT STD_LOGIC;
        reg_write_out : OUT STD_LOGIC;
        memory_read_out, memory_write_out : OUT STD_LOGIC;
        halt_out : OUT STD_LOGIC
    );
END ENTITY pre_exec_reg;

ARCHITECTURE behavior OF pre_exec_reg IS
BEGIN
    pre_exec_P : PROCESS (clk, reset) IS
    BEGIN
        IF reset = '1' THEN
            val_a_out <= (OTHERS => '0');
            next_pc_out <= to_signed(2, 32);
            val_b_out <= (OTHERS => '0');
            val_d_out <= (OTHERS => '0');
            opcode_out <= (OTHERS => '0');
            dest_addr_out <= (OTHERS => '0');
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
            next_pc_out <= next_pc_in;
            IF opcode_in = BRANCH THEN
                val_a_out <= pc_in;
            ELSE
                val_a_out <= val_a_in;
            END IF;
            IF use_imm_in = '1' THEN
                val_b_out <= imm_in;
            ELSE
                val_b_out <= val_b_in;
            END IF;
            val_d_out <= val_d_in;
            opcode_out <= opcode_in;
            dest_addr_out <= dest_addr_in;
            func_out <= func_in;
            imm_out <= imm_in;
            use_imm_out <= use_imm_in;
            use_jmp_out <= use_jmp_in;
            reg_write_out <= reg_write_in;
            memory_read_out <= memory_read_in;
            memory_write_out <= memory_write_in;
            halt_out <= halt_in;
        END IF;
    END PROCESS pre_exec_P;
END ARCHITECTURE behavior;