library IEEE;
library work;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.instructions.all;

entity pre_fetch_reg is
    port(
        clk : in std_logic;
        next_pc_in, instruction : in signed(31 downto 0);
        reset : in std_logic;
        next_pc_out : out signed(31 downto 0);
        opcode_out, addr_a_out, addr_b_out, addr_dest_out, func_out : out signed(4 downto 0);
        imm_out : out signed(31 downto 0);
        use_imm_out : out std_logic;
        use_jmp_out : out std_logic;
        reg_write_out : out std_logic;
        memory_read_out, memory_write_out : out std_logic
    );
end entity pre_fetch_reg;

architecture behavior of pre_fetch_reg is
    begin 
    pre_fetch_P: process (clk, reset) is
    begin
        if rising_edge(clk) then
            if reset = '1' then
                next_pc_out <= to_signed(3, 32);
                addr_a_out <= (others =>'0');
                addr_b_out <= (others => '0');
                opcode_out <= (others => '0');
                addr_dest_out <= (others => '0');
                func_out <= (others => '0');
                imm_out <= (others => '0');
                use_imm_out <= '0';
                use_jmp_out <= '0';
                reg_write_out <= '0';
                memory_read_out <= '0';
                memory_write_out <= '0';
            else 
                next_pc_out <= next_pc_in;
                opcode_out <= instruction(31 downto 27);
                addr_dest_out <= instruction(26 downto 22);
                addr_a_out <= instruction(21 downto 17);
                addr_b_out <= instruction(16 downto 12);
                func_out <= instruction(4 downto 0);
                -- Instruction format is slightly different when branching
                if instruction(31 downto 27) = BRANCH then
                    imm_out <= instruction(21) & "000000000000000" & instruction(20 downto 5);
                else
                    imm_out <= instruction(16) & "000000000000000" & instruction(15 downto 0);
                end if;
                if instruction(31 downto 27) = BRANCH or instruction(31 downto 27) = LOADI or instruction(31 downto 27) = ADDI then
                    use_imm_out <= '1';
                else
                    use_imm_out <= '0';
                end if;
                if instruction(31 downto 27) = JMP then
                    use_jmp_out <= '1';
                else
                    use_jmp_out <= '0';
                end if;
                if instruction(31 downto 27) = NOP or instruction(31 downto 27) = HALT or instruction(31 downto 27) = JMP or instruction(31 downto 27) = BRANCH or instruction(31 downto 27) = STORE then
                    reg_write_out <= '0';
                else
                    reg_write_out <= '1';
                end if;
                if instruction(31 downto 27) = LOAD or instruction(31 downto 27) = LOADI then
                    memory_read_out <= '1';
                else
                    memory_read_out <= '0';
                end if;
                if  instruction(31 downto 27) = STORE then
                    memory_write_out <= '1';
                else
                    memory_write_out <= '0';
                end if;
            end if;
        end if;
    end process pre_fetch_P;
end architecture behavior;