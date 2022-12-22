library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

use work.instructions.ALL;

entity extract_info is
    Port (
        clk : in std_logic;
        instruction : in signed(31 downto 0);
        next_pc_in : in signed(31 downto 0);
        next_pc_out : out signed(31 downto 0);
        opcode, dest_addr, op1_addr, op2_addr, func : out signed(4 downto 0);
        imm : out signed(16 downto 0);
        use_imm : out std_logic;
        use_jmp : out std_logic;
        reg_write : out std_logic;
        memory_read, memory_write : out std_logic
    );
end extract_info;

architecture behavior of extract_info is
begin
extract_P: process (clk, instruction, next_pc_in) is
begin
        next_pc_out <= next_pc_in;
        opcode_out <= instruction(31 downto 27);
        dest_addr <= instruction(26 downto 22);
        op1_addr <= instruction(21 downto 17);
        op2_addr <= instruction(16 downto 12);
        func <= instruction(4 downto 0);
        imm <= instruction(16 downto 0);

        if instruction(31 downto 27) = LOADI or instruction(31 downto 27) = ADDI then
                use_imm <= '1';
            else
                use_imm <= '0';
            end if;

            if instruction(31 downto 27) = JMP then
                use_jmp <= '1';
            else
                use_jmp <= '0';
            end if;

            if instruction(31 downto 27) = JMP or instruction(31 downto 27) = BRANCH or instruction(31 downto 27) = STORE then
                reg_write <= '0';
            else
                reg_write <= '1';
            end if;

            if instruction(31 downto 27) = LOAD then
                memory_read <= '1';
                memory_write <= '0';
            elsif instruction(31 downto 27) = STORE then
                memory_read <= '0';
                memory_write <= '1';
            else
                memory_read <= '0';
                memory_write <= '0';
            end if;
    end process extract_P;

end architecture behavior;