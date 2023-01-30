LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY pre_mem_reg IS
    PORT (
        -- control
        reset : IN STD_LOGIC;
        clk : IN STD_LOGIC;
        -- in
        next_pc_in, res_in, val_d_in : IN signed(31 DOWNTO 0);
        dest_addr_in : IN signed(4 DOWNTO 0);
        use_jmp_in : IN STD_LOGIC;
        use_branch_in : IN STD_LOGIC;
        reg_write_in : IN STD_LOGIC;
        memory_read_in, memory_write_in : IN STD_LOGIC;
        halt_in : IN STD_LOGIC;
        -- out
        next_pc_out, res_out, val_d_out : OUT signed(31 DOWNTO 0);
        dest_addr_out : OUT signed(4 DOWNTO 0);
        use_jmp_out : OUT STD_LOGIC;
        use_branch_out : OUT STD_LOGIC;
        reg_write_out : OUT STD_LOGIC;
        memory_read_out, memory_write_out : OUT STD_LOGIC;
        halt_out : OUT STD_LOGIC
    );
END ENTITY pre_mem_reg;

ARCHITECTURE behavior OF pre_mem_reg IS
BEGIN
    pre_mem_P : PROCESS (clk, reset) IS
    BEGIN
        IF reset = '1' THEN
            next_pc_out <= to_signed(1, 32);
            res_out <= (OTHERS => '0');
            val_d_out <= (OTHERS => '0');
            dest_addr_out <= (OTHERS => '0');
            use_jmp_out <= '0';
            use_branch_out <= '0';
            reg_write_out <= '0';
            memory_read_out <= '0';
            memory_write_out <= '0';
            halt_out <= '0';
        END IF;

        IF rising_edge(clk) THEN
            IF use_jmp_in = '1' THEN
                next_pc_out <= val_d_in;
            ELSIF use_branch_in = '1' THEN
                next_pc_out <= res_in;
            ELSE
                next_pc_out <= next_pc_in;
            END IF;
            res_out <= res_in;
            val_d_out <= val_d_in;
            dest_addr_out <= dest_addr_in;
            use_jmp_out <= use_jmp_in;
            use_branch_out <= use_branch_in;
            reg_write_out <= reg_write_in;
            memory_read_out <= memory_read_in;
            memory_write_out <= memory_write_in;
            halt_out <= halt_in;
        END IF;
    END PROCESS pre_mem_P;
END ARCHITECTURE behavior;