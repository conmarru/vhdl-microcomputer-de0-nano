library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity pre_mem_reg is
    port(
        clk : in std_logic;
        next_pc_in, res_in : in signed(31 downto 0);
        dest_addr_in : in signed(4 downto 0);
        use_jmp_in : in std_logic;
        reg_write_in : in std_logic;
        memory_read_in, memory_write_in : in std_logic;
        next_pc_out, res_out : out signed(31 downto 0);
        dest_addr_out : out signed(4 downto 0);
        use_jmp_out : out std_logic;
        reg_write_out : out std_logic;
        memory_read_out, memory_write_out : out std_logic;
        reset : in std_logic
    );
end entity pre_mem_reg;

architecture behavior of pre_mem_reg is
    begin 
    pre_mem_P: process (clk, reset) is
    begin
        if rising_edge(clk) then
            if reset = '1' then
                next_pc_out <= to_signed(0, 32);
                res_out <= (others =>'0');
                dest_addr_out <= (others => '0');
                use_jmp_out <= '0';
                reg_write_out <= '0';
                memory_read_out <= '0';
                memory_write_out <= '0';
            else 
                next_pc_out <= next_pc_in;
                res_out <= res_in;
                dest_addr_out <= dest_addr_in;
                use_jmp_out <= use_jmp_in;
                reg_write_out <= reg_write_in;
                memory_read_out <= memory_read_in;
                memory_write_out <= memory_write_in;
            end if;
        end if;
    end process pre_mem_P;
end architecture behavior;