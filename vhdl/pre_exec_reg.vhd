library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.instructions.all;

entity pre_exec_reg is
    port(
        -- control
        reset : in std_logic;
        clk : in std_logic;
        -- in
        pc_in, next_pc_in, val_a_in, val_b_in, val_d_in : in signed(31 downto 0);
        opcode_in, dest_addr_in, func_in : in signed(4 downto 0);
        imm_in : in signed(31 downto 0);
        use_imm_in : in std_logic;
        use_jmp_in : in std_logic;
        reg_write_in : in std_logic;
        memory_read_in, memory_write_in : in std_logic;
        halt_in : in std_logic;
        -- out
        next_pc_out, val_a_out, val_b_out, val_d_out : out signed(31 downto 0);
        opcode_out, dest_addr_out, func_out : out signed(4 downto 0);
        imm_out : out signed(31 downto 0);
        use_imm_out : out std_logic;
        use_jmp_out : out std_logic;
        reg_write_out : out std_logic;
        memory_read_out, memory_write_out : out std_logic;
        halt_out : out std_logic
    );
end entity pre_exec_reg;

architecture behavior of pre_exec_reg is
    begin 
    pre_exec_P: process (clk, reset) is
    begin
        if reset = '1' then
            val_a_out <= (others => '0');
            next_pc_out <= to_signed(2, 32);
            val_b_out <= (others => '0');
            val_d_out <= (others => '0');
            opcode_out <= (others => '0');
            dest_addr_out <= (others => '0');
            func_out <= (others => '0');
            imm_out <= (others => '0');
            use_imm_out <= '0';
            use_jmp_out <= '0';
            reg_write_out <= '0';
            memory_read_out <= '0';
            memory_write_out <= '0';
            halt_out <= '0';
        end if;
        if rising_edge(clk) then
            next_pc_out <= next_pc_in;
            if opcode_in = BRANCH then
                val_a_out <= pc_in;
            else
                val_a_out <= val_a_in;
            end if;
            if use_imm_in = '1' then
                val_b_out <= imm_in;
            else
                val_b_out <= val_b_in;
            end if;
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
        end if;
    end process pre_exec_P;
end architecture behavior;